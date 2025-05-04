package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

// Constants
const (
	NrOfTravelers = 15
	BoardWidth    = 15
	BoardHeight   = 15

	MinSteps = 10
	MaxSteps = 100

	MinDelay = 10 * time.Millisecond
	MaxDelay = 50 * time.Millisecond
	LockWait = 100 * time.Millisecond // Timeout before giving up on a move
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
	Id       int
	Symbol   rune
	Pos      Position
	Steps    int
	Traces   []Trace
	RandGen  *rand.Rand
	MoveDir  Position // Fixed movement direction
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

	// Lock the starting position
	board[t.Pos.X][t.Pos.Y].Lock()
	t.storeTrace(startTime)

	// Random number of steps
	t.Steps = MinSteps + int(t.RandGen.Float64()*float64(MaxSteps-MinSteps))

	for i := 0; i < t.Steps; i++ {
		time.Sleep(MinDelay + time.Duration(t.RandGen.Float64()*float64(MaxDelay-MinDelay)))

		oldPos := t.Pos
		candidate := Position{
			X: (t.Pos.X + t.MoveDir.X + BoardWidth) % BoardWidth,
			Y: (t.Pos.Y + t.MoveDir.Y + BoardHeight) % BoardHeight,
		}

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
			t.storeTrace(startTime)
			reportCh <- t.Traces
			return
		}
	}

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
		randGen := rand.New(rand.NewSource(seed))

		moveDir := Position{}
		if i%2 == 0 {
			// Parzysty: losowy kierunek w pionie
			if randGen.Intn(2) == 0 {
				moveDir.Y = -1
			} else {
				moveDir.Y = 1
			}
		} else {
			// Nieparzysty: losowy kierunek w poziomie
			if randGen.Intn(2) == 0 {
				moveDir.X = -1
			} else {
				moveDir.X = 1
			}
		}

		t := &Traveler{
			Id:      i,
			Symbol:  symbol,
			Pos:     Position{X: i, Y: i},
			Traces:  make([]Trace, 0, MaxSteps+1),
			RandGen: randGen,
			MoveDir: moveDir,
		}

		go travelerRoutine(t, startTime, reportCh, &wg)
		symbol++
	}

	go printerRoutine(reportCh, done)

	wg.Wait()
	close(reportCh)
	<-done
}

