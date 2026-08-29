#ifndef GRAPHICS_ENGINE_PRIVATE_H_
#define GRAPHICS_ENGINE_PRIVATE_H_

#include "graphics_engine.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct gfx_buffer_t {
    uint16_t width;
    uint16_t height;
    gfx_pixel_format_t pixel_format;
    size_t stride;
    uint8_t* pixels;
} gfx_buffer_t;

struct gfx_tile_s {
    struct gfx_tile_s* next;
    struct gfx_tile_s* prev;
    gfx_tile_type_t type;
    bool visible;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint32_t color;
    char text[32];
    const gfx_font_t* font;
    uint8_t* bitmap;
    bool owns_bitmap;
};

typedef struct gfx_tile_s gfx_tile_t;

struct gfx_engine_s {
    uint16_t width;
    uint16_t height;
    gfx_display_driver_t display;
    gfx_buffer_t front;
    gfx_buffer_t back;
    gfx_tile_t* tiles_head;
    gfx_tile_t* tiles_tail;
    size_t tile_count;
};

// ***********************************************************************************************************
// Function name : gfx_buffer_clear                                                                          *
// Description   : Clears a buffer with a solid color.                                                       *
// Input         : (gfx_buffer_t*) buffer : Buffer to clear.                                                   *
//                (uint32_t) color : Fill color.                                                             *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_buffer_clear(gfx_buffer_t* buffer, uint32_t color);

// ***********************************************************************************************************
// Function name : gfx_buffer_put_pixel                                                                       *
// Description   : Writes a single pixel into a frame buffer.                                                *
// Input         : (gfx_buffer_t*) buffer : Target buffer.                                                     *
//                (int) x : X coordinate.                                                                    *
//                (int) y : Y coordinate.                                                                    *
//                (uint32_t) color : Pixel color.                                                            *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_buffer_put_pixel(
    gfx_buffer_t* buffer,
    int x,
    int y,
    uint32_t color
);

// ***********************************************************************************************************
// Function name : gfx_buffer_draw_rect                                                                       *
// Description   : Draws a filled rectangle in the buffer.                                                   *
// Input         : (gfx_buffer_t*) buffer : Target buffer.                                                     *
//                (uint16_t) x : Start X position.                                                           *
//                (uint16_t) y : Start Y position.                                                           *
//                (uint16_t) width : Rectangle width.                                                        *
//                (uint16_t) height : Rectangle height.                                                      *
//                (uint32_t) color : Fill color.                                                             *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_buffer_draw_rect(
    gfx_buffer_t* buffer,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint32_t color
);

// ***********************************************************************************************************
// Function name : gfx_buffer_draw_circle                                                                     *
// Description   : Draws a circle in the buffer.                                                             *
// Input         : (gfx_buffer_t*) buffer : Target buffer.                                                     *
//                (uint16_t) x0 : Center X coordinate.                                                       *
//                (uint16_t) y0 : Center Y coordinate.                                                       *
//                (uint16_t) radius : Circle radius.                                                         *
//                (uint32_t) color : Circle color.                                                           *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_buffer_draw_circle(
    gfx_buffer_t* buffer,
    uint16_t x0,
    uint16_t y0,
    uint16_t radius,
    uint32_t color
);

// ***********************************************************************************************************
// Function name : gfx_buffer_draw_line                                                                       *
// Description   : Draws a line between two points.                                                          *
// Input         : (gfx_buffer_t*) buffer : Target buffer.                                                     *
//                (uint16_t) x1 : Start X coordinate.                                                        *
//                (uint16_t) y1 : Start Y coordinate.                                                        *
//                (uint16_t) x2 : End X coordinate.                                                          *
//                (uint16_t) y2 : End Y coordinate.                                                          *
//                (uint32_t) color : Line color.                                                             *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_buffer_draw_line(
    gfx_buffer_t* buffer,
    uint16_t x1,
    uint16_t y1,
    uint16_t x2,
    uint16_t y2,
    uint32_t color
);

// ***********************************************************************************************************
// Function name : gfx_buffer_draw_bitmap                                                                     *
// Description   : Draws a bitmap image into the buffer.                                                     *
// Input         : (gfx_buffer_t*) buffer : Target buffer.                                                     *
//                (uint16_t) x : Start X position.                                                           *
//                (uint16_t) y : Start Y position.                                                           *
//                (uint16_t) width : Bitmap width.                                                           *
//                (uint16_t) height : Bitmap height.                                                         *
//                (const uint32_t*) pixels : Pixel array.                                                    *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_buffer_draw_bitmap(
    gfx_buffer_t* buffer,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    const uint8_t* pixels,
    uint32_t color
);

// ***********************************************************************************************************
// ***********************************************************************************************************
// Function name : gfx_buffer_draw_glyph                                                                     *
// Description   : Draws a single glyph from a bitmap font into the target buffer.                           *
// Input         : (gfx_buffer_t*) buffer : Target buffer.                                                     *
//                (uint16_t) x : Start X position.                                                           *
//                (uint16_t) y : Start Y position.                                                           *
//                (char) ch : Character to draw.                                                             *
//                (uint32_t) color : Glyph.                                                                  *
//                (const gfx_font_t*) font : Font.                                                             *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_buffer_draw_glyph(
    gfx_buffer_t* buffer,
    uint16_t x,
    uint16_t y,
    char ch,
    uint32_t color,
    const gfx_font_t* font
);

// ***********************************************************************************************************
// Function name : gfx_buffer_draw_text                                                                       *
// Description   : Draws a text string in the buffer using a bitmap font.                                    *
// Input         : (gfx_buffer_t*) buffer : Target buffer.                                                     *
//                (uint16_t) x : Start X position.                                                           *
//                (uint16_t) y : Start Y position.                                                           *
//                (const char*) text : String to draw.                                                       *
//                (uint32_t) color : Text color.                                                             *
//                (const gfx_font_t*) font : Font for the string.                                              *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_buffer_draw_text(
    gfx_buffer_t* buffer,
    uint16_t x,
    uint16_t y,
    const char* text,
    uint32_t color,
    const gfx_font_t* font
);

// ***********************************************************************************************************
// Function name : gfx_tile_draw                                                                             *
// Description   : Draws one tile into the supplied buffer.                                                  *
// Input         : (struct gfx_tile_s*) tile : Tile to render.                                               *
//                (gfx_buffer_t*) buffer : Target buffer.                                                      *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_draw(gfx_tile_t* tile, gfx_buffer_t* buffer);

#endif
