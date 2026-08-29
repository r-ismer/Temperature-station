#include "graphics_engine_private.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ***********************************************************************************************************
// Function name : gfx_tile_draw                                                                             *
// Description   : Draws a tile into the supplied buffer based on its type.                                  *
// Input         : (struct gfx_tile_t*) tile : Tile to render.                                                 *
//                (gfx_buffer_t*) buffer : Target buffer.                                                      *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_draw(gfx_tile_t* tile, gfx_buffer_t* buffer)
{
    if (tile == NULL || buffer == NULL || !tile->visible) {
        return;
    }

    switch (tile->type) {
        case gfx_tile_rectangle:
            gfx_buffer_draw_rect(buffer, tile->x, tile->y, tile->width, tile->height, tile->color);
            break;
        case gfx_tile_circle:
            gfx_buffer_draw_circle(buffer, tile->x, tile->y, tile->width, tile->color);
            break;
        case gfx_tile_line:
            gfx_buffer_draw_line(buffer, tile->x, tile->y, tile->width, tile->height, tile->color);
            break;
        case gfx_tile_text:
            if (tile->font == NULL) {
                return;
            }
            gfx_buffer_draw_text(buffer, tile->x, tile->y, tile->text, tile->color, tile->font);
            break;
        case gfx_tile_bitmap:
            if (tile->bitmap != NULL) {
                gfx_buffer_draw_bitmap(
                    buffer,
                    tile->x,
                    tile->y,
                    tile->width,
                    tile->height,
                    tile->bitmap,
                    tile->color
                );
            }
            break;
        default:
            break;
    }
}

// ***********************************************************************************************************
// Function name : gfx_tile_create_internal                                                                   *
// Description   : Allocates and initializes a new tile object.                                              *
// Input         : (gfx_tile_type_t) type : Tile subtype to create.                                             *
//                (uint16_t) x : Tile X coordinate.                                                           *
//                (uint16_t) y : Tile Y coordinate.                                                           *
//                (uint16_t) width : Tile width.                                                              *
//                (uint16_t) height : Tile height.                                                            *
//                (uint32_t) color : Base tile color.                                                         *
// Return        : (gfx_tile_t*) : Pointer to the newly created tile or NULL on failure.                      *
// ***********************************************************************************************************
static gfx_tile_t* gfx_tile_create_internal(
    gfx_tile_type_t type,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint32_t color
)
{
    gfx_tile_t* tile = (gfx_tile_t*)calloc(1u, sizeof(gfx_tile_t));
    if (tile == NULL) {
        return NULL;
    }

    tile->type = type;
    tile->visible = true;
    tile->x = x;
    tile->y = y;
    tile->width = width;
    tile->height = height;
    tile->color = color;
    tile->next = NULL;
    tile->prev = NULL;
    tile->font = NULL;
    tile->bitmap = NULL;
    tile->owns_bitmap = false;

    return tile;
}

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
)
{
    gfx_tile_t* tile = gfx_tile_create_internal(
        gfx_tile_rectangle,
        x,
        y,
        width,
        height,
        color
    );
    return tile;
}

// ***********************************************************************************************************
// Function name : gfx_tile_create_circle                                                                     *
// Description   : Creates a circle tile centered at the given coordinates.                                  *
// Input         : (uint16_t) x : Center X coordinate.                                                       *
//                (uint16_t) y : Center Y coordinate.                                                       *
//                (uint16_t) radius : Circle radius in pixels.                                               *
//                (uint32_t) color : Circle fill color.                                                     *
// Return        : (gfx_tile_handle_t) : Handle to the created tile or NULL on failure.                         *
// ***********************************************************************************************************
gfx_tile_handle_t gfx_tile_create_circle(uint16_t x, uint16_t y, uint16_t radius, uint32_t color)
{
    gfx_tile_t* tile = gfx_tile_create_internal(gfx_tile_circle, x, y, radius, radius, color);
    return tile;
}

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
gfx_tile_handle_t gfx_tile_create_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    gfx_tile_t* tile = gfx_tile_create_internal(gfx_tile_line, x1, y1, x2, y2, color);
    return tile;
}

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
)
{
    if (font == NULL) {
        return NULL;
    }

    gfx_tile_t* tile = gfx_tile_create_internal(gfx_tile_text, x, y, 0U, 0U, color);
    if (tile == NULL) {
        return NULL;
    }

    tile->font = font;
    if (text != NULL) {
        snprintf(tile->text, sizeof(tile->text), "%s", text);
    }

    return tile;
}

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
)
{
    gfx_tile_t* tile = gfx_tile_create_internal(
        gfx_tile_bitmap,
        x,
        y,
        width,
        height,
        color
    );
    if (tile == NULL) {
        return NULL;
    }

    if (pixels != NULL) {
        const size_t pixel_count = (size_t)width * (size_t)height;
        tile->bitmap = (uint8_t*)malloc(pixel_count * sizeof(uint8_t));
        if (tile->bitmap != NULL) {
            memcpy(tile->bitmap, pixels, pixel_count * sizeof(uint8_t));
            tile->owns_bitmap = true;
        }
    }

    return tile;
}

// ***********************************************************************************************************
// Function name : gfx_tile_destroy                                                                          *
// Description   : Releases the memory used by a tile instance.                                              *
// Input         : (gfx_tile_handle_t) tile : Tile instance to destroy.                                         *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_destroy(gfx_tile_handle_t tile)
{
    if (tile == NULL) {
        return;
    }

    if (tile->owns_bitmap && tile->bitmap != NULL) {
        free(tile->bitmap);
    }

    free(tile);
}

// ***********************************************************************************************************
// Function name : gfx_tile_set_visible                                                                       *
// Description   : Enables or disables rendering of a tile.                                                  *
// Input         : (gfx_tile_handle_t) tile : Tile to update.                                                   *
//                (bool) visible : true to draw the tile, false to hide it.                                 *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_set_visible(gfx_tile_handle_t tile, bool visible)
{
    if (tile != NULL) {
        tile->visible = visible;
    }
}

// ***********************************************************************************************************
// Function name : gfx_tile_set_color                                                                         *
// Description   : Updates the color used for drawing the tile.                                              *
// Input         : (gfx_tile_handle_t) tile : Tile to update.                                                   *
//                (uint32_t) color : New color value.                                                       *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_set_color(gfx_tile_handle_t tile, uint32_t color)
{
    if (tile != NULL) {
        tile->color = color;
    }
}

// ***********************************************************************************************************
// Function name : gfx_tile_set_position                                                                      *
// Description   : Moves a tile to a new screen position.                                                    *
// Input         : (gfx_tile_handle_t) tile : Tile to update.                                                   *
//                (uint16_t) x : New X coordinate.                                                          *
//                (uint16_t) y : New Y coordinate.                                                          *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_set_position(gfx_tile_handle_t tile, uint16_t x, uint16_t y)
{
    if (tile != NULL) {
        tile->x = x;
        tile->y = y;
    }
}

// ***********************************************************************************************************
// Function name : gfx_tile_set_size                                                                          *
// Description   : Updates the width and height of a tile.                                                   *
// Input         : (gfx_tile_handle_t) tile : Tile to update.                                                   *
//                (uint16_t) width : New width in pixels.                                                   *
//                (uint16_t) height : New height in pixels.                                                 *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_tile_set_size(gfx_tile_handle_t tile, uint16_t width, uint16_t height)
{
    if (tile != NULL) {
        tile->width = width;
        tile->height = height;
    }
}
