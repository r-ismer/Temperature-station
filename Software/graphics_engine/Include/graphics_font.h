#ifndef GRAPHICS_FONT_H_
#define GRAPHICS_FONT_H_

#include <stdint.h>

// Font structure
typedef struct gfx_font_t {
    uint8_t width;
    uint8_t height;
    uint8_t spacing;
    uint8_t first_char;
    uint8_t last_char;
    const uint8_t* data;
} gfx_font_t;

// Default fonts 
extern const gfx_font_t gfx_font_default_small;
extern const gfx_font_t gfx_font_default_large;

const gfx_font_t* gfx_font_get_default(void);
const gfx_font_t* gfx_font_get_default_small(void);
const gfx_font_t* gfx_font_get_default_large(void);

#endif
