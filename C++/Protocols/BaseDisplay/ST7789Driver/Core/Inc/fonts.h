/*
 * fonts.h
 *
 *  Created on: Jul 13, 2025
 *      Author: tarikberkekayadibi
 */

#ifndef INC_FONTS_H_
#define INC_FONTS_H_

#include <stdint.h> // Use standard integer types

// Font structure
typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;

// Font declaration (tells the compiler this exists elsewhere)
extern FontDef Font_11x18;

#endif /* INC_FONTS_H_ */

