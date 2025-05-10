all:
	gnatmake travelers.adb
run:
	./travelers > out
	bash ../display-travel.bash out
clean:
	rm *.o
	rm *.ali
	rm ./travelers
	rm ./out