#include "pico/stdlib.h"

typedef enum
{
  empty,
  human,
  ai
} Player;

typedef struct
{
  Player player;
  // Once the game is over, the winning positions can be highlighted
  // (change colour)
  bool winningPos;
} GridPos;

Player isWinner(volatile GridPos grid[]);
int aiPlay(volatile GridPos grid[]);
bool canPlayAtPos(int pos, volatile GridPos grid[]);
bool playPos(Player player, int pos, volatile GridPos grid[]);
int nextFreePos(volatile GridPos grid[]);
int rowColToPos(int row, int col);
bool allElementsEqual(volatile GridPos grid[], int size);
