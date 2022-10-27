SOURCES := $(wildcard sources/*.cpp)

all: sudoku

sudoku: sources/main.cpp
	g++ -o sudoku $(SOURCES) -lncurses

clean:
	rm sudoku
	