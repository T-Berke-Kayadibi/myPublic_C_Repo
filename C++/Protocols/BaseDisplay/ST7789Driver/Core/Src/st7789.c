/*
 * st7789.c
 *
 *  Created on: Jul 13, 2025
 *      Author: tarikberkekayadibi
 */


#include "main.h"
#include "st7789.h"
#include <string.h>

// Private function prototypes
static void ST7789_WriteCommand(uint8_t cmd);
static void ST7789_WriteData(uint8_t *buff, size_t buff_size);
static void ST7789_WriteSmallData(uint8_t data);
static void ST7789_Reset(void);
static void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

// We will manually control a Chip Select (CS) pin for stability.
// Let's use PA4 for this. You must configure PA4 as GPIO_Output in CubeMX.
#define ST7789_CS_PORT GPIOA
#define ST7789_CS_PIN  GPIO_PIN_4

#define ST7789_CS_Low()  HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET)
#define ST7789_CS_High() HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET)

// GPIO control functions using definitions from main.h
#define ST7789_RES_Clr() HAL_GPIO_WritePin(ST7789_RES_PIN_GPIO_Port, ST7789_RES_PIN_Pin, GPIO_PIN_RESET)
#define ST7789_RES_Set() HAL_GPIO_WritePin(ST7789_RES_PIN_GPIO_Port, ST7789_RES_PIN_Pin, GPIO_PIN_SET)
#define ST7789_DC_Clr()  HAL_GPIO_WritePin(ST7789_DC_PIN_GPIO_Port, ST7789_DC_PIN_Pin, GPIO_PIN_RESET)
#define ST7789_DC_Set()  HAL_GPIO_WritePin(ST7789_DC_PIN_GPIO_Port, ST7789_DC_PIN_Pin, GPIO_PIN_SET)
#define ST7789_BLK_Set() HAL_GPIO_WritePin(ST7789_BLK_PIN_GPIO_Port, ST7789_BLK_PIN_Pin, GPIO_PIN_SET)


static void ST7789_Reset(void) {
    ST7789_RES_Set();
    HAL_Delay(50);
    ST7789_RES_Clr();
    HAL_Delay(50);
    ST7789_RES_Set();
    HAL_Delay(50);
}

static void ST7789_WriteCommand(uint8_t cmd) {
    ST7789_DC_Clr();
    ST7789_CS_Low();
    HAL_SPI_Transmit(&ST7789_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
    ST7789_CS_High();
}

static void ST7789_WriteData(uint8_t *buff, size_t buff_size) {
    ST7789_DC_Set();
    ST7789_CS_Low();
    HAL_SPI_Transmit(&ST7789_SPI_PORT, buff, buff_size, HAL_MAX_DELAY);
    ST7789_CS_High();
}

static void ST7789_WriteSmallData(uint8_t data) {
    ST7789_DC_Set();
    ST7789_CS_Low();
    HAL_SPI_Transmit(&ST7789_SPI_PORT, &data, sizeof(data), HAL_MAX_DELAY);
    ST7789_CS_High();
}

void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t data[4];

    // Column Address Set
    ST7789_WriteCommand(0x2A);
    data[0] = (x0 >> 8) & 0xFF;
    data[1] = x0 & 0xFF;
    data[2] = (x1 >> 8) & 0xFF;
    data[3] = x1 & 0xFF;
    ST7789_WriteData(data, sizeof(data));

    // Row Address Set
    ST7789_WriteCommand(0x2B);
    data[0] = (y0 >> 8) & 0xFF;
    data[1] = y0 & 0xFF;
    data[2] = (y1 >> 8) & 0xFF;
    data[3] = y1 & 0xFF;
    ST7789_WriteData(data, sizeof(data));

    // Write to RAM
    ST7789_WriteCommand(0x2C);
}

// NEW INITIALIZATION SEQUENCE BASED ON ADAFRUIT LIBRARY
void ST7789_Init(void) {
    ST7789_Reset();

    ST7789_WriteCommand(0x01); // Software Reset
    HAL_Delay(150);

    ST7789_WriteCommand(0x11); // Sleep Out
    HAL_Delay(10);

    ST7789_WriteCommand(0x3A); // Interface Pixel Format
    ST7789_WriteSmallData(0x55); // 16-bit/pixel
    HAL_Delay(10);

    ST7789_WriteCommand(0x36); // Memory Data Access Control
    ST7789_WriteSmallData(0x00); // Default rotation

    // These settings for a 240x240 display might need adjustment
    // if your display is different, but they are a good starting point.
    uint8_t caset[4] = {0x00, 0x00, (ST7789_WIDTH-1) >> 8, (ST7789_WIDTH-1) & 0xFF};
    ST7789_WriteCommand(0x2A); // Column Address Set
    ST7789_WriteData(caset, 4);

    uint8_t raset[4] = {0x00, 0x00, (ST7789_HEIGHT-1) >> 8, (ST7789_HEIGHT-1) & 0xFF};
    ST7789_WriteCommand(0x2B); // Row Address Set
    ST7789_WriteData(raset, 4);

    ST7789_WriteCommand(0x21); // Display Inversion On
    HAL_Delay(10);

    ST7789_WriteCommand(0x13); // Normal Display Mode On
    HAL_Delay(10);

    ST7789_WriteCommand(0x29); // Display ON
    HAL_Delay(10);

    ST7789_BLK_Set(); // Turn on backlight
}


void ST7789_Fill_Color(uint16_t color) {
    uint16_t i, j;
    ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);

    for (i = 0; i < ST7789_HEIGHT; i++) {
        for (j = 0; j < ST7789_WIDTH; j++) {
            uint8_t data[] = {color >> 8, color & 0xFF};
            ST7789_WriteData(data, sizeof(data));
        }
    }
}

void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if ((x < 0) || (x >= ST7789_WIDTH) || (y < 0) || (y >= ST7789_HEIGHT)) {
        return;
    }
    ST7789_SetAddressWindow(x, y, x, y);
    uint8_t data[] = {color >> 8, color & 0xFF};
    ST7789_WriteData(data, sizeof(data));
}

void ST7789_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    while (*str) {
        if (x + font.width >= ST7789_WIDTH) {
            x = 0;
            y += font.height;
            if (y + font.height >= ST7789_HEIGHT) {
                break;
            }
        }
        // Draw character
        for (int i = 0; i < font.height; i++) {
            uint16_t b = font.data[(*str - 32) * font.height + i];
            for (int j = 0; j < font.width; j++) {
                if ((b << j) & 0x8000) {
                    ST7789_DrawPixel(x + j, y + i, color);
                } else {
                    ST7789_DrawPixel(x + j, y + i, bgcolor);
                }
            }
        }
        x += font.width;
        str++;
    }
}

void ST7789_Fill_Rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if ((x >= ST7789_WIDTH) || (y >= ST7789_HEIGHT)) return;
    if ((x + w) > ST7789_WIDTH) w = ST7789_WIDTH - x;
    if ((y + h) > ST7789_HEIGHT) h = ST7789_HEIGHT - y;

    ST7789_SetAddressWindow(x, y, x + w - 1, y + h - 1);

    uint8_t data[] = {color >> 8, color & 0xFF};
    uint32_t size = w * h;

    ST7789_DC_Set();
    ST7789_CS_Low();
    for(uint32_t i = 0; i < size; i++) {
        HAL_SPI_Transmit(&ST7789_SPI_PORT, data, 2, HAL_MAX_DELAY);
    }
    ST7789_CS_High();
}
