#include <stdio.h>

#include "pico/stdlib.h"
#include "lib/fonts.h"
#include "lib/st7735.h"
#include "painting.h"

// Game constants
const int gridSize = 3;

// Display dimensions
const u_int16_t width = 80;
const u_int16_t height = 160;

enum Player
{
  empty,
  ai,
  human
};

typedef struct
{
  enum Player player;
} GridPos;

int main()
{
  // Screen and other boilerplate initialisation
  stdio_init_all();                 // Initialise serial in/output
  setvbuf(stdout, NULL, _IONBF, 0); // Disable line and block buffering on stdout (for talking through serial)
  sleep_ms(1000);                   // Give the Pico some time to think...
  ST7735_Init();                    // Initialise the screen

  clearScreen();

  GridPos grid[] = {
      (GridPos){.player = human},
      (GridPos){.player = ai},
      (GridPos){.player = empty},
      (GridPos){.player = empty},
      (GridPos){.player = human},
      (GridPos){.player = ai},
      (GridPos){.player = empty},
      (GridPos){.player = empty},
      (GridPos){.player = human},

  };
  paintGrid(gridSize, grid);
}

void clearScreen()
{
  ST7735_FillScreen(ST7735_BLACK);
}

void paintGrid(int size, GridPos grid[])
{
  uint16_t oneThird = 27;
  uint16_t twoThirds = 53;

  // Paint the lines forming the grid
  paintVerticalLine(oneThird, 0, width);
  paintVerticalLine(twoThirds, 0, width);
  paintHorizontalLine(oneThird, 0, width);
  paintHorizontalLine(twoThirds, 0, width);

  // Paint the players
  for (int i = 0; i < size * size; i++)
  {
    printf("Painting player %d at position %d\n", grid[i].player, i);
    switch (grid[i].player)
    {
    case ai:
      paintAI(i);
      break;
    case human:
      paintHuman(i);
      break;
    case empty:
      break;
    }
  }
}

void paintHuman(uint8_t pos)
{
  const uint16_t oneThird = 26;

  // Paint a square
  uint16_t x = (pos % 3);
  x += x * oneThird;
  uint16_t y = pos / 3;
  y += y * oneThird;

  // Add inset
  const uint16_t inset = 5;
  x += inset;
  y += inset;
  paintSquare(x, y, oneThird - (2 * inset));
}



void paintAI(uint8_t pos)
{
  const uint16_t oneThird = 26;

  uint16_t x = (pos % 3);
  x += x * oneThird;
  uint16_t y = pos / 3;
  y += y * oneThird;

  const uint16_t inset = 5;
  paintVerticalLine(x + (oneThird / 2), y + inset, y + oneThird - inset);
  paintHorizontalLine(y + (oneThird / 2), x + inset, x + oneThird - inset);
}

