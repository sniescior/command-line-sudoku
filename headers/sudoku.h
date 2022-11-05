#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector>

class Sudoku {
private:
    int sudokuArr[9][9];
    std::vector <int> numbers;
public:
    int getItem(int x, int y);
    Sudoku();
    ~Sudoku();
    Sudoku(int grid[9][9]);
    Sudoku* copySudoku();
    void createSeed();
    void printSudoku();
    bool isSafe(int row, int col, int number);
    bool solveSudoku(int row, int col);
    void generateSudoku(int level);
    int countBlank();
    void setItem(int x, int y, int number);
};

#endif