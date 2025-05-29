C_SOURCES   = $(wildcard *.c)
CPP_SOURCES = $(wildcard *.cpp)
C_BINARIES   = $(patsubst %.c, %, $(C_SOURCES))
CPP_BINARIES = $(patsubst %.cpp, %, $(CPP_SOURCES))
BINARIES = $(C_BINARIES) $(CPP_BINARIES)

C_FLAGS   = -W -pedantic -std=c2x -O3
CPP_FLAGS = -Wall -pedantic -std=c++2a -O3
LIBS = -lgsl

.PHONY: all clean

all: $(BINARIES)

$(C_BINARIES): %: %.c
	$(CC) $(C_FLAGS) $^ -o $@ $(LIBS)
	strip $@

$(CPP_BINARIES): %: %.cpp
	$(CXX) $(CPP_FLAGS) $^ -o $@ $(LIBS)
	strip $@

clean:
	rm -f $(BINARIES)
