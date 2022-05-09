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
} GridPos;

Player isWinner(GridPos grid[]);
int aiPlay(GridPos grid[]);
bool canPlayAtPos(int pos, GridPos grid[]);
void playPos(Player player, int pos, GridPos grid[]);
int nextFreePos(GridPos grid[]);
