#include "logic.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdio.h>

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

Player winner(GridPos *grid)
{
    // Check horizontal lines
    for (int row = 0; row < size; row++)
    {
        GridPos *ptrRowArray = grid + (row * size);
        if (allElementsEqual(ptrRowArray, size))
        {
            return (*ptrRowArray).player;
        }
    }

    // Check vertical lines
    for (int col = 0; col < size; col++)
    {
        GridPos *ptrColArray = malloc(sizeof(GridPos) * size);
        for (int i = 0; i < size; i++)
        {
            ptrColArray[i] = grid[i * size + col];
        }

        if (allElementsEqual(ptrColArray, 3))
        {
            Player player = (*ptrColArray).player;
            free(ptrColArray);
            return player;
        }
    }

    // Check diagonal lines
    for (int diag = 0; diag < size; diag++)
    {
        GridPos *ptrDiagArrayLtoR = malloc(sizeof(GridPos) * size);
        GridPos *ptrDiagArrayRtoL = malloc(sizeof(GridPos) * size);
        for (int i = 0; i < size; i++)
        {
            ptrDiagArrayLtoR[i] = grid[rowColToPos(i, i)];
            ptrDiagArrayRtoL[i] = grid[rowColToPos(size - i - 1, i)];
        }

        if (allElementsEqual(ptrDiagArrayLtoR, size))
        {
            return (*ptrDiagArrayLtoR).player;
        }
        if (allElementsEqual(ptrDiagArrayRtoL, size))
        {
            return (*ptrDiagArrayRtoL).player;
        }
    }

    return empty;
}

bool canPlayAtPos(int pos, GridPos grid[])
{
    return (grid[pos]).player == empty;
}

bool playPos(Player player, int pos, GridPos grid[])
{
    if (!canPlayAtPos(pos, grid))
    {
        // error
        printf("ERROR: Cannot play at position %d", pos);
        return false;
    }

    (grid[pos]).player = player;
    return true;
}

int aiPlay(GridPos grid[])
{
    // TODO Implement better AI such as minimax (https://github.com/britannio/tic-tac-toe)
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
    return -1;
}

int rowColToPos(int row, int col)
{
    return row * 3 + col;
}