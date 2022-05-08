#include "painting.h"

void paintSquare(uint16_t x, uint16_t y, uint16_t size)
{
    paintVerticalLine(x, y, y + size);
    paintVerticalLine(x + size, y, y + size);
    paintHorizontalLine(y, x, x + size);
    paintHorizontalLine(y + size, x, x + size);
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