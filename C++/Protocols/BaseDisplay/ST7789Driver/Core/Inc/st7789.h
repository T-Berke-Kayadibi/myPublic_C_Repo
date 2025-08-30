/*
 * st7789.h
 *
 *  Created on: Jul 13, 2025
 *      Author: tarikberkekayadibi
 */

#ifndef ST7789_H
#define ST7789_H

#include "stm32f1xx_hal.h"
#include "fonts.h" // Include font declarations

// User-configurable settings
#define ST7789_SPI_PORT hspi1 // Set to your SPI handle
extern SPI_HandleTypeDef ST7789_SPI_PORT;

// Display dimensions
#define ST7789_WIDTH  240
#define ST7789_HEIGHT 240

// Color definitions
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF

// Public function prototypes
void ST7789_Init(void);
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7789_Fill_Color(uint16_t color);
void ST7789_Fill_Rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789_SetRotation(uint8_t r);

#endif //ST7789_H
