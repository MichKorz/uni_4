package main

import (
	"fmt"
	"math"
	"math/rand"
	"sync"
	"time"
)

// Constants
const (
	NrOfTravelers = 15
	MinSteps      = 10
	MaxSteps      = 100

	MinDelay = 10 * time.Millisecond
	MaxDelay = 50 * time.Millisecond
	LockWait = 100 * time.Millisecond // Timeout before giving up on a move

	BoardWidth  = 15
	BoardHeight = 15

	WildTravelerSpawnDelay = 50 * time.Millisecond
	WildTravelerLifeTime   = 300 * time.Millisecond

	DecayTime = 80 * time.Millisecond
)

type Message struct {
	messageType       int
	responseChannel   chan int
	additionalChannel chan int
}

func createMessage(msgType int, responseChannel chan int) Message {
	return Message{
		messageType:     msgType,
		responseChannel: responseChannel,
	}
}

type Tile struct {
	status         int
	ch             chan Message
	waitQueue      []chan int
	wildTravelerCh chan int
	trapChannel    chan int
}

func tileRoutine(ch chan Message) {
	tile := Tile{
		status:         0,
		ch:             ch,
		waitQueue:      make([]chan int, 0),
		wildTravelerCh: nil,
	}

	for {
		req := <-tile.ch

		switch req.messageType {
		case 0:
			tile.status = 0
			for _, ch := range tile.waitQueue {
    			select {
    				case ch <- 12: // Or appropriate message
        			// Message sent successfully
			    default:
				// Traveler likely timed out and is no longer listening.
				// This prevents the tileRoutine from blocking.
			    }
			}	
			tile.waitQueue = nil
			tile.wildTravelerCh = nil

		case 1:
			if tile.status == 0 {
				tile.status = 1
				req.responseChannel <- 11
			} else if tile.status == 1 {
				tile.waitQueue = append(tile.waitQueue, req.responseChannel)
			} else if tile.status == 2 {
				tile.waitQueue = append(tile.waitQueue, req.responseChannel)
				tile.wildTravelerCh <- 30
			} else if tile.status == 5 {
				req.responseChannel <- 41
				tile.status = 6
			} else if tile.status == 6 {
				tile.waitQueue = append(tile.waitQueue, req.responseChannel)
			}

		case 2:
			if tile.status == 0 {
				tile.status = 2
				tile.wildTravelerCh = req.responseChannel
				req.responseChannel <- 11
			} else if tile.status == 5 {
				req.responseChannel <- 41
				tile.status = 6
			} else {
				req.responseChannel <- 13
			}

		case 20:
			if tile.status == 0 {
				tile.status = 1
				req.responseChannel <- 11
			} else {
				req.responseChannel <- 13
			}

		case 21:
			if tile.status == 0 {
				tile.status = 2
				tile.wildTravelerCh = req.additionalChannel
				req.responseChannel <- 11
			} else {
				req.responseChannel <- 13
			}
		case 40:
			tile.trapChannel = req.additionalChannel
			tile.status = 5
		case 8:
			tile.trapChannel <- 8
			tile.status = 5
			for _, ch := range tile.waitQueue {
				ch <- 12
			}
			tile.waitQueue = nil
			tile.wildTravelerCh = nil
		case 9:
			tile.trapChannel <- 9

		default:
			// Echo back received message type
			req.responseChannel <- req.messageType
		}
	}
}

// Position represents a 2D coordinate
type Position struct {
	X, Y int
}

// Trace represents a single travel step
type Trace struct {
	TimeStamp time.Duration
	Id        int
	Position  Position
	Symbol    rune
}

// Traveler struct
type Traveler struct {
	Id      int
	Symbol  rune
	Pos     Position
	Steps   int
	Traces  []Trace
	RandGen *rand.Rand
}

// CandidateMove determines the next position
func (t *Traveler) candidateMove() Position {
	candidate := t.Pos
	N := int(math.Floor(4.0 * t.RandGen.Float64()))
	switch N {
	case 0:
		candidate.Y = (candidate.Y + BoardHeight - 1) % BoardHeight
	case 1:
		candidate.Y = (candidate.Y + 1) % BoardHeight
	case 2:
		candidate.X = (candidate.X + BoardWidth - 1) % BoardWidth
	case 3:
		candidate.X = (candidate.X + 1) % BoardWidth
	}
	return candidate
}

// StoreTrace appends a travel record
func (t *Traveler) storeTrace(startTime time.Time) {
	t.Traces = append(t.Traces, Trace{
		TimeStamp: time.Since(startTime),
		Id:        t.Id,
		Position:  t.Pos,
		Symbol:    t.Symbol,
	})
}

func travelerRoutine(t *Traveler, startTime time.Time, reportCh chan<- []Trace, wg *sync.WaitGroup) {
	defer wg.Done()

	t.storeTrace(startTime)

	// Random number of steps
	t.Steps = MinSteps + int(t.RandGen.Float64()*float64(MaxSteps-MinSteps))

	// Create a response channel for receiving the message
	response := make(chan int)

	time.Sleep(MinDelay + time.Duration(t.RandGen.Float64()*float64(MaxDelay-MinDelay)))

mainLoop:
	for i := 0; i < t.Steps; i++ {

		oldPos := t.Pos
		candidate := t.candidateMove()

	sendMessage:
		for {
			msg := createMessage(1, response)
			channels[candidate.Y][candidate.X] <- msg

			select {
			case resp := <-response:
				switch resp {
				case 11:
					t.Pos = candidate
					t.storeTrace(startTime)
					msg := createMessage(0, response)
					channels[oldPos.Y][oldPos.X] <- msg
					break sendMessage
				case 12:
					continue sendMessage
				case 14:
					i--
					continue mainLoop
				case 41:
					t.Pos = candidate
					t.Symbol = rune(t.Symbol + 32)
					t.storeTrace(startTime)
					reportCh <- t.Traces
					msg := createMessage(0, response)
					channels[oldPos.Y][oldPos.X] <- msg
					time.Sleep(DecayTime)
					msg = createMessage(8, response)
					channels[t.Pos.Y][t.Pos.X] <- msg
					return
				}
			case <-time.After(LockWait):
				t.Symbol = rune(t.Symbol + 32) // Convert to lowercase (e.g., 'A' -> 'a')
				t.storeTrace(startTime)        // Store this final state with the updated symbol
				break mainLoop                 // Exit routine early
			}
		}
		time.Sleep(MinDelay + time.Duration(t.RandGen.Float64()*float64(MaxDelay-MinDelay)))
	}

	// Send the full trace
	reportCh <- t.Traces
}

func wildTravelerRoutine(t *Traveler, startTime time.Time, reportCh chan<- []Trace, response chan int) {
	t.storeTrace(startTime)

	lifeTimer := time.After(WildTravelerLifeTime)
wildLoop:
	for {
		select {
		case <-lifeTimer:
			// Wild traveler expired
			msg := createMessage(0, response)
			channels[t.Pos.Y][t.Pos.X] <- msg
			t.Pos = Position{X: BoardWidth, Y: BoardHeight}
			t.storeTrace(startTime)
			reportCh <- t.Traces
			return

		case msg := <-response:
			if msg == 30 {
				directions := []Position{
					{X: 0, Y: -1}, // Up
					{X: 0, Y: 1},  // Down
					{X: -1, Y: 0}, // Left
					{X: 1, Y: 0},  // Right
				}

				for _, d := range directions {
					newX := (t.Pos.X + d.X + BoardWidth) % BoardWidth
					newY := (t.Pos.Y + d.Y + BoardHeight) % BoardHeight

					msg := createMessage(2, response)
					channels[newY][newX] <- msg

					select {
					case resp := <-response:
						if resp == 11 {
							msg := createMessage(0, response)
							channels[t.Pos.Y][t.Pos.X] <- msg
							t.Pos = Position{X: newX, Y: newY}
							t.storeTrace(startTime)
							continue wildLoop
						} else if resp == 13 {
							continue
						} else if resp == 41 {
							msg := createMessage(0, response)
							channels[t.Pos.Y][t.Pos.X] <- msg
							t.Pos = Position{X: newX, Y: newY}
							t.Symbol = 47
							t.storeTrace(startTime)
							reportCh <- t.Traces
							time.Sleep(DecayTime)
							msg = createMessage(8, response)
							channels[t.Pos.Y][t.Pos.X] <- msg
							return
						}
					case <-time.After(LockWait):
						// Ignore timeouts or stray messages (like type 30)
						continue
					}
				}

				// All directions failed
				msg := createMessage(14, response)
				channels[t.Pos.Y][t.Pos.X] <- msg
			}
		}
	}
}

// printerRoutine collects and prints reports
func printerRoutine(reportCh <-chan []Trace, done chan<- struct{}) {
	for {
		traces := <-reportCh
		if len(traces) == 1 && traces[0].Id == -1 {
			break
		}
		for _, trace := range traces {
			fmt.Printf("%.6f %d %d %d %c\n",
				trace.TimeStamp.Seconds(),
				trace.Id,
				trace.Position.X,
				trace.Position.Y,
				trace.Symbol)
		}
	}
	done <- struct{}{}
}

func trapRoutine(t *Traveler, startTime time.Time, reportCh chan<- []Trace, response chan int) {
	t.storeTrace(startTime)
	// Wait for messages from the response channel
	for {
		select {
		case msg := <-response:
			switch msg {
			case 8:
				// Store the trace when response is 8
				t.storeTrace(startTime)
			case 9:
				// Report the traces and return when response is 9
				reportCh <- t.Traces
				return
			}
		}
	}
}

// Global 2D channel grid
var channels [][]chan Message

func main() {
	// Set up channel grid and tile goroutines
	channels = make([][]chan Message, BoardHeight)
	for i := range channels {
		channels[i] = make([]chan Message, BoardWidth)
		for j := range channels[i] {
			ch := make(chan Message)
			channels[i][j] = ch
			go tileRoutine(ch)
		}
	}

	startTime := time.Now()
	reportCh := make(chan []Trace, NrOfTravelers)
	done := make(chan struct{})
	go printerRoutine(reportCh, done)

	// Generate 10 distinct random points on the board
	points := make(map[Position]struct{})
	rng := rand.New(rand.NewSource(time.Now().UnixNano()))

	for len(points) < 10 {
		p := Position{
			X: rng.Intn(BoardWidth),
			Y: rng.Intn(BoardHeight),
		}
		points[p] = struct{}{} // Only adds if the point doesn't already exist
	}

	symbol := '#'
	id := NrOfTravelers
	trapChannels := make([]chan int, 0, 10)

	for p := range points {
		pos := Position{X: p.X, Y: p.Y}
		trapChannel := make(chan int)
		trapChannels = append(trapChannels, trapChannel) // Store for later

		msg := Message{
			messageType:       40,
			responseChannel:   nil,
			additionalChannel: trapChannel,
		}
		channels[pos.Y][pos.X] <- msg

		t := &Traveler{
			Id:      id,
			Symbol:  symbol,
			Traces:  make([]Trace, 0, MaxSteps+1),
			RandGen: nil,
			Pos:     pos,
		}
		id++
		go trapRoutine(t, startTime, reportCh, trapChannel)
	}

	fmt.Printf("-1 %d %d %d\n", NrOfTravelers+50, BoardWidth, BoardHeight)

	var wg sync.WaitGroup
	wg.Add(NrOfTravelers)

	symbol = 'A'
	for i := 0; i < NrOfTravelers; i++ {
		seed := time.Now().UnixNano() + int64(i)
		rng := rand.New(rand.NewSource(seed))

		var pos Position
		for {
			pos = Position{
				X: int(rng.Float64() * float64(BoardWidth)),
				Y: int(rng.Float64() * float64(BoardHeight)),
			}

			respCh := make(chan int, 1)
			msg := createMessage(20, respCh)

			channels[pos.Y][pos.X] <- msg

			resp := <-respCh
			if resp == 11 {
				break
			}
			if resp == 13 {
				continue
			}
		}
		t := &Traveler{
			Id:      i,
			Symbol:  symbol,
			Traces:  make([]Trace, 0, MaxSteps+1),
			RandGen: rand.New(rand.NewSource(seed)),
			Pos:     pos,
		}
		go travelerRoutine(t, startTime, reportCh, &wg)
		symbol++
	}

	// Create channel to signal when WaitGroup is done
	doneWg := make(chan struct{})

	go func() {
		wg.Wait()
		close(doneWg)
	}()

	ticker := time.NewTicker(WildTravelerSpawnDelay)
	defer ticker.Stop()

	symbol = 48
	id = NrOfTravelers + 10
wildTravelerSpwanLoop:
	for {
		select {
		case <-doneWg:
			time.Sleep(WildTravelerLifeTime)
			for _, ch := range trapChannels {
				ch <- 9
			}

			time.Sleep(WildTravelerLifeTime * 5)
			reportCh <- []Trace{
				{
					Id: -1,
				},
			}
			break wildTravelerSpwanLoop

		case <-ticker.C:
			seed := time.Now().UnixNano() + int64(id)
			rng := rand.New(rand.NewSource(seed))

			wildTravelerRespCh := make(chan int)

			var pos Position
			for {
				pos = Position{
					X: int(rng.Float64() * float64(BoardWidth)),
					Y: int(rng.Float64() * float64(BoardHeight)),
				}

				respCh := make(chan int, 1)

				msg := Message{
					messageType:       21,
					responseChannel:   respCh,
					additionalChannel: wildTravelerRespCh,
				}

				channels[pos.Y][pos.X] <- msg

				resp := <-respCh
				if resp == 11 {
					break
				}
				if resp == 13 {
					continue
				}
			}
			t := &Traveler{
				Id:      id,
				Symbol:  symbol,
				Traces:  make([]Trace, 0, MaxSteps+1),
				RandGen: nil,
				Pos:     pos,
			}
			go wildTravelerRoutine(t, startTime, reportCh, wildTravelerRespCh)
			symbol++
			if symbol == 58 {
				symbol = 48
			}
			id++
		}
	}

	close(reportCh)
	<-done
}
