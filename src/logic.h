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

Player isWinner(GridPos grid[]);
int aiPlay(GridPos grid[]);
bool canPlayAtPos(int pos, GridPos grid[]);
void playPos(Player player, int pos, GridPos grid[]);
int nextFreePos(GridPos grid[]);
