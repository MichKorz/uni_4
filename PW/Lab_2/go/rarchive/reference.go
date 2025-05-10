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
)

// Board with mutex tiles
var board [BoardWidth][BoardHeight]sync.Mutex

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

// travelerRoutine handles movement and deadlock resolution
func travelerRoutine(t *Traveler, startTime time.Time, reportCh chan<- []Trace, wg *sync.WaitGroup) {
	defer wg.Done()

	// Set a random start position and lock the tile
	t.Pos = Position{
		X: int(t.RandGen.Float64() * float64(BoardWidth)),
		Y: int(t.RandGen.Float64() * float64(BoardHeight)),
	}
	board[t.Pos.X][t.Pos.Y].Lock()
	t.storeTrace(startTime)

	// Random number of steps
	t.Steps = MinSteps + int(t.RandGen.Float64()*float64(MaxSteps-MinSteps))

	for i := 0; i < t.Steps; i++ {
		time.Sleep(MinDelay + time.Duration(t.RandGen.Float64()*float64(MaxDelay-MinDelay)))

		oldPos := t.Pos
		candidate := t.candidateMove()

		// Attempt to lock the new tile with a timeout
		locked := make(chan bool, 1)

		go func() {
			board[candidate.X][candidate.Y].Lock()
			locked <- true
		}()

		select {
		case <-locked: // Successfully acquired the lock
			board[oldPos.X][oldPos.Y].Unlock()
			t.Pos = candidate
			t.storeTrace(startTime)
		case <-time.After(LockWait): // Timeout occurred, traveler "gives up"
			t.Symbol = rune(t.Symbol + 32) // Convert to lowercase (e.g., 'A' -> 'a')
			t.storeTrace(startTime)        // Store this final state with the updated symbol
			reportCh <- t.Traces           // Send trace as if it finished
			return                         // Exit routine early
		}
	}

	// Send the full trace
	reportCh <- t.Traces
}

// printerRoutine collects and prints reports
func printerRoutine(reportCh <-chan []Trace, done chan<- struct{}) {
	for i := 0; i < NrOfTravelers; i++ {
		traces := <-reportCh
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

func main() {
	startTime := time.Now()

	fmt.Printf("-1 %d %d %d\n", NrOfTravelers, BoardWidth, BoardHeight)

	reportCh := make(chan []Trace, NrOfTravelers)
	done := make(chan struct{})

	var wg sync.WaitGroup
	wg.Add(NrOfTravelers)

	symbol := 'A'
	for i := 0; i < NrOfTravelers; i++ {
		seed := time.Now().UnixNano() + int64(i)
		t := &Traveler{
			Id:      i,
			Symbol:  symbol,
			Traces:  make([]Trace, 0, MaxSteps+1),
			RandGen: rand.New(rand.NewSource(seed)),
		}
		go travelerRoutine(t, startTime, reportCh, &wg)
		symbol++
	}

	go printerRoutine(reportCh, done)

	wg.Wait()
	close(reportCh)
	<-done
}
