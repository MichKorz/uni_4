package main

import (
	"fmt"
	"math/rand"
	"sync"
	"sync/atomic" 
	"time"
)

// Configuration Constants
const (
	NumProcesses = 2 
	MinSteps     = 5 
	MaxSteps     = 10 

	// Delays to simulate work
	MinDelay = 10 * time.Millisecond
	MaxDelay = 50 * time.Millisecond
)

// Process States
type ProcessState int

const (
	StateLocalSection ProcessState = iota
	StateEntryProtocol
	StateCriticalSection
	StateExitProtocol
)

// Stringer for ProcessState
func (ps ProcessState) String() string {
	switch ps {
	case StateLocalSection:
		return "LOCAL_SECTION"
	case StateEntryProtocol:
		return "ENTRY_PROTOCOL"
	case StateCriticalSection:
		return "CRITICAL_SECTION"
	case StateExitProtocol:
		return "EXIT_PROTOCOL"
	default:
		return "UNKNOWN_STATE"
	}
}

type Trace struct {
	Timestamp    time.Duration 
	ProcessID    int
	ProcessState ProcessState
	Symbol       rune
}

var globalStartTime time.Time

var (
	// flagPeterson[i] is 1 if process i wants to enter CS, 0 otherwise
	flagPeterson [NumProcesses]int32 
	// turnPeterson indicates whose turn it is if both want to enter
	turnPeterson int32 
)

func processTask(
	id int,
	symbol rune,
	numOwnSteps int,
	batchTraceChannel chan<- []Trace,
	wg *sync.WaitGroup,
) {
	defer wg.Done()

	var localTraces []Trace 

	addLocalTrace := func(state ProcessState) {
		localTraces = append(localTraces, Trace{
			Timestamp:    time.Since(globalStartTime),
			ProcessID:    id,
			ProcessState: state,
			Symbol:       symbol,
		})
	}

	otherID := int32(1 - id) // The ID of the other process, as int32

	addLocalTrace(StateLocalSection) // Initial state

	for step := 0; step < numOwnSteps; step++ {
		//Local Section
		delay := MinDelay + time.Duration(rand.Int63n(int64(MaxDelay-MinDelay)))
		time.Sleep(delay)

		// Entry Protocol (Peterson's Algorithm)
		addLocalTrace(StateEntryProtocol)

		atomic.StoreInt32(&flagPeterson[id], 1)         // Set flag[id] = true (1)
		atomic.StoreInt32(&turnPeterson, otherID) // Give turn to the other process

		// Wait while the other process wants to enter AND it's its turn
		for atomic.LoadInt32(&flagPeterson[otherID]) == 1 && atomic.LoadInt32(&turnPeterson) == otherID {
			time.Sleep(1 * time.Microsecond) // Busy wait with a small yield
		}

		// Critical Section
		addLocalTrace(StateCriticalSection)
		criticalDelay := MinDelay + time.Duration(rand.Int63n(int64(MaxDelay-MinDelay)))
		time.Sleep(criticalDelay)

		// Exit Protocol
		addLocalTrace(StateExitProtocol)

		atomic.StoreInt32(&flagPeterson[id], 0) // Set flag[id] = false (0)

		addLocalTrace(StateLocalSection) 
	}

	batchTraceChannel <- localTraces
}


func printerTask(batchTraceChannel <-chan []Trace, numProcesses int) {

	for traceBatch := range batchTraceChannel {
		for _, t := range traceBatch {
			fmt.Printf("%-18d %-2d %-2d %-2d %c\n",
				t.Timestamp.Nanoseconds(),
				t.ProcessID,
				t.ProcessID,         
				int(t.ProcessState), 
				t.Symbol)
		}
	}


	boardWidth := numProcesses
	boardHeight := len([]ProcessState{StateLocalSection, StateEntryProtocol, StateCriticalSection, StateExitProtocol})

	fmt.Printf("-1 %d %d %d ", numProcesses, boardWidth, boardHeight)
	states := []ProcessState{StateLocalSection, StateEntryProtocol, StateCriticalSection, StateExitProtocol}
	for _, s := range states {
		fmt.Printf("%s;", s.String())
	}
	fmt.Println("ALGORITHM=Peterson;") 
}

func main() {
	globalStartTime = time.Now()
	rand.Seed(time.Now().UnixNano())

	var wg sync.WaitGroup
	batchTraceChannel := make(chan []Trace, NumProcesses)

	go printerTask(batchTraceChannel, NumProcesses)

	for i := 0; i < NumProcesses; i++ { // This loop will run for i=0 and i=1
		wg.Add(1)
		symbol := rune('A' + i)
		numOwnSteps := MinSteps + rand.Intn(MaxSteps-MinSteps+1)

		go processTask(
			i,
			symbol,
			numOwnSteps,
			batchTraceChannel,
			&wg,
		)
	}

	wg.Wait()         
	close(batchTraceChannel)

	time.Sleep(200 * time.Millisecond) 
}

