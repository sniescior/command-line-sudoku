#include "../headers/sudoku.h"
#include <iostream>
#include <algorithm>
#include <ctime>

Sudoku::Sudoku() {
    for(int i = 1; i < 10; i++) { this->numbers.push_back(i); }
    this->createSeed();
}

Sudoku::Sudoku(int grid[9][9]) {
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            this->sudokuArr[i][j] = grid[i][j];
        }
    }
}

void Sudoku::printSudoku() {
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            std::cout << this->sudokuArr[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int Sudoku::getItem(int x, int y) {
    return this->sudokuArr[x][y];
}

/**
 * 
 * --------------------- SUDOKU SOLVING FUNCTIONS ---------------------
 * 
*/

// Checks if it's legal to assign number to the given row and column
bool Sudoku::isSafe(int row, int col, int number) {
    // If we find the same number in row, we return false (not safe)
    for (int i = 0; i < 9; i++) {
        if(this->sudokuArr[row][i] == number) return false;
    }

    // Same as the above but for columns
    for (int i = 0; i < 9; i++) {
        if(this->sudokuArr[i][col] == number) return false;
    }

    int startRow = row - row % 3;
    int startCol = col - col % 3;

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(this->sudokuArr[i + startRow][j + startCol] == number) return false;
        }
    }

    return true;
}

bool Sudoku::solveSudoku(int row, int col) {
    if(row == 8 && col == 9) return true;

    if(col == 9) {
        row++;
        col = 0;
    }

    if(this->sudokuArr[row][col] > 0) return solveSudoku(row, col + 1);

    for(int i = 1; i <= 9; i++) {
        if(isSafe(row, col, i)) {
            this->sudokuArr[row][col] = i;
            
            if(solveSudoku(row, col + 1)) {
                return true;
            }
        }

        this->sudokuArr[row][col] = 0;
    }

    return false;
}

void Sudoku::setItem(int x, int y, int number) {
    this->sudokuArr[x][y] = number;
}

/**
 * 
 * --------------------- SUDOKU GENERATOR FUNCTIONS ---------------------
 * 
*/

int getRandom(int limit) {
    return rand()%limit;
}

void Sudoku::createSeed() {
    // Filling random fields with random numbers

    srand((unsigned int) time (NULL));
    for(int i = 0; i < 3; i++) {
        int randomRow = rand()%9;
        // srand((unsigned int) time (NULL));
        int randomCol = rand()%9;
        // srand((unsigned int) time (NULL));
        int randomNumber = rand()%9 + 1;
        this->sudokuArr[randomRow][randomCol] = randomNumber;
    }
}

int Sudoku::countBlank() {
    int blankSum = 0;
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(this->sudokuArr[i][j] == 0) {
                blankSum += 1;
            }
        }
    }

    return blankSum;
}

void Sudoku::generateSudoku(int level) {
    // level Easy (0) -> 40 blankFields
    // level Medium (1) -> 50 blankFields
    // level Hard (2) -> 60 blankFields

    // In reality those numbers are approximate values representing blank fields in the puzzle
    // depending on choosen level

    int blankFieldsCount = 0;
    switch (level) {
        case 0:
            blankFieldsCount = 40;
            break;
        case 1:
            blankFieldsCount = 53;
            break;
        case 2:
            blankFieldsCount = 63;
            break;
    
        default:
            break;
    }

    srand((unsigned int) time (NULL));
    for(int i = 0; i < blankFieldsCount + 10; i++) {
        int randomRow = rand()%9;
        int randomCol = rand()%9;
        this->sudokuArr[randomRow][randomCol] = 0;

        if(this->countBlank() > blankFieldsCount) {
            break;
        }
    }
}

Sudoku* Sudoku::copySudoku() {
    return new Sudoku(this->sudokuArr);
}


