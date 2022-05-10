#include <stdio.h>

#include "pico/stdlib.h"
#include "lib/fonts.h"
#include "lib/st7735.h"
#include "painting.h"
#include "logic.h"
#include "constants.h"
#include "lib/ICM20948.h"
#include "pico/multicore.h"

typedef enum
{
  Left,
  Right,
  Up,
  Down
} Move;

static void core1_entry();
static void clearScreen();
static void paintGrid(GridPos grid[]);
static void paintCursor();
static void updateCursor(GridPos grid[]);
static void paintHuman(uint8_t pos);
static void paintAI(uint8_t pos);
static void buttonCallback(uint gpio, uint32_t events);
static void updatePosWithMove(Move move);

static volatile int cursorPos = 0;

volatile GridPos grid[] = {
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

void core1_entry()
{
  float x;
  float y;
  float z;
  printf("Running core1_entry()\n");

  while (true)
  {
    sleep_ms(500);
    // Use the addresses of our variables. This function sets data at a pointer, so we give it three pointers.
    icm20948AccelRead(&x, &y, &z);
    // Left = +x
    // Up = +y
    printf("x\ty\n%.2f\t%.2f\n", x, y);

    const int xThreshold = 0.6;
    const int yThreshold = 0.5;
    Move move;

    // Detect action
    if (x > xThreshold)
      move = Left;
    else if (x < -xThreshold)
      move = Right;
    else if (y > yThreshold)
      move = Up;
    else if (y < -yThreshold)
      move = Down;

    if (move == NULL)
      continue;

    multicore_fifo_push_blocking(move);
  }
}

int main()
{
  // Initialise serial in/output
  stdio_init_all();

  // INITIALISE SCREEN
  // ---------------------------------------------------------------------------
  // Disable line and block buffering on stdout (for talking through serial)
  setvbuf(stdout, NULL, _IONBF, 0);
  // Give the Pico some time to think...
  sleep_ms(1000);
  // Initialise the screen
  ST7735_Init();
  clearScreen();

  // INITIALISE ACCELEROMETER
  // ---------------------------------------------------------------------------
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

  // INITIALISE BUTTON
  // ---------------------------------------------------------------------------
  gpio_init(BUTTON_GPIO);
  gpio_set_dir(BUTTON_GPIO, GPIO_IN);
  // We are using the button to pull down to 0v when pressed, so ensure that when
  // unpressed, it uses internal pull ups. Otherwise when unpressed, the input will
  // be floating.
  gpio_pull_up(BUTTON_GPIO);
  gpio_set_irq_enabled_with_callback(BUTTON_GPIO, GPIO_IRQ_EDGE_FALL, true, &buttonCallback);
  printf("Button initialised!\n");
  // ---------------------------------------------------------------------------

  // Start the second core to handle inputs
  multicore_launch_core1(core1_entry);

  paintGrid(grid);
  Player winner;
  while ((winner = isWinner(grid)) == empty)
  {
    Move move = multicore_fifo_pop_blocking();
    updatePosWithMove(move);
    paintGrid(grid);
    switch (move)
    {
    case Left:
      printf("Moved left\n");
      break;
    case Right:
      printf("Moved right\n");
      break;
    case Up:
      printf("Moved up\n");
      break;
    case Down:
      printf("Moved down\n");
      break;
    default:
      break;
    }
  }
  paintGrid(grid);

  printf("Winner is %d!!!\n", winner);

  // Remove once piece placing is implemented
  while (1)
    tight_loop_contents();
}

void buttonCallback(uint gpio, uint32_t events)
{
  printf("Button pressed, place piece\n");
  bool played = playPos(human, cursorPos, grid);
  if (!played)
    return;

  paintGrid(grid);
  // AI's turn
  int pos = aiPlay(grid);
  playPos(ai, pos, grid);
  paintGrid(grid);
}

void clearScreen()
{
  ST7735_FillScreen(ST7735_BLACK);
}

void updatePosWithMove(Move move)
{
  switch (move)
  {
  case Left:
    if (cursorPos % gridSize == 0)
    {
      printf("Rejecting move Left: %d\n", cursorPos);
      return;
    }
    cursorPos--;
    break;
  case Right:
    if ((cursorPos + 1) % gridSize == 0)
    {
      printf("Rejecting move Right: %d\n", cursorPos);
      return;
    }
    cursorPos++;
    break;
  case Up:
    if (cursorPos < gridSize)
    {
      printf("Rejecting move Up: %d\n", cursorPos);
      return;
    }
    cursorPos -= 3;
    break;
  case Down:
    if (cursorPos >= (gridSize * gridSize) - gridSize)
    {
      printf("Rejecting move Down: %d\n", cursorPos);
      return;
    }
    cursorPos += 3;
  }
}

void paintGrid(GridPos grid[])
{
  // Clear top half of screen
  ST7735_FillRectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT / 2, ST7735_BLACK);
  uint16_t oneThird = 27;
  uint16_t twoThirds = 53;

  // Paint the lines forming the grid
  paintVerticalLine(oneThird, 0, ST7735_WIDTH, ST7735_WHITE);
  paintVerticalLine(twoThirds, 0, ST7735_WIDTH, ST7735_WHITE);
  paintHorizontalLine(oneThird, 0, ST7735_WIDTH, ST7735_WHITE);
  paintHorizontalLine(twoThirds, 0, ST7735_WIDTH, ST7735_WHITE);

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
