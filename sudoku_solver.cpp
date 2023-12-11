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

bool
isPresentInBox(int boxStartRow, int boxStartCol, int num, int **grid) { //check whether num is present in 3x3 box or not
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            if (grid[row + boxStartRow][col + boxStartCol] == num)
                return true;
    return false;
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

bool solveSudoku(int **grid) {
    int row, col;
    if (!findEmptyPlace(row, col, grid))
        return true; //when all places are filled
    for (int num = 1; num <= 9; num++) { //valid numbers are 1 - 9
        if (isValidPlace(row, col, num, grid)) { //check validation, if yes, put the number in the grid
            grid[row][col] = num;
            if (solveSudoku(grid)) //recursively go for other rooms in the grid
                return true;
            grid[row][col] = 0; //turn to unassigned space when conditions are not satisfied
        }
    }
    return false;
}

bool checkSudoku(int **grid) {
    // Check validity for each row
    for (int row = 0; row < N; row++) {
        for (int num = 1; num <= N; num++) {
            if (isPresentInRow(row, num, grid))
                return false;
        }
    }

    // Check validity for each column
    for (int col = 0; col < N; col++) {
        for (int num = 1; num <= N; num++) {
            if (isPresentInCol(col, num, grid))
                return false;
        }
    }

    // Check validity for each box
    for (int startRow = 0; startRow < N; startRow += 3) {
        for (int startCol = 0; startCol < N; startCol += 3) {
            for (int num = 1; num <= N; num++) {
                if (isPresentInBox(startRow, startCol, num, grid))
                    return false;
            }
        }
    }

    return true; // The Sudoku grid is valid
}

int printSolution(int **grid) {
    if (!solveSudoku(grid)) cout << "No solution exists";
    else if (!checkSudoku(grid)) cout << "Incorrect solution";
    else sudokuGrid(grid);
}
