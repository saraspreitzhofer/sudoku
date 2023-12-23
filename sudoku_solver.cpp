#include <iostream>

const int N = 9;

using namespace std;

bool isPresentInCol(int col, int num, int **grid) { //check whether num is present in col or not
    for (int row = 0; row < N; row++)
        if (grid[row][col] == num)
            return true;
    return false;
}

bool isPresentInRow(int row, int num, int **grid) { //check whether num is present in row or not
    for (int col = 0; col < N; col++)
        if (grid[row][col] == num)
            return true;
    return false;
}

bool isPresentInBox(int boxStartRow, int boxStartCol, int num, int **grid) { //check whether num is present in 3x3 box
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            if (grid[row + boxStartRow][col + boxStartCol] == num)
                return true;
    return false;
}

int isNumberRepeated(int row, int col, int num, int **sudoku) {
    int count = 0;
    int repetitions = 0;
    // Check row
    for (int i = 0; i < N; ++i) {
        if (sudoku[row][i] == num) {
            count++;
            if (count > 1) {
                repetitions++;
                break;
            }
        }
    }
    // Check column
    count = 0;
    for (int i = 0; i < N; ++i) {
        if (sudoku[i][col] == num) {
            count++;
            if (count > 1) {
                repetitions++;
                break;
            }
        }
    }
    // Check box
    count = 0;
    bool stop = false;
    int boxStartRow = row - row % 3;
    int boxStartCol = col - col % 3;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (sudoku[boxStartRow + i][boxStartCol + j] == num) {
                count++;
                if (count > 1) {
                    repetitions++;
                    stop = true;
                    break;
                }
            }
        }
        if (stop) break;
    }
    return repetitions;
}

void sudokuGrid(int **grid) { //print the sudoku grid after solve
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            if (col == 3 || col == 6)
                cout << " | ";
            cout << grid[row][col] << " ";
        }
        if (row == 2 || row == 5) {
            cout << endl;
            for (int i = 0; i < N; i++)
                cout << "---";
        }
        cout << endl;
    }
}

bool findEmptyPlace(int &row, int &col, int **grid) { //get empty location and update row and column
    for (row = 0; row < N; row++)
        for (col = 0; col < N; col++)
            if (grid[row][col] == 0) //marked with 0 is empty
                return true;
    return false;
}

bool isValidPlace(int row, int col, int num, int **grid) {
    //when item not found in col, row and current 3x3 box
    return !isPresentInRow(row, num, grid) && !isPresentInCol(col, num, grid) && !isPresentInBox(row - row % 3,
                                                                                                 col - col % 3, num,
                                                                                                 grid);
}

int countZeros(int **grid) {
    int count = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (grid[i][j] == 0) count++;
        }
    }
    return count;
}

bool solveSudokuRecursion(int **grid, int &solutionCount) {
    int row, col;
    if (!findEmptyPlace(row, col, grid)) {
        solutionCount++;
        return solutionCount == 1; // Return true if there is only one solution
    }
    for (int num = 1; num <= 9; num++) { //valid numbers are 1 - 9
        if (isValidPlace(row, col, num, grid)) { //check validation, if yes, put the number in the grid
            grid[row][col] = num;
            if (!solveSudokuRecursion(grid, solutionCount)) // Return false if more than one solution found
                return false;
            grid[row][col] = 0; //turn to unassigned space
        }
    }
    return true;
}

bool solveSudoku(int **grid) {
    int solutionCount = 0;
    return solveSudokuRecursion(grid, solutionCount) && solutionCount == 1;
}


bool checkSudoku(int **grid) {
    int repetitions = 0;
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            int num = grid[row][col];
            if (num == 0) return false;
            repetitions += isNumberRepeated(row, col, num, grid);
        }
    }
    return repetitions == 0;
}

bool isSolvable(int **grid) {
    if (!solveSudoku(grid)) {
        cout << "No solution exists" << endl;
        return false;
    } else if (countZeros(grid) == 0 && !checkSudoku(grid)) {
        cout << "Incorrect solution" << endl;
        return false;
    } else {
        cout << "Valid solution" << endl << endl;
        return true;
    }
}