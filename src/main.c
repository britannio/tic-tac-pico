#include <stdio.h>

#include "pico/stdlib.h"
#include "lib/fonts.h"
#include "lib/st7735.h"
#include "painting.h"
#include "logic.h"
#include "constants.h"

static volatile int cursorPos = 0;

int main()
{
  // Screen and other boilerplate initialisation
  stdio_init_all();                 // Initialise serial in/output
  setvbuf(stdout, NULL, _IONBF, 0); // Disable line and block buffering on stdout (for talking through serial)
  sleep_ms(1000);                   // Give the Pico some time to think...
  ST7735_Init();                    // Initialise the screen

  clearScreen();

  GridPos grid[] = {
      (GridPos){.player = empty, .winningPos = false},
      (GridPos){.player = empty, .winningPos = false},
      (GridPos){.player = empty, .winningPos = false},
      (GridPos){.player = empty, .winningPos = false},
      (GridPos){.player = empty, .winningPos = false},
      (GridPos){.player = empty, .winningPos = false},
      (GridPos){.player = empty, .winningPos = false},
      (GridPos){.player = empty, .winningPos = false},
      (GridPos){.player = empty, .winningPos = false},
  };

  bool isHumanTurn = true;
  Player winner;
  while ((winner = isWinner(grid)) == empty)
  {
    paintGrid(grid);
    if (isHumanTurn)
    {
      // Human's turn
      playPos(human, nextFreePos(grid), grid);
      // TODO select a position
      // TODO play the selected position
    }
    else
    {
      // AI's turn
      int pos = aiPlay(grid);
      playPos(ai, pos, grid);
    }
    isHumanTurn = !isHumanTurn;
    sleep_ms(1000);
  }
  paintGrid(grid);

  printf("Winner is %d!!!\n", winner);
}

void clearScreen()
{
  ST7735_FillScreen(ST7735_BLACK);
}

void paintGrid(GridPos grid[])
{
  uint16_t oneThird = 27;
  uint16_t twoThirds = 53;

  // Paint the lines forming the grid
  paintVerticalLine(oneThird, 0, width, ST7735_WHITE);
  paintVerticalLine(twoThirds, 0, width, ST7735_WHITE);
  paintHorizontalLine(oneThird, 0, width, ST7735_WHITE);
  paintHorizontalLine(twoThirds, 0, width, ST7735_WHITE);

  // Paint the players
  for (int i = 0; i < gridSize * gridSize; i++)
  {
    switch (grid[i].player)
    {
    case empty:
      break;
    case human:
      paintHuman(i);
      break;
    case ai:
      paintAI(i);
      break;
    }
  }

  paintCursor();
}

void paintCursor()
{
  // Roughly one third of the display.
  const uint16_t oneThird = 26;

  // Top left of the cell
  uint16_t x = cursorPos % 3;
  x += x * oneThird;
  uint16_t y = cursorPos / 3;
  y += y * oneThird;

  // Add inset
  const uint16_t inset = 12; // Just under half of the box
  x += inset;
  y += inset;
  uint16_t size = 4;
  ST7735_FillRectangle(x, y, size, size, ST7735_GREEN);
}

void updateCursor(GridPos grid[])
{
  cursorPos = nextFreePos(grid);
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
  paintSquare(x, y, oneThird - (2 * inset), ST7735_WHITE);
}

void paintAI(uint8_t pos)
{
  const uint16_t oneThird = 26;

  uint16_t x = (pos % 3);
  x += x * oneThird;
  uint16_t y = pos / 3;
  y += y * oneThird;

  const uint16_t inset = 5;
  paintVerticalLine(x + (oneThird / 2), y + inset, y + oneThird - inset, ST7735_WHITE);
  paintHorizontalLine(y + (oneThird / 2), x + inset, x + oneThird - inset, ST7735_WHITE);
}
