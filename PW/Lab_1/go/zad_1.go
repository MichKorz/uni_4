package main

import (
	"fmt"
	"math"
	"math/rand"
	"sync"
	"time"
)

// Constants similar to the Ada program
const (
	NrOfTravelers = 15
	MinSteps      = 10
	MaxSteps      = 100

	MinDelay = 10 * time.Millisecond
	MaxDelay = 50 * time.Millisecond

	BoardWidth  = 15
	BoardHeight = 15
)

// Position represents a point on the torus board.
type Position struct {
	X, Y int
}

// Trace represents a single step trace of a traveler.
type Trace struct {
	TimeStamp time.Duration
	Id        int
	Position  Position
	Symbol    rune
}

// Traveler holds a traveler's state.
type Traveler struct {
	Id      int
	Symbol  rune
	Pos     Position
	Steps   int
	Traces  []Trace
	RandGen *rand.Rand
}

// move functions with torus behavior
func moveUp(pos *Position) {
	pos.Y = (pos.Y + BoardHeight - 1) % BoardHeight
}

func moveDown(pos *Position) {
	pos.Y = (pos.Y + 1) % BoardHeight
}

func moveLeft(pos *Position) {
	pos.X = (pos.X + BoardWidth - 1) % BoardWidth
}

func moveRight(pos *Position) {
	pos.X = (pos.X + 1) % BoardWidth
}

// makeStep chooses a random direction and moves the traveler.
func (t *Traveler) makeStep() {
	// Random integer in [0,3]
	N := int(math.Floor(4.0 * t.RandGen.Float64()))
	switch N {
	case 0:
		moveUp(&t.Pos)
	case 1:
		moveDown(&t.Pos)
	case 2:
		moveLeft(&t.Pos)
	case 3:
		moveRight(&t.Pos)
	default:
		fmt.Printf("Unexpected direction %d\n", N)
	}
}

// storeTrace appends a trace with the current timestamp.
func (t *Traveler) storeTrace(startTime time.Time) {
	trace := Trace{
		TimeStamp: time.Since(startTime),
		Id:        t.Id,
		Position:  t.Pos,
		Symbol:    t.Symbol,
	}
	t.Traces = append(t.Traces, trace)
}

// travelerRoutine simulates a traveler’s movement.
// Once done, it sends the traveler's trace slice to the reportCh.
func travelerRoutine(t *Traveler, startTime time.Time, reportCh chan<- []Trace, wg *sync.WaitGroup) {
	defer wg.Done()

	// Set a random starting position on the board.
	t.Pos = Position{
		X: int(t.RandGen.Float64() * float64(BoardWidth)),
		Y: int(t.RandGen.Float64() * float64(BoardHeight)),
	}
	t.storeTrace(startTime) // store initial position

	// Decide on the number of steps randomly
	stepsRange := MaxSteps - MinSteps
	t.Steps = MinSteps + int(t.RandGen.Float64()*float64(stepsRange))

	// Wait for a start signal if needed.
	// In this design, travelers start immediately.

	// Movement loop: take a step, store the trace, and wait a random delay.
	for i := 0; i < t.Steps; i++ {
		// Random delay between MinDelay and MaxDelay.
		delayDuration := MinDelay + time.Duration(t.RandGen.Float64()*float64(MaxDelay-MinDelay))
		time.Sleep(delayDuration)

		t.makeStep()
		t.storeTrace(startTime)
	}
	// Send the complete trace sequence to the printer
	reportCh <- t.Traces
}

// printerRoutine receives trace slices from travelers and prints them.
func printerRoutine(reportCh <-chan []Trace, done chan<- struct{}) {
	for i := 0; i < NrOfTravelers; i++ {
		traces := <-reportCh
		for _, trace := range traces {
			// Print timestamp, traveler id, position X, position Y, and symbol.
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

	// Print simulation parameters
	fmt.Printf("-1 %d %d %d\n", NrOfTravelers, BoardWidth, BoardHeight)

	reportCh := make(chan []Trace, NrOfTravelers)
	done := make(chan struct{})

	// WaitGroup to synchronize all traveler goroutines.
	var wg sync.WaitGroup
	wg.Add(NrOfTravelers)

	// Create travelers with unique seeds and symbols starting from 'A'.
	symbol := 'A'
	for i := 0; i < NrOfTravelers; i++ {
		// Each traveler gets its own random generator seeded with a combination of time and id.
		seed := time.Now().UnixNano() + int64(i)
		t := &Traveler{
			Id:      i,
			Symbol:  symbol,
			Traces:  make([]Trace, 0, MaxSteps+1),
			RandGen: rand.New(rand.NewSource(seed)),
		}
		// Start each traveler as a goroutine.
		go travelerRoutine(t, startTime, reportCh, &wg)
		symbol++ // next symbol
	}

	// Start printer goroutine.
	go printerRoutine(reportCh, done)

	// Wait for all traveler routines to finish.
	wg.Wait()
	// Close the report channel now that all reports have been sent.
	close(reportCh)
	<-done

	// The program is open for further modifications.
	// For instance, you can add synchronization on a shared board "tile" structure here.
}

