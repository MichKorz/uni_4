package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	. "time"
)

const EthernetLength = 30
const Interval = Millisecond * 300
const MaxWait = 50

type readOp struct {
	key  int
	resp chan string
}
type writeOp struct {
	key  int
	val  string
	sign string
	resp chan bool
}

var stations []string
var connections []string

func main() {
	reads := make(chan *readOp)
	writes := make(chan *writeOp)

	stations = fillSlice(stations, " ")
	connections = fillSlice(connections, " ")

	go station("A", 10, reads, writes)
	Sleep(Nanosecond)
	go station("B", 2, reads, writes)
	Sleep(Nanosecond)
	//go station("C", 25, reads, writes)
	//Sleep(Nanosecond)
	//go station("D", 14, reads, writes)

	ethernet(reads, writes)
}

func ethernet(reads chan *readOp, writes chan *writeOp) {
	var state []string
	state = fillSlice(state, "0")
	printConnections()
	printEthernet(state)
	for {
		select {
		case write := <-writes:
			isRight := true
			switch write.val {
			case "0":
				if state[write.key] == write.sign || state[write.key] == "=" {
					state[write.key] = write.val
				}
			default:
				if state[write.key] == "0" || state[write.key] == write.val {
					state[write.key] = write.val
				} else {
					state[write.key] = "="
					isRight = false
				}
			}
			printEthernet(state)
			write.resp <- isRight

		case read := <-reads:
			read.resp <- state[read.key]
		}
	}
}

func station(signal string, place int, reads chan *readOp, writes chan *writeOp) {
	stations[place] = signal
	connections[place] = "|"
	wait := getRandomTime()
	cu := make(chan bool, 1)
	cd := make(chan bool, 1)
	counter := 0
	Sleep(wait)
	for {
		if isFree(place, reads) {
			sendSignal(place, signal, signal, writes)
			Sleep(Interval)
			go propagateUp(place, signal, signal, writes, cu)
			go propagateDown(place, signal, signal, writes, cd)
			Sleep(Interval)

			c1 := <-cu
			c2 := <-cd

			sendSignal(place, "0", signal, writes)
			Sleep(Interval)

			go propagateUp(place, "0", signal, writes, nil)
			go propagateDown(place, "0", signal, writes, nil)

			if !c1 || !c2 {
				counter++
				wait = getTimeToWait(counter)
			} else {
				counter = 0
				wait = getRandomTime()
			}
			Sleep(wait)
		} else {
			Sleep(Interval)
		}
	}
}

func getTimeToWait(counter int) Duration {
	source := rand.NewSource(Now().UnixNano())
	generator := rand.New(source)
	var MaxWait = 0
	switch {
	case counter < 10:
		MaxWait = int(math.Pow(2, float64(counter)))
	case counter < 17:
		MaxWait = int(math.Pow(2, 10))
	default:
		{
			println("Ethernet is not working correctly")
			os.Exit(1)
		}
	}
	r := Duration(generator.Intn(MaxWait))
	return r * Interval
}

func getRandomTime() Duration {
	source := rand.NewSource(Now().UnixNano())
	generator := rand.New(source)
	r := Duration(generator.Intn(MaxWait))
	return r * Interval
}

func isFree(key int, reads chan *readOp) bool {
	read := &readOp{
		key:  key,
		resp: make(chan string)}
	reads <- read
	return <-read.resp == "0"
}

func sendSignal(key int, val string, sign string, writes chan *writeOp) bool {
	write := &writeOp{
		key:  key,
		val:  val,
		sign: sign,
		resp: make(chan bool)}
	writes <- write
	return <-write.resp
}

func propagateUp(src int, signal string, sign string, writes chan *writeOp, cu chan bool) {
	isRight := true
	i := 0
	j := src
	for i < 2*EthernetLength {
		for j+1 < EthernetLength {
			check := true
			check = sendSignal(j+1, signal, sign, writes)
			if !check {
				isRight = false
			}
			j++
			Sleep(Interval)
		}
		i++
	}
	cu <- isRight
}

func propagateDown(src int, signal string, sign string, writes chan *writeOp, cd chan bool) {
	isRight := true
	i := 0
	j := src
	for i < 2*EthernetLength {
		for j-1 >= 0 {
			check := true
			check = sendSignal(j-1, signal, sign, writes)
			if !check {
				isRight = false
			}
			j--
			Sleep(Interval)
		}
		i++
	}
	cd <- isRight
}

func fillSlice(slice []string, s string) []string {
	for i := 0; i < EthernetLength; i++ {
		slice = append(slice, s)
	}
	return slice
}

func printConnections() {
	fmt.Println(stations)
	fmt.Println(connections)
}

func printEthernet(slice []string) {
	fmt.Print("\r", slice)
}
