#include <stdio.h>

#include "pico/stdlib.h"
#include "lib/fonts.h"
#include "lib/st7735.h"
#include "painting.h"
#include "logic.h"
#include "constants.h"
#include "lib/ICM20948.h"
#include "pico/multicore.h"

static void core1_entry();
static void clearScreen();
static void paintGrid(GridPos grid[]);
static void paintCursor();
static void updateCursor(GridPos grid[]);
static void paintHuman(uint8_t pos);
static void paintAI(uint8_t pos);

static volatile int cursorPos = 0;

typedef enum
{
  Left,
  Right,
  Up,
  Down
} Move;

void core1_entry()
{
  float x;
  float y;
  float z;
  printf("Running core1_entry()\n");

  while (true)
  {
    icm20948AccelRead(&x, &y, &z); // Use the addresses of our variables. This function sets data at a pointer, so we give it three pointers.
    // Left = +x
    // Up = +y
    printf("x\ty\n%.2f\t%.2f\n", x, y);
    sleep_ms(500);
    char xString[100];
    int xChars = sprintf(xString, "%.2f", x);
    char yString[100];
    int yChars = sprintf(yString, "%.2f", y);
    // Clear the bottom half of the screen
    ST7735_FillRectangle(0, 80, ST7735_WIDTH / 2, ST7735_HEIGHT / 2, ST7735_BLACK);

    // Write X and Y values to display
    ST7735_WriteString(7, 80, xString, Font_16x26, ST7735_RED, ST7735_BLACK);
    ST7735_WriteString(7, 120, yString, Font_16x26, ST7735_GREEN, ST7735_BLACK);

  }

  // multicore_fifo_push_blocking(FLAG_VALUE);

  // uint32_t g = multicore_fifo_pop_blocking();

  // if (g != FLAG_VALUE)
  //     printf("Hmm, that's not right on core 1!\n");
  // else
  //     printf("Its all gone well on core 1!");

  // while (1)
  //     tight_loop_contents();
}

int main()
{
  // Screen and other boilerplate initialisation
  stdio_init_all();                 // Initialise serial in/output
  setvbuf(stdout, NULL, _IONBF, 0); // Disable line and block buffering on stdout (for talking through serial)
  sleep_ms(1000);                   // Give the Pico some time to think...
  ST7735_Init();                    // Initialise the screen
  // ---------------------------------------------------------------------------
  // Accelerometer (Critically important, but I do not know what it does.)
  i2c_init(i2c0, 400 * 1000);
  gpio_set_function(4, GPIO_FUNC_I2C);
  gpio_set_function(5, GPIO_FUNC_I2C);
  gpio_pull_up(4);
  gpio_pull_up(5);

  IMU_EN_SENSOR_TYPE enMotionSensorType;

  imuInit(&enMotionSensorType);
  if (IMU_EN_SENSOR_TYPE_ICM20948 != enMotionSensorType)
  {
    printf("Failed to initialise IMU...\n");
  }

  printf("IMU initialised!\n");
  // ---------------------------------------------------------------------------

  clearScreen();
  // Start the second core to handle inputs
  multicore_launch_core1(core1_entry);

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
  
  // Remove once piece placing is implemented
  while (1)
    tight_loop_contents();
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
