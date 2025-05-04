package main

import (
	"fmt"
	"math/rand"
	"time"
	"unicode"
)

const (
	NrOfTravelers     = 20
	NrOfWildTravelers = 10
	MinSteps          = 10
	MaxSteps          = 100
	MinDelay          = 10 * time.Millisecond
	MaxDelay          = 50 * time.Millisecond
	BoardWidth        = 15
	BoardHeight       = 15
)

var (
	StartTime = time.Now()
	printer   = Printer{}
	Board     = [BoardWidth][BoardHeight]Node{}
)

type Position struct {
	X, Y int
}

func Move_Down(pos *Position) {
	pos.Y = (pos.Y + 1) % BoardHeight
}

func Move_Up(pos *Position) {
	pos.Y = (pos.Y + BoardHeight - 1) % BoardHeight
}

func Move_Right(pos *Position) {
	pos.X = (pos.X + 1) % BoardWidth
}

func Move_Left(pos *Position) {
	pos.X = (pos.X + BoardWidth - 1) % BoardWidth
}

func Move_Direction(pos *Position, direction int) {
	switch direction {
	case 0:
		Move_Up(pos)
	case 1:
		Move_Down(pos)
	case 2:
		Move_Left(pos)
	case 3:
		Move_Right(pos)
	}
}

type Trace struct {
	TimeStamp time.Duration
	Id        int
	Position  Position
	Symbol    rune
}

func Print_Traces(traces []Trace) {
	for _, trace := range traces {
		fmt.Printf("%f %d %d %d %c\n",
			float64(trace.TimeStamp)/float64(time.Second),
			trace.Id,
			trace.Position.X,
			trace.Position.Y,
			trace.Symbol,
		)
	}
}

type Printer struct {
	TraceChannel chan []Trace
	Done         chan bool
}

func (p *Printer) Start() {
	p.TraceChannel = make(chan []Trace, NrOfTravelers)
	p.Done = make(chan bool)

	go func() {
		for i := 0; i < NrOfTravelers + NrOfWildTravelers; i++ {
			traces := <-p.TraceChannel
			Print_Traces(traces)
		}

		p.Done <- true
	}()
}

type GeneralTraveler interface {
	Init(id int, symbol rune)
	Start()
	Store_Trace()
}

type Response int

const (
	Success Response = iota
	Fail
	Deadlock
)

type Traveler struct {
	Id       int
	Symbol   rune
	Position Position

	traces    []Trace
	timeStamp time.Duration
	response  Response
}

type Legal struct {
	Traveler
	steps int
}

type RelocateRequest struct {
	Position Position
	Status   Response
}

type Wild struct {
	Traveler
	RelocateChannel chan RelocateRequest

	timeAppear    time.Duration
	timeDisappear time.Duration
}

type EnterRequest struct {
	Traveler        GeneralTraveler
	ResponseChannel chan Response
}
type Node struct {
	EnterChannel chan EnterRequest
	LeaveChannel chan bool

	position Position
	traveler GeneralTraveler
	waiting  []EnterRequest
}

func (n *Node) Init(position Position) {
	n.EnterChannel = make(chan EnterRequest)
	n.LeaveChannel = make(chan bool)

	n.position = position
	n.traveler = nil

	n.Start()
}

func (n *Node) Start() {
	go func() {
		// main loop
		// wait for enter or leave or relocate
		for true {
			select {
				case Request := <-n.EnterChannel:
					// enter
					if n.traveler == nil {
						// if empty - assign traveler
						n.traveler = Request.Traveler
						Request.ResponseChannel <- Success
					} else if _, ok := n.traveler.(*Legal); ok {
						// if legal - no one can enter
						Request.ResponseChannel <- Fail
					} else if wild, ok := n.traveler.(*Wild); ok {
						// if wild - try to move him
						if _, ok := Request.Traveler.(*Legal); ok {
							var newPosition Position
							var nodeResponse Response
							directions := []int{0, 1, 2, 3}
							for _, dir := range directions {
								newPosition = n.position
								Move_Direction(&newPosition, dir)

								request := EnterRequest{n.traveler, make(chan Response)}
								Board[newPosition.X][newPosition.Y].EnterChannel <- request
								nodeResponse = <-request.ResponseChannel
								if nodeResponse != Fail {
									break
								}
							}
							if nodeResponse != Fail {
								wild.RelocateChannel <- RelocateRequest{newPosition, Success}
								n.traveler = Request.Traveler
								Request.ResponseChannel <- Success
							} else {
								Request.ResponseChannel <- Fail
							}
						  // else if wild tries to enter -> do not allow
						} else { 
							Request.ResponseChannel <- Fail
						}
					  // any other case	
					} else {
						Request.ResponseChannel <- Fail
					}
				case <-n.LeaveChannel:
					n.traveler = nil
			}
		}
	}()
}

func (t *Traveler) Store_Trace() {
	t.traces = append(t.traces, Trace{
		TimeStamp: t.timeStamp,
		Id:        t.Id,
		Position:  t.Position,
		Symbol:    t.Symbol,
	})
}

func (t *Legal) Init(id int, symbol rune) {
	t.Id = id
	t.Symbol = symbol
	t.steps = MinSteps + rand.Intn(MaxSteps-MinSteps+1)

	t.response = Fail
	// try to move in
	for t.response == Fail {
		t.Position = Position{
			X: rand.Intn(BoardWidth),
			Y: rand.Intn(BoardHeight),
		}

		request := EnterRequest{t, make(chan Response, 1)}
		Board[t.Position.X][t.Position.Y].EnterChannel <- request
		t.response = <-request.ResponseChannel
	}

	t.timeStamp = time.Since(StartTime)
	t.Store_Trace()
}

func (t *Legal) Start() {
	go func() {
		for i := 0; i < t.steps; i++ {
			if t.response == Deadlock {
				break
			}

			time.Sleep(MinDelay + time.Duration(rand.Int63n(int64(MaxDelay-MinDelay))))

			// create channels for success and deadlock
			// successChannel is buffered to avoid deadlock
			// deadlockChannel is buffered to avoid deadlock (yes I know how it sounds)
			successChannel := make(chan bool, 1)
			deadlockChannel := make(chan bool, 1)

			var newPosition Position

			// runs in a separate goroutine
			// tries to enter a new position
			// if success sends to successChannel
			// if tiemeout is reached reads from deadlockChannel and exits (by setting t.response to Deadlock)
			go func() {
				t.response = Fail

				// go's while's are weird
				for t.response == Fail {

					newPosition = t.Position
					Move_Direction(&newPosition, rand.Intn(4))

					request := EnterRequest{t, make(chan Response, 1)}
					Board[newPosition.X][newPosition.Y].EnterChannel <- request

					select {
						case t.response = <-request.ResponseChannel:
							if t.response != Fail {
								successChannel <- true
							} else {
								time.Sleep(time.Millisecond)
							}

						case <-deadlockChannel:
							t.response = Deadlock
					}
				}
			}()

			// wait for success or deadlock
			// if success - leave the old position
			// if deadlock - I'm sorry
			select {
				case <-successChannel:
				case <-time.After(6 * MaxDelay):
					deadlockChannel <- true
			}

			// handle response
			switch t.response {
				case Success:
					Board[t.Position.X][t.Position.Y].LeaveChannel <- true
					t.Position = newPosition

				case Deadlock:
					t.Symbol = unicode.ToLower(t.Symbol)
			}

			// store trace
			t.timeStamp = time.Since(StartTime)
			t.Store_Trace()
		}

		printer.TraceChannel <- t.traces
	}()
}

func (t *Wild) Init(id int, symbol rune) {
	t.RelocateChannel = make(chan RelocateRequest)
	t.Id = id
	t.Symbol = symbol
	t.timeAppear = time.Duration(rand.Int63n(int64(MaxDelay * MaxSteps)))
	t.timeDisappear = t.timeAppear + time.Duration(rand.Int63n(int64(MaxDelay*MaxSteps-t.timeAppear)))
}

func (t *Wild) Start() {
	go func() {
		time.Sleep(t.timeAppear)

		// try to move in
		t.response = Fail
		for t.response == Fail {
			t.Position = Position{
				X: rand.Intn(BoardWidth),
				Y: rand.Intn(BoardHeight),
			}

			request := EnterRequest{t, make(chan Response)}
			Board[t.Position.X][t.Position.Y].EnterChannel <- request
			t.response = <-request.ResponseChannel
		}

		t.timeStamp = time.Since(StartTime)
		t.Store_Trace()

		// main loop
		t.RelocateChannel = make(chan RelocateRequest)
		for true {
			if time.Since(StartTime) > t.timeDisappear {
				break
			}

			select {
				case Request := <-t.RelocateChannel:
					t.response = Request.Status
					t.Position = Request.Position
					t.timeStamp = time.Since(StartTime)
					t.Store_Trace()

				case <-time.After(t.timeDisappear - time.Since(StartTime)):
			}
		}

		// free the board
		Board[t.Position.X][t.Position.Y].LeaveChannel <- true
		t.Position = Position{
			X: BoardWidth,
			Y: BoardHeight,
		}
		t.timeStamp = time.Since(StartTime)
		t.Store_Trace()
		

		printer.TraceChannel <- t.traces
	}()
}

func main() {
	var travelers [NrOfTravelers + NrOfWildTravelers]GeneralTraveler

	fmt.Printf(
		"-1 %d %d %d\n",
		NrOfTravelers+NrOfWildTravelers,
		BoardWidth,
		BoardHeight,
	)

	printer.Start()

	for i := 0; i < BoardWidth; i++ {
		for j := 0; j < BoardHeight; j++ {
			Board[i][j].Init(Position{i, j})
		}
	}

	id := 0
	symbol := 'A'
	for i := 0; i < NrOfTravelers; i++ {
		travelers[id] = &Legal{}
		travelers[id].Init(id, symbol)
		id++
		symbol++
	}

	symbol = '0'
	for i := 0; i < NrOfWildTravelers; i++ {
		travelers[id] = &Wild{}
		travelers[id].Init(id, symbol)
		id++
		symbol++
	}

	id = 0
	for i := 0; i < NrOfTravelers; i++ {
		travelers[id].Start()
		id++
	}

	for i := 0; i < NrOfWildTravelers; i++ {
		travelers[id].Start()
		id++
	}

	<-printer.Done
}