#ifndef GRAPHICS_TILE_H_
#define GRAPHICS_TILE_H_

#include <stdbool.h>
#include <stdint.h>

#include "graphics_font.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gfx_buffer_t gfx_buffer_t;
typedef struct gfx_tile_s gfx_tile_t;
typedef gfx_tile_t* gfx_tile_handle_t;

typedef enum gfx_tile_type_t {
    gfx_tile_rectangle = 0,
    gfx_tile_circle = 1,
    gfx_tile_line = 2,
    gfx_tile_text = 3,
    gfx_tile_bitmap = 4
} gfx_tile_type_t;

// ***********************************************************************************************************
// Function name : gfx_tile_create_rectangle                                                                  *
// Description   : Creates a rectangle tile with a solid color fill.                                         *
// Input         : (uint16_t) x : Top-left X coordinate.                                                     *
//                (uint16_t) y : Top-left Y coordinate.                                                     *
//                (uint16_t) width : Rectangle width in pixels.                                             *
//                (uint16_t) height : Rectangle height in pixels.                                           *
//                (uint32_t) color : Fill color used by the tile.                                           *
// Return        : (gfx_tile_handle_t) : Handle to the created tile or NULL on failure.                         *
// ***********************************************************************************************************
gfx_tile_handle_t gfx_tile_create_rectangle(
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint32_t color
);

// ***********************************************************************************************************
// Function name : gfx_tile_create_circle                                                                     *
// Description   : Creates a circle tile centered at the given coordinates.                                  *
// Input         : (uint16_t) x : Center X coordinate.                                                       *
//                (uint16_t) y : Center Y coordinate.                                                       *
//                (uint16_t) radius : Circle radius in pixels.                                               *
//                (uint32_t) color : Circle fill color.                                                     *
// Return        : (gfx_tile_handle_t) : Handle to the created tile or NULL on failure.                         *
// ***********************************************************************************************************
gfx_tile_handle_t gfx_tile_create_circle(uint16_t x, uint16_t y, uint16_t radius, uint32_t color);

// ***********************************************************************************************************
// Function name : gfx_tile_create_line                                                                       *
// Description   : Creates a line tile from one point to another.                                            *
// Input         : (uint16_t) x1 : Start X coordinate.                                                      *
//                (uint16_t) y1 : Start Y coordinate.                                                      *
//                (uint16_t) x2 : End X coordinate.                                                        *
//                (uint16_t) y2 : End Y coordinate.                                                        *
//                (uint32_t) color : Line color.                                                            *
// Return        : (gfx_tile_handle_t) : Handle to the created tile or NULL on failure.                         *
// ***********************************************************************************************************
gfx_tile_handle_t gfx_tile_create_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);

// ***********************************************************************************************************
// Function name : gfx_tile_create_text                                                                       *
// Description   : Creates a text tile with a null-terminated string and an explicit font.                   *
// Input         : (uint16_t) x : Top-left X coordinate.                                                     *
//                (uint16_t) y : Top-left Y coordinate.                                                     *
//                (const char*) text : Text to display.                                                     *
//                (uint32_t) color : Text color.                                                            *
//                (const gfx_font_t*) font : Font used by this text tile.                                    *
// Return        : (gfx_tile_handle_t) : Handle to the created tile or NULL on failure.                         *
// ***********************************************************************************************************
gfx_tile_handle_t gfx_tile_create_text(
    uint16_t x,
    uint16_t y,
    const char* text,
    uint32_t color,
    const gfx_font_t* font
);

// ***********************************************************************************************************
// Function name : gfx_tile_create_bitmap                                                                     *
// Description   : Creates bitmap tile.                                                                      *
// Input         : (uint16_t) x : Top-left X coordinate.                                                     *
//                (uint16_t) y : Top-left Y coordinate.                                                     *
//                (uint16_t) width : Bitmap width in pixels.                                                 *
//                (uint16_t) height : Bitmap height in pixels.                                               *
//                (const uint8_t*) pixels : Pixel data.                                                       *
//                (uint32_t) color : Color modifier applied to the bitmap.                                   *
// Return        : (gfx_tile_handle_t) : Handle to the created tile or NULL on failure.                         *
// ***********************************************************************************************************
gfx_tile_handle_t gfx_tile_create_bitmap(
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    const uint8_t* pixels,
    uint32_t color
);

// ***********************************************************************************************************
// Function name : gfx_tile_destroy                                                                          *
// Description   : Releases the memory used by a tile instance.                                              *
// Input         : (gfx_tile_handle_t) tile : Tile instance to destroy.                                         *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_destroy(gfx_tile_handle_t tile);

// ***********************************************************************************************************
// Function name : gfx_tile_set_visible                                                                       *
// Description   : Enables or disables rendering of a tile.                                                  *
// Input         : (gfx_tile_handle_t) tile : Tile to update.                                                   *
//                (bool) visible : true to draw the tile, false to hide it.                                 *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_set_visible(gfx_tile_handle_t tile, bool visible);

// ***********************************************************************************************************
// Function name : gfx_tile_set_color                                                                         *
// Description   : Updates the color used for drawing the tile.                                              *
// Input         : (gfx_tile_handle_t) tile : Tile to update.                                                   *
//                (uint32_t) color : New color value.                                                       *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_set_color(gfx_tile_handle_t tile, uint32_t color);

// ***********************************************************************************************************
// Function name : gfx_tile_set_position                                                                      *
// Description   : Moves a tile to a new screen position.                                                    *
// Input         : (gfx_tile_handle_t) tile : Tile to update.                                                   *
//                (uint16_t) x : New X coordinate.                                                          *
//                (uint16_t) y : New Y coordinate.                                                          *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_set_position(gfx_tile_handle_t tile, uint16_t x, uint16_t y);

// ***********************************************************************************************************
// Function name : gfx_tile_set_size                                                                          *
// Description   : Updates the width and height of a tile.                                                   *
// Input         : (gfx_tile_handle_t) tile : Tile to update.                                                   *
//                (uint16_t) width : New width in pixels.                                                   *
//                (uint16_t) height : New height in pixels.                                                 *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_set_size(gfx_tile_handle_t tile, uint16_t width, uint16_t height);

#ifdef __cplusplus
}
#endif

#endif
