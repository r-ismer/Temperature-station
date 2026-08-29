#include "graphics_engine_private.h"

#include <stddef.h>
#include <stdio.h>

// ***********************************************************************************************************
// Function name : gfx_buffer_get_storage_size                                                                 *
// Description   : Calculates the required storage size for a buffer based on its dimensions and pixel format*
// ***********************************************************************************************************
static size_t gfx_buffer_get_storage_size(
    uint16_t width,
    uint16_t height,
    gfx_pixel_format_t pixel_format
)
{
    const size_t pixel_count = (size_t)width * (size_t)height;

    switch (pixel_format) {
        case gfx_pixel_format_1bpp:
            return (pixel_count + 7U) / 8U;
        case gfx_pixel_format_8bpp:
            return pixel_count;
        case gfx_pixel_format_rgb565:
            return pixel_count * 2U;
        case gfx_pixel_format_rgb888:
            return pixel_count * 3U;
        default:
            return pixel_count * 2U;
    }
}
// ***********************************************************************************************************
// Function name : gfx_buffer_encode_rgb565                                                                   *
// Description   : Converts a 32-bit RGB color to a 16-bit RGB565 format.                                    *
// ***********************************************************************************************************
static uint16_t gfx_buffer_encode_rgb565(uint32_t color)
{
    const uint8_t red = (uint8_t)((color >> 16U) & 0xFFU);
    const uint8_t green = (uint8_t)((color >> 8U) & 0xFFU);
    const uint8_t blue = (uint8_t)(color & 0xFFU);

    return (uint16_t)((((uint16_t)(red >> 3U)) << 11U) |
                      ((((uint16_t)green >> 2U)) << 5U) |
                      ((uint16_t)(blue >> 3U)));
}

// ***********************************************************************************************************
// Function name : gfx_buffer_write_pixel                                                                     *
// Description   : Writes a pixel value to a specific index in the buffer.                                   *
// ***********************************************************************************************************
static void gfx_buffer_write_pixel(
    gfx_buffer_t* buffer,
    size_t index,
    uint32_t color
)
{
    if (buffer == NULL || buffer->pixels == NULL) {
        return;
    }

    switch (buffer->pixel_format) {
        case gfx_pixel_format_1bpp: {
            const uint8_t bit_index = (uint8_t)(index % 8U);
            const size_t byte_index = index / 8U;
            const uint8_t mask = (uint8_t)(1U << (7U - bit_index));

            if (color != 0U) {
                buffer->pixels[byte_index] |= mask;
            } else {
                buffer->pixels[byte_index] &= (uint8_t)(~mask);
            }
            break;
        }
        case gfx_pixel_format_8bpp: {
            buffer->pixels[index] = (uint8_t)(color & 0xFFU);
            break;
        }
        case gfx_pixel_format_rgb565: {
            uint16_t* pixel_data = (uint16_t*)buffer->pixels;
            pixel_data[index] = gfx_buffer_encode_rgb565(color);
            break;
        }
        case gfx_pixel_format_rgb888: {
            uint8_t* pixel_data = buffer->pixels;
            const size_t offset = index * 3U;
            pixel_data[offset + 0U] = (uint8_t)((color >> 16U) & 0xFFU);
            pixel_data[offset + 1U] = (uint8_t)((color >> 8U) & 0xFFU);
            pixel_data[offset + 2U] = (uint8_t)(color & 0xFFU);
            break;
        }
        default:
            break;
    }
}

// ***********************************************************************************************************
// Function name : gfx_engine_initialize_buffer                                                               *
// Description   : Allocates and configures a frame buffer with a requested size.                            *
// ***********************************************************************************************************
static void gfx_engine_initialize_buffer(
    gfx_buffer_t* buffer,
    uint16_t width,
    uint16_t height,
    gfx_pixel_format_t pixel_format
)
{
    const size_t storage_size = gfx_buffer_get_storage_size(width, height, pixel_format);

    buffer->width = width;
    buffer->height = height;
    buffer->pixel_format = pixel_format;
    buffer->stride = (size_t)width;
    buffer->pixels = (uint8_t*)calloc(storage_size, sizeof(uint8_t));
}

// ***********************************************************************************************************
// Function name : gfx_engine_free_buffer                                                                     *
// Description   : Releases the pixel memory held by a buffer and resets its dimensions.                     *
// ***********************************************************************************************************
static void gfx_engine_free_buffer(gfx_buffer_t* buffer)
{
    if (buffer != NULL && buffer->pixels != NULL) {
        free(buffer->pixels);
        buffer->pixels = NULL;
    }
    buffer->width = 0;
    buffer->height = 0;
    buffer->pixel_format = gfx_pixel_format_rgb565;
    buffer->stride = 0U;
}

// ***********************************************************************************************************
// Function name : gfx_buffer_is_inside                                                                       *
// Description   : Verifies whether a coordinate is within the buffer bounds.                                *
// ***********************************************************************************************************
static bool gfx_buffer_is_inside(const gfx_buffer_t* buffer, int x, int y)
{
    return (buffer != NULL) &&
           (x >= 0) && (y >= 0) &&
           ((uint32_t)x < buffer->width) && ((uint32_t)y < buffer->height);
}

// ***********************************************************************************************************
// Function name : gfx_buffer_clear                                                                          *
// Description   : Fills a buffer with a solid-color value.                                                  *
// ***********************************************************************************************************
void gfx_buffer_clear(gfx_buffer_t* buffer, uint32_t color)
{
    if (buffer == NULL || buffer->pixels == NULL) {
        return;
    }

    switch (buffer->pixel_format) {
        case gfx_pixel_format_1bpp:
            memset(buffer->pixels, (color != 0U) ? 0xFFU : 0x00U,
                   gfx_buffer_get_storage_size(
                       buffer->width,
                       buffer->height,
                       buffer->pixel_format
                   ));
            break;
        case gfx_pixel_format_8bpp:
            memset(buffer->pixels, (int)(color & 0xFFU),
                   gfx_buffer_get_storage_size(
                       buffer->width,
                       buffer->height,
                       buffer->pixel_format
                   ));
            break;
        case gfx_pixel_format_rgb565: {
            uint16_t* pixel_data = (uint16_t*)buffer->pixels;
            const uint16_t packed_color = gfx_buffer_encode_rgb565(color);
            const size_t count = ((size_t)buffer->width * (size_t)buffer->height);
            for (size_t i = 0; i < count; ++i) {
                pixel_data[i] = packed_color;
            }
            break;
        }
        case gfx_pixel_format_rgb888: {
            const size_t count = ((size_t)buffer->width * (size_t)buffer->height);
            for (size_t i = 0; i < count; ++i) {
                gfx_buffer_write_pixel(buffer, i, color);
            }
            break;
        }
        default:
            break;
    }
}

// ***********************************************************************************************************
// Function name : gfx_buffer_put_pixel                                                                       *
// Description   : Writes a single pixel in the target buffer when it is inside the clip area.               *
// ***********************************************************************************************************
void gfx_buffer_put_pixel(gfx_buffer_t* buffer, int x, int y, uint32_t color)
{
    if (buffer == NULL || buffer->pixels == NULL) {
        return;
    }

    if (!gfx_buffer_is_inside(buffer, x, y)) {
        return;
    }

    const size_t index = ((size_t)y * (size_t)buffer->width) + (size_t)x;
    gfx_buffer_write_pixel(buffer, index, color);
}

// ***********************************************************************************************************
// Function name : gfx_buffer_draw_rect                                                                       *
// Description   : Draws a filled rectangle in the target buffer.                                            *
// ***********************************************************************************************************
void gfx_buffer_draw_rect(
    gfx_buffer_t* buffer,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint32_t color
)
{
    if (buffer == NULL || buffer->pixels == NULL) {
        return;
    }

    for (uint16_t py = y; py < (uint16_t)((uint32_t)y + (uint32_t)height); ++py) {
        for (uint16_t px = x; px < (uint16_t)((uint32_t)x + (uint32_t)width); ++px) {
           gfx_buffer_put_pixel(buffer, (int)px, (int)py, color);
        }
    }
}

// ***********************************************************************************************************
// Function name : gfx_buffer_draw_circle                                                                     *
// Description   : Draws a filled circle in the target buffer using a midpoint algorithm.                    *
// ***********************************************************************************************************
void gfx_buffer_draw_circle(gfx_buffer_t* buffer, uint16_t x0, uint16_t y0, uint16_t radius, uint32_t color)
{
    if (buffer == NULL || buffer->pixels == NULL) {
        return;
    }

    int x = (int)radius;
    int y = 0;
    int decision = 1 - x;

    while (x >= y) {
        gfx_buffer_put_pixel(buffer, (int)x0 + x, (int)y0 + y, color);
        gfx_buffer_put_pixel(buffer, (int)x0 - x, (int)y0 + y, color);
        gfx_buffer_put_pixel(buffer, (int)x0 + x, (int)y0 - y, color);
        gfx_buffer_put_pixel(buffer, (int)x0 - x, (int)y0 - y, color);
        gfx_buffer_put_pixel(buffer, (int)x0 + y, (int)y0 + x, color);
        gfx_buffer_put_pixel(buffer, (int)x0 - y, (int)y0 + x, color);
        gfx_buffer_put_pixel(buffer, (int)x0 + y, (int)y0 - x, color);
        gfx_buffer_put_pixel(buffer, (int)x0 - y, (int)y0 - x, color);

        y++;
        if (decision < 0) {
           decision += 2 * y + 1;
        } else {
           x--;
           decision += 2 * (y - x + 1);
        }
    }
}

// ***********************************************************************************************************
// Function name : gfx_buffer_draw_line                                                                       *
// Description   : Draws a line between two points on the target buffer.                                     *
// ***********************************************************************************************************
void gfx_buffer_draw_line(
    gfx_buffer_t* buffer,
    uint16_t x1,
    uint16_t y1,
    uint16_t x2,
    uint16_t y2,
    uint32_t color
)
{
    if (buffer == NULL || buffer->pixels == NULL) {
        return;
    }

    int dx = (int)x2 - (int)x1;
    int dy = (int)y2 - (int)y1;
    int sx = (dx >= 0) ? 1 : -1;
    int sy = (dy >= 0) ? 1 : -1;
    dx = (dx >= 0) ? dx : -dx;
    dy = (dy >= 0) ? dy : -dy;

    int err = ((dx > dy) ? dx : -dy) / 2;
    int x = (int)x1;
    int y = (int)y1;

    while (1) {
        gfx_buffer_put_pixel(buffer, x, y, color);
        if (x == (int)x2 && y == (int)y2) {
            break;
        }

        int e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x += sx;
        }
        if (e2 < dy) {
            err += dx;
            y += sy;
        }
    }
}

// ***********************************************************************************************************
// Function name : gfx_buffer_draw_bitmap                                                                     *
// Description   : Draws a bitmap image into the target buffer.                                              *
// ***********************************************************************************************************
void gfx_buffer_draw_bitmap(
    gfx_buffer_t* buffer,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    const uint8_t* pixels,
    uint32_t color
)
{
    if (buffer == NULL || buffer->pixels == NULL || pixels == NULL) {
        return;
    }

    for (uint16_t py = 0; py < height; ++py) {
        for (uint16_t px = 0; px < width; ++px) {
            const uint8_t alpha = pixels[(size_t)py * (size_t)width + (size_t)px];
            if (alpha != 0x00U) {
                gfx_buffer_put_pixel(
                    buffer,
                    (int)x + (int)px,
                    (int)y + (int)py,
                    color
                );
            }
        }
    }
}

// ***********************************************************************************************************
// Function name : gfx_buffer_draw_glyph                                                                     *
// Description   : Draws one glyph from a bitmap font into the target buffer.                                *
// ***********************************************************************************************************
void gfx_buffer_draw_glyph(
    gfx_buffer_t* buffer,
    uint16_t x,
    uint16_t y,
    char ch,
    uint32_t color,
    const gfx_font_t* font
)
{
    const gfx_font_t* active_font = (font != NULL) ? font : gfx_font_get_default();
    if (buffer == NULL || active_font == NULL || buffer->pixels == NULL) {
        return;
    }

    unsigned char glyph = (unsigned char)ch;
    if (glyph >= 'a' && glyph <= 'z') {
        glyph = (unsigned char)(glyph - 'a' + 'A');
    }
    if (glyph < active_font->first_char || glyph > active_font->last_char) {
        glyph = ' ';
    }

    const size_t glyph_index = (size_t)glyph - (size_t)active_font->first_char;
    const size_t glyph_offset = glyph_index * (size_t)active_font->width * (size_t)active_font->height;
    for (uint8_t col = 0U; col < active_font->width; ++col) {
        for (uint8_t row = 0U; row < active_font->height; ++row) {
            const size_t pixel_index = glyph_offset + ((size_t)col * (size_t)active_font->height) + (size_t)row;
            const uint8_t alpha = active_font->data[pixel_index];
            if (alpha != 0x00U) {
                gfx_buffer_put_pixel(buffer, (int)x + (int)col, (int)y + (int)row, color);
            }
        }
    }
}

// ***********************************************************************************************************
// Function name : gfx_buffer_draw_text                                                                       *
// Description   : Draws a text string into the target buffer using a bitmap font.                           *
// ***********************************************************************************************************
void gfx_buffer_draw_text(
    gfx_buffer_t* buffer,
    uint16_t x,
    uint16_t y,
    const char* text,
    uint32_t color,
    const gfx_font_t* font
)
{
    if (buffer == NULL || text == NULL) {
        return;
    }

    const gfx_font_t* active_font = (font != NULL) ? font : gfx_font_get_default();
    if (active_font == NULL) {
        return;
    }

    uint16_t cursor_x = x;
    for (const char* ch = text; *ch != '\0'; ++ch) {
        gfx_buffer_draw_glyph(buffer, cursor_x, y, *ch, color, active_font);
        cursor_x += (uint16_t)(active_font->width + active_font->spacing);
    }
}

// ***********************************************************************************************************
// Function name : gfx_engine_swap_buffers                                                                    *
// Description   : Swaps the front and back frame buffers for the next display pass.                         *
// ***********************************************************************************************************
static void gfx_engine_swap_buffers(gfx_engine_handle_t engine)
{
    if (engine == NULL) {
        return;
    }

    gfx_buffer_t temp = engine->front;
    engine->front = engine->back;
    engine->back = temp;
}

static void gfx_engine_destroy_internal(gfx_engine_handle_t handle)
{
    if (handle == NULL) {
        return;
    }

    gfx_tile_t* current = handle->tiles_head;
    while (current != NULL) {
        gfx_tile_t* next = current->next;
        gfx_tile_destroy(current);
        current = next;
    }

    gfx_engine_free_buffer(&handle->front);
    gfx_engine_free_buffer(&handle->back);
    free(handle);
}

static void gfx_engine_clear_internal(gfx_engine_handle_t handle, uint32_t color)
{
    if (handle == NULL) {
        return;
    }

    gfx_buffer_clear(&handle->back, color);
}

static int gfx_engine_add_tile_internal(gfx_engine_handle_t handle, gfx_tile_handle_t tile)
{
    if (handle == NULL || tile == NULL) {
        return -1;
    }

    tile->prev = handle->tiles_tail;
    tile->next = NULL;

    if (handle->tiles_tail != NULL) {
        handle->tiles_tail->next = tile;
    } else {
        handle->tiles_head = tile;
    }

    handle->tiles_tail = tile;
    handle->tile_count++;
    return 0;
}

static int gfx_engine_remove_tile_internal(gfx_engine_handle_t handle, gfx_tile_handle_t tile)
{
    if (handle == NULL || tile == NULL) {
        return -1;
    }

    gfx_tile_t* current = handle->tiles_head;
    while (current != NULL) {
        if (current == tile) {
            if (current->prev != NULL) {
                current->prev->next = current->next;
            } else {
                handle->tiles_head = current->next;
            }

            if (current->next != NULL) {
                current->next->prev = current->prev;
            } else {
                handle->tiles_tail = current->prev;
            }

            current->next = NULL;
            current->prev = NULL;
            handle->tile_count--;
            return 0;
        }
        current = current->next;
    }

    return -1;
}

static bool gfx_engine_render_internal(gfx_engine_handle_t handle)
{
    if (handle == NULL) {
        return false;
    }

    gfx_buffer_clear(&handle->back, 0u);

    gfx_tile_t* tile = handle->tiles_head;
    while (tile != NULL) {
        gfx_tile_draw(tile, &handle->back);
        tile = tile->next;
    }

    gfx_engine_swap_buffers(handle);
    return true;
}

static bool gfx_engine_flush_internal(gfx_engine_handle_t handle)
{
    if (handle == NULL) {
        return false;
    }

    if (handle->display.write_region == NULL) {
        return false;
    }

    int result = handle->display.write_region(
        handle->display.user_data,
        0,
        0,
        handle->width,
        handle->height,
        handle->front.pixels,
        handle->front.pixel_format
    );
    if (result != 0) {
        return false;
    }

    if (handle->display.flush != NULL) {
        return handle->display.flush(handle->display.user_data) == 0;
    }

    return true;
}

// ***********************************************************************************************************
// Function name : gfx_engine_init                                                                           *
// Description   : Creates a graphics engine instance and initializes the frame buffers and callbacks.       *
// ***********************************************************************************************************
gfx_engine_handle_t gfx_engine_init(gfx_engine_config_t* config)
{
    if (config == NULL) {
        return NULL;
    }

    gfx_engine_handle_t handle = (gfx_engine_handle_t)calloc(1u, sizeof(gfx_engine_t));
    if (handle == NULL) {
        return NULL;
    }

    handle->width = config->width;
    handle->height = config->height;
    handle->tiles_head = NULL;
    handle->tiles_tail = NULL;
    handle->tile_count = 0u;
    handle->display = (config->display != NULL) ? *config->display : (gfx_display_driver_t){0};

    if (handle->display.pixel_format == gfx_pixel_format_1bpp ||
        handle->display.pixel_format == gfx_pixel_format_8bpp ||
        handle->display.pixel_format == gfx_pixel_format_rgb565 ||
        handle->display.pixel_format == gfx_pixel_format_rgb888) {
        handle->front.pixel_format = handle->display.pixel_format;
        handle->back.pixel_format = handle->display.pixel_format;
    } else {
        handle->front.pixel_format = config->pixel_format;
        handle->back.pixel_format = config->pixel_format;
    }

    gfx_engine_initialize_buffer(&handle->front, config->width, config->height, handle->front.pixel_format);
    gfx_engine_initialize_buffer(&handle->back, config->width, config->height, handle->back.pixel_format);
    if (handle->front.pixels == NULL || handle->back.pixels == NULL) {
        gfx_engine_free_buffer(&handle->front);
        gfx_engine_free_buffer(&handle->back);
        free(handle);
        return NULL;
    }

    if (handle->display.init != NULL) {
        handle->display.init(handle->display.user_data);
    }

    return handle;
}

// ***********************************************************************************************************
// Function name : gfx_engine_destroy                                                                       *
// Description   : Releases all resources owned by the graphics engine.                                       *
// ***********************************************************************************************************
void gfx_engine_destroy(gfx_engine_handle_t handle)
{
    if (handle == NULL) {
        return;
    }

    gfx_engine_destroy_internal(handle);
}

// ***********************************************************************************************************
// Function name : gfx_engine_clear                                                                         *
// Description   : Fills the current back buffer with a solid color.                                         *
// ***********************************************************************************************************
void gfx_engine_clear(gfx_engine_handle_t handle, uint32_t color)
{
    if (handle == NULL) {
        return;
    }

    gfx_engine_clear_internal(handle, color);
}

// ***********************************************************************************************************
// Function name : gfx_engine_add_tile                                                                      *
// Description   : Adds a tile to the engine render list so it is included in the next render pass.          *
// ***********************************************************************************************************
int gfx_engine_add_tile(gfx_engine_handle_t handle, gfx_tile_handle_t tile)
{
    return gfx_engine_add_tile_internal(handle, tile);
}

// ***********************************************************************************************************
// Function name : gfx_engine_remove_tile                                                                   *
// Description   : Removes a tile from the engine render list.                                               *
// ***********************************************************************************************************
int gfx_engine_remove_tile(gfx_engine_handle_t handle, gfx_tile_handle_t tile)
{
    return gfx_engine_remove_tile_internal(handle, tile);
}

// ***********************************************************************************************************
// Function name : gfx_engine_render                                                                        *
// Description   : Renders all visible tiles into the back buffer and swaps the frame buffers.               *
// ***********************************************************************************************************
bool gfx_engine_render(gfx_engine_handle_t handle)
{
    if (handle == NULL) {
        return false;
    }

    return gfx_engine_render_internal(handle);
}

// ***********************************************************************************************************
// Function name : gfx_engine_flush                                                                         *
// Description   : Sends the current front buffer contents to the display driver.                            *
// ***********************************************************************************************************
bool gfx_engine_flush(gfx_engine_handle_t handle)
{
    if (handle == NULL) {
        return false;
    }

    return gfx_engine_flush_internal(handle);
}


