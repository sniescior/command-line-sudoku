all: sudoku

sudoku: main.cpp
	g++ -o sudoku main.cpp -lncurses

clean:
	rm sudoku
	