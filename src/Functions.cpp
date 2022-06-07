#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>
#include "Functions.h"

using namespace cv;
using namespace std;


/* 
 * Searches the grid to find an entry that is still unassigned. 
 */
bool SudokuSolver::findUnassignedLocation(int grid[N][N], int &row, int &col)
{
    for (row = 0; row < N; row++)
    {
        for (col = 0; col < N; col++)
        {
            if (grid[row][col] == UNASSIGNED)
            {
                return true;
            }
        }
    }
    return false;
}

/* 
 * Returns whether any assigned entry n the specified row matches
 * the given number. 
 */
bool SudokuSolver::usedInRow(int grid[N][N], int row, int num)
{
    for (int col = 0; col < N; col++)
    {
        if (grid[row][col] == num)
        {
            return true;
        }
    }
    return false;
}

/* 
 * Returns whether any assigned entry in the specified column matches
 * the given number. 
 */
bool SudokuSolver::usedInCol(int grid[N][N], int col, int num)
{
    for (int row = 0; row < N; row++)
    {
        if (grid[row][col] == num)
        {
            return true;
        }
    }
    return false;
}

/* 
 * Returns whether any assigned entry within the specified 3x3 box matches
 * the given number. 
 */
bool SudokuSolver::usedInBox(int grid[N][N], int boxStartRow, int boxStartCol, int num)
{
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            if (grid[row + boxStartRow][col + boxStartCol] == num)
            {
                return true;
            }
        }
    }
    return false;
}

/* 
 * Returns whether it will be legal to assign num to the given row,col location.
 */
bool SudokuSolver::isSafe(int grid[N][N], int row, int col, int num)
{
    return !usedInRow(grid, row, num) && !usedInCol(grid, col, num) &&
        !usedInBox(grid, row - row % 3, col - col % 3, num);
}

/* 
 * assign values to all unassigned locations for Sudoku solution
 */
bool SudokuSolver::solve(int grid[N][N])
{
    // sanity check
    {
        char valueExists[9];

        // check rows
        for (int i = 0; i < 9; i++) 
        {
            memset(valueExists, 0, 9 * sizeof(char));
            for (int j = 0; j < 9; j++) 
            {
                int value = grid[i][j];
                if (!value) 
                {
                    continue;
                }
                char* exist = &valueExists[value - 1];
                if (*exist) 
                {
                    return false;
                }
                *exist = 1;
            }
        }
        // check cols
        for (int j = 0; j < 9; j++) 
        {
            memset(valueExists, 0, 9 * sizeof(char));
            for (int i = 0; i < 9; i++) 
            {
                int value = grid[i][j];
                if (!value) 
                {
                    continue;
                }
                char* exist = &valueExists[value - 1];
                if (*exist) 
                {
                    return false;
                }
                *exist = 1;
            }
        }
        // check blocks
        for (int a = 0; a < 3; a++) 
        {
            for (int b = 0; b < 3; b++) 
            {
                memset(valueExists, 0, 9 * sizeof(char));
                for (int i = 3 * a; i < 3 * (a + 1); i++) 
                {
                    for (int j = 3 * b; j < 3 * (b + 1); j++) 
                    {
                        int value = grid[i][j];
                        if (!value) 
                        {
                            continue;
                        }
                        char* exist = &valueExists[value - 1];
                        if (*exist) 
                        {
                            return false;
                        }
                        *exist = 1;
                    }
                }
            }
        }
    }

    int row, col;
    if (!findUnassignedLocation(grid, row, col))
    {
        return true;
    }
    for (int num = 1; num <= 9; num++)
    {
        if (isSafe(grid, row, col, num))
        {
            grid[row][col] = num;
            if (solve(grid))
            {
                return true;
            }
            grid[row][col] = UNASSIGNED;
        }
    }
    return false;
}
