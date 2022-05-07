#include <stdio.h>

#include "pico/stdlib.h"
#include "lib/fonts.h"
#include "lib/st7735.h"

const u_int16_t width = 80;
const u_int16_t height = 160;

int main()
{
  // Screen and other boilerplate initialisation
  stdio_init_all();                 // Initialise serial in/output
  setvbuf(stdout, NULL, _IONBF, 0); // Disable line and block buffering on stdout (for talking through serial)
  sleep_ms(1000);                   // Give the Pico some time to think...
  ST7735_Init();                    // Initialise the screen

  clearScreen();

  paintGrid(3);
}

void clearScreen()
{
  ST7735_FillScreen(ST7735_BLACK);
}

void paintVerticalLine(uint16_t x, uint16_t y1, uint16_t y2)
{
  // Swap variables if necessary
  if (y1 > y2)
  {
    uint16_t temp = y1;
    y1 = y2;
    y2 = temp;
  }
  for (uint16_t y = y1; y <= y2; y++)
  {
    ST7735_DrawPixel(x, y, ST7735_WHITE);
  }
}

void paintHorizontalLine(uint16_t y, uint16_t x1, uint16_t x2)
{
  // Swap variables if necessary
  if (x1 > x2)
  {
    uint16_t temp = x1;
    x1 = x2;
    x2 = temp;
  }
  for (uint16_t x = x1; x <= x2; x++)
  {
    ST7735_DrawPixel(x, y, ST7735_WHITE);
  }
}

void paintGrid(int size)
{
  paintVerticalLine(27, 0, width);
  paintVerticalLine(53, 0, width);
  paintHorizontalLine(27, 0, width);
  paintHorizontalLine(53, 0, width);
}
