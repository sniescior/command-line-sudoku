#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector>

class Sudoku {
private:
    int sudokuArr[9][9];
    std::vector <int> numbers;
public:
    Sudoku();
    ~Sudoku();
    Sudoku(int grid[9][9]);
    void createSeed();
    void printSudoku();
    bool isSafe(int row, int col, int number);
    bool solveSudoku(int row, int col);
};

#endif