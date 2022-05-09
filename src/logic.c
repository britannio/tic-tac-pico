#include "logic.h"
#include "pico/stdlib.h"
#include <stdlib.h>

const int size = 3;

bool allElementsEqual(GridPos grid[], int size)
{
    for (int i = 0; i < size; i++)
    {
        if (grid[i].player != grid[0].player)
        {
            return false;
        }
    }
    return true;
}

Player isWinner(GridPos *grid)
{
    // Check horizontal lines
    for (int row = 0; row < size; row++)
    {
        GridPos *rowArray = grid + (row * size);
        if (allElementsEqual(rowArray, size))
        {
            return (*rowArray).player;
        }
    }

    // Check vertical lines
    for (int col = 0; col < size; col++)
    {
        GridPos *colArray = malloc(sizeof(GridPos) * size);
        for (int i = 0; i < size; i++)
        {
            colArray[i] = grid[i * size + col];
        }

        if (allElementsEqual(colArray, 3))
        {
            Player player = (*colArray).player;
            free(colArray);
            return player;
        }
    }

    // Check diagonal lines
    for (int diag = 0; diag < size; diag++)
    {
        GridPos *diagArrayLtoR = malloc(sizeof(GridPos) * size);
        GridPos *diagArrayRtoL = malloc(sizeof(GridPos) * size);
        for (int i = 0; i < size; i++)
        {
            diagArrayLtoR[i] = grid[rowColToPos(i, i)];
            diagArrayRtoL[i] = grid[rowColToPos(size - i - 1, i)];
        }

        if (allElementsEqual(diagArrayLtoR, size))
        {
            return (*diagArrayLtoR).player;
        }
        if (allElementsEqual(diagArrayRtoL, size))
        {
            return (*diagArrayRtoL).player;
        }
    }

    return empty;
}

bool canPlayAtPos(int pos, GridPos grid[])
{
    return (grid[pos]).player == empty;
}

void playPos(Player player, int pos, GridPos grid[])
{
    if (!canPlayAtPos(pos, grid))
    {
        // error
        printf("ERROR: Cannot play at position %d", pos);
        return;
    }

    (grid[pos]).player = player;
}

int aiPlay(GridPos grid[])
{
    // TODO Implement better AI
    return nextFreePos(grid);
}

int nextFreePos(GridPos grid[])
{
    for (int i = 0; i < size * size; i++)
    {
        GridPos pos = grid[i];
        if (pos.player == empty)
        {
            return i;
        }
    }
}

int rowColToPos(int row, int col)
{
    return row * 3 + col;
}