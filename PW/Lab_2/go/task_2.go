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
)

type Message struct {
	messageType     int
	responseChannel chan int
}

func createMessage(msgType int, responseChannel chan int) Message {
	return Message{
		messageType:     msgType,
		responseChannel: responseChannel,
	}
}

type Tile struct {
	status    int
	ch        chan Message
	waitQueue []chan int
}

func tileRoutine(ch chan Message) {
	tile := Tile{
		status:    0,
		ch:        ch,
		waitQueue: make([]chan int, 0),
	}

	for {
		req := <-tile.ch

		switch req.messageType {
		case 0:
			tile.status = 0
			for _, ch := range tile.waitQueue {
				ch <- 12
			}
			tile.waitQueue = nil
		case 1:
			// Respond based on tile status
			if tile.status == 0 {
				tile.status = 1
				req.responseChannel <- 11
			} else if tile.status == 1 {
				tile.waitQueue = append(tile.waitQueue, req.responseChannel)
			}

		case 2:
			if tile.status == 0 {
				tile.status = 2
				req.responseChannel <- 11
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
				req.responseChannel <- 11
			} else {
				req.responseChannel <- 13
			}

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

func wildTravelerRoutine(t *Traveler, startTime time.Time, reportCh chan<- []Trace) {
	t.storeTrace(startTime)

	response := make(chan int)

	lifeTimer := time.After(WildTravelerLifeTime)
wildLoop:
	for {
		select {
		case <-lifeTimer:
			// Wild traveler expired
			//t.Symbol = '.'
			//t.Pos = Position{BoardHeight, BoardWidth}
			t.storeTrace(startTime)
			reportCh <- t.Traces
			msg := createMessage(0, response)
			channels[t.Pos.Y][t.Pos.X] <- msg
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

	fmt.Printf("-1 %d %d %d\n", NrOfTravelers + 50, BoardWidth, BoardHeight)

	reportCh := make(chan []Trace, NrOfTravelers)
	done := make(chan struct{})

	var wg sync.WaitGroup
	wg.Add(NrOfTravelers)

	symbol := 'A'
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

	go printerRoutine(reportCh, done)

	// Create channel to signal when WaitGroup is done
	doneWg := make(chan struct{})

	go func() {
		wg.Wait()
		close(doneWg)
	}()

	ticker := time.NewTicker(WildTravelerSpawnDelay)
	defer ticker.Stop()

	symbol = 48
	id := NrOfTravelers
wildTravelerSpwanLoop:
	for {
		select {
		case <-doneWg:
			time.Sleep(WildTravelerLifeTime)
			reportCh <- []Trace{
				{
					Id: -1,
				},
			}
			break wildTravelerSpwanLoop

		case <-ticker.C:
			seed := time.Now().UnixNano() + int64(id)
			rng := rand.New(rand.NewSource(seed))

			var pos Position
			for {
				pos = Position{
					X: int(rng.Float64() * float64(BoardWidth)),
					Y: int(rng.Float64() * float64(BoardHeight)),
				}

				respCh := make(chan int, 1)
				msg := createMessage(21, respCh)

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
			go wildTravelerRoutine(t, startTime, reportCh)
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
