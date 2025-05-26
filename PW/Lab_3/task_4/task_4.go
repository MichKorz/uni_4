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

// Trace struct to record process activity
type Trace struct {
	Timestamp    time.Duration
	ProcessID    int
	ProcessState ProcessState
	Symbol       rune
}


var globalStartTime time.Time


var (
	// flagDekker[i] is 1 if process i wants to enter CS, 0 otherwise
	flagDekker [NumProcesses]int32 // Array of int32 for atomic operations
	// turnDekker indicates whose turn it is if both want to enter
	turnDekker int32 // int32 for atomic operations. Process 0 has initial turn if contention.
)

func processTask(
	id int, // Process ID (0 or 1 for Dekker's)
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

	myID := int32(id)
	otherID := int32(1 - id) // The ID of the other process, as int32

	addLocalTrace(StateLocalSection) // Initial state

	for step := 0; step < numOwnSteps; step++ {
		// Local Section
		delay := MinDelay + time.Duration(rand.Int63n(int64(MaxDelay-MinDelay)))
		time.Sleep(delay)

		// Entry Protocol (Dekker's Algorithm)
		addLocalTrace(StateEntryProtocol)

		atomic.StoreInt32(&flagDekker[myID], 1)

		for atomic.LoadInt32(&flagDekker[otherID]) == 1 {
			if atomic.LoadInt32(&turnDekker) == otherID { // If it's the other process's turn
				atomic.StoreInt32(&flagDekker[myID], 0) // I retract my interest temporarily
				for atomic.LoadInt32(&turnDekker) == otherID { // Wait until it's no longer their turn
					time.Sleep(1 * time.Microsecond) // Be polite, yield CPU
				}
				atomic.StoreInt32(&flagDekker[myID], 1) // Re-assert my interest
			} else {
				// It's my turn (or turn is not yet definitively other's if they haven't set it),
				// but the other is still interested. I must wait for them to finish or retract their flag.
				// This constitutes a busy wait on flagDekker[otherID] within the outer loop.
				time.Sleep(1 * time.Microsecond) // Be polite, yield CPU while waiting
			}
		}

		// Critical Section
		addLocalTrace(StateCriticalSection)
		criticalDelay := MinDelay + time.Duration(rand.Int63n(int64(MaxDelay-MinDelay)))
		time.Sleep(criticalDelay)

		// Exit Protocol (Dekker's Algorithm)
		addLocalTrace(StateExitProtocol)

		atomic.StoreInt32(&turnDekker, otherID)      // Give the turn to the other process
		atomic.StoreInt32(&flagDekker[myID], 0)      // I am no longer interested

		addLocalTrace(StateLocalSection) // Marking return to local section for next iteration
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
	fmt.Println("ALGORITHM=Dekker;")
}

func main() {
	globalStartTime = time.Now()
	rand.Seed(time.Now().UnixNano())

	// Shared variables for Dekker's algorithm are global and zero-valued:
	// flagDekker elements are int32, default to 0 (false).
	// turnDekker is int32, defaults to 0. Process 0 has initial turn if contention.

	var wg sync.WaitGroup
	batchTraceChannel := make(chan []Trace, NumProcesses)


	go printerTask(batchTraceChannel, NumProcesses)

	for i := 0; i < NumProcesses; i++ { 
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

