#pragma once

/************************** DEFINES **************************************/

#define UNASSIGNED 0
#define N 9

/****************** SUDOKU SOLVER CLASS *******************************/

class SudokuSolver
{
public:
    virtual ~SudokuSolver() {}

    bool solve(int grid[N][N]);

private:
    bool findUnassignedLocation(int grid[N][N], int &row, int &col);
    bool usedInRow(int grid[N][N], int row, int num);
    bool usedInCol(int grid[N][N], int col, int num);
    bool usedInBox(int grid[N][N], int boxStartRow, int boxStartCol, int num);
    bool isSafe(int grid[N][N], int row, int col, int num);
};
