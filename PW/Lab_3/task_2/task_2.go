package main

import (
	"fmt"
	"math/rand"
	"sync"
	"sync/atomic" // Added for atomic operations
	"time"
)

// Configuration Constants
const (
	NumProcessesBakery = 20 
	MinStepsBakery     = 5
	MaxStepsBakery     = 20

	MinDelayBakery = 10 * time.Millisecond
	MaxDelayBakery = 50 * time.Millisecond
)

// Process States (same as Peterson's version)
type ProcessState int

const (
	StateLocalSection ProcessState = iota
	StateEntryProtocol
	StateCriticalSection
	StateExitProtocol
)

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

// Trace struct (same as Peterson's version)
type Trace struct {
	Timestamp    time.Duration
	ProcessID    int
	ProcessState ProcessState
	Symbol       rune
}

var globalStartTimeBakery time.Time

// --- Bakery Algorithm Shared Variables (using sync/atomic functions) ---
var (
	// choosingBakery[i] is 1 if process i is picking a number, 0 otherwise
	choosingBakery []int32
	// numberBakery[i] is the ticket number for process i
	numberBakery []int32
)

// processTaskBakery simulates a single concurrent process using Bakery algorithm
func processTaskBakery(
	id int,
	symbol rune,
	numOwnSteps int,
	batchTraceChannel chan<- []Trace,
	wg *sync.WaitGroup,
	numTotalProcesses int, // Pass NumProcessesBakery here
) {
	defer wg.Done()

	var localTraces []Trace

	addLocalTrace := func(state ProcessState) {
		localTraces = append(localTraces, Trace{
			Timestamp:    time.Since(globalStartTimeBakery),
			ProcessID:    id,
			ProcessState: state,
			Symbol:       symbol,
		})
	}

	addLocalTrace(StateLocalSection) // Initial state

	for step := 0; step < numOwnSteps; step++ {
		//Local Section
		delay := MinDelayBakery + time.Duration(rand.Int63n(int64(MaxDelayBakery-MinDelayBakery)))
		time.Sleep(delay)

		//Entry Protocol
		addLocalTrace(StateEntryProtocol)

		atomic.StoreInt32(&choosingBakery[id], 1) // choosingBakery[id] = true (1)

		maxTicket := int32(0)
		for i := 0; i < numTotalProcesses; i++ {
			num_i := atomic.LoadInt32(&numberBakery[i])
			if num_i > maxTicket {
				maxTicket = num_i
			}
		}
		atomic.StoreInt32(&numberBakery[id], maxTicket+1)

		atomic.StoreInt32(&choosingBakery[id], 0) // choosingBakery[id] = false (0)

		for j := 0; j < numTotalProcesses; j++ {
			if j == id {
				continue // Skip self
			}

			// Wait if process j is currently choosing its number
			for atomic.LoadInt32(&choosingBakery[j]) == 1 {
				time.Sleep(1 * time.Microsecond) // Busy wait, yield
			}

			for {
				num_j := atomic.LoadInt32(&numberBakery[j])
				if num_j == 0 { // Process j is not interested or has finished CS
					break
				}
				my_num := atomic.LoadInt32(&numberBakery[id])

				if (num_j < my_num) || (num_j == my_num && j < id) {
					time.Sleep(1 * time.Microsecond) // Busy wait
				} else {
					break // Condition not met, proceed
				}
			}
		}

		// Critical Section
		addLocalTrace(StateCriticalSection)
		criticalDelay := MinDelayBakery + time.Duration(rand.Int63n(int64(MaxDelayBakery-MinDelayBakery)))
		time.Sleep(criticalDelay)

		// Exit Protocol (Bakery Algorithm)
		addLocalTrace(StateExitProtocol)

		atomic.StoreInt32(&numberBakery[id], 0)

		addLocalTrace(StateLocalSection)
	}

	batchTraceChannel <- localTraces
}

// printerTaskBakery (same printing logic as Peterson's version)
func printerTaskBakery(batchTraceChannel <-chan []Trace, numProcesses int) {

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
	fmt.Println("ALGORITHM=Bakery;")
}

func main() {
	globalStartTimeBakery = time.Now()
	rand.Seed(time.Now().UnixNano())

	// Initialize Bakery shared variables
	choosingBakery = make([]int32, NumProcessesBakery) // Defaults to 0 (false)
	numberBakery = make([]int32, NumProcessesBakery)   // Defaults to 0

	var wgBakery sync.WaitGroup
	batchTraceChannelBakery := make(chan []Trace, NumProcessesBakery)


	go printerTaskBakery(batchTraceChannelBakery, NumProcessesBakery)

	for i := 0; i < NumProcessesBakery; i++ {
		wgBakery.Add(1)
		symbol := rune('A' + i)
		numOwnSteps := MinStepsBakery + rand.Intn(MaxStepsBakery-MinStepsBakery+1)

		go processTaskBakery(
			i,
			symbol,
			numOwnSteps,
			batchTraceChannelBakery,
			&wgBakery,
			NumProcessesBakery,
		)
	}

	wgBakery.Wait()
	close(batchTraceChannelBakery)

	time.Sleep(200 * time.Millisecond)
}

