#include "lcd_spi_display.h"

#include <stdlib.h>
#include <string.h>

#define APP_LCD_SPI_MODE_COMMAND 0U
#define APP_LCD_SPI_MODE_DATA 1U

typedef struct app_lcd_spi_handle_s {
    app_lcd_spi_config_t config;
    app_lcd_spi_set_chip_select set_chip_select;
    app_lcd_spi_set_data_mode set_data_mode;
    app_lcd_spi_write write;
    app_lcd_spi_delay delay;
} app_lcd_spi_handle_s;

static void app_lcd_spi_set_data_mode_internal(app_lcd_spi_handle_t* handle, bool data_mode)
{
    if (handle == NULL) {
        return;
    }

    if (handle->set_data_mode != NULL) {
        handle->set_data_mode(data_mode);
    }
}

static void app_lcd_spi_set_chip_select_internal(app_lcd_spi_handle_t* handle, bool selected)
{
    if (handle == NULL) {
        return;
    }

    if (handle->set_chip_select != NULL) {
        handle->set_chip_select(selected);
    }
}

static void app_lcd_spi_write_transfer(app_lcd_spi_handle_t* handle, const uint8_t* data, size_t length)
{
    if (handle == NULL || data == NULL || length == 0U) {
        return;
    }

    if (handle->write != NULL) {
        handle->write(data, length);
    }
}

static void app_lcd_spi_write_byte(app_lcd_spi_handle_t* handle, uint8_t value)
{
    if (handle == NULL) {
        return;
    }

    app_lcd_spi_set_chip_select_internal(handle, true);
    app_lcd_spi_write_transfer(handle, &value, 1U);
    app_lcd_spi_set_chip_select_internal(handle, false);
}

static void app_lcd_spi_write_dummy(app_lcd_spi_handle_t* handle)
{
    if (handle == NULL) {
        return;
    }

    for (uint8_t i = 0U; i < handle->config.dummy_cycles; ++i) {
        app_lcd_spi_write_byte(handle, 0x00U);
    }
}

static void app_lcd_spi_write_address(app_lcd_spi_handle_t* handle, uint16_t value)
{
    if (handle == NULL) {
        return;
    }

    app_lcd_spi_write_byte(handle, (uint8_t)((value >> 8U) & 0xFFU));
    app_lcd_spi_write_byte(handle, (uint8_t)(value & 0xFFU));
}

static void app_lcd_spi_send_command(app_lcd_spi_handle_t* handle, uint8_t command)
{
    if (handle == NULL) {
        return;
    }

    app_lcd_spi_set_data_mode_internal(handle, false);
    app_lcd_spi_write_byte(handle, command);
}

static void app_lcd_spi_send_data(app_lcd_spi_handle_t* handle, const uint8_t* data, uint16_t length)
{
    if (handle == NULL || data == NULL || length == 0U) {
        return;
    }

    app_lcd_spi_set_data_mode_internal(handle, true);
    app_lcd_spi_set_chip_select_internal(handle, true);
    app_lcd_spi_write_transfer(handle, data, (size_t)length);
    app_lcd_spi_set_chip_select_internal(handle, false);
}

static void app_lcd_spi_set_window(
    app_lcd_spi_handle_t* handle,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height
)
{
    if (handle == NULL) {
        return;
    }

    app_lcd_spi_send_command(handle, handle->config.row_address_cmd);
    app_lcd_spi_write_dummy(handle);
    app_lcd_spi_write_address(handle, y);

    app_lcd_spi_send_command(handle, handle->config.col_address_cmd);
    app_lcd_spi_write_dummy(handle);
    app_lcd_spi_write_address(handle, x);

    (void)width;
    (void)height;
}

static size_t app_lcd_spi_bytes_per_pixel(gfx_pixel_format_t pixel_format)
{
    switch (pixel_format) {
        case gfx_pixel_format_1bpp:
            return 1U;
        case gfx_pixel_format_8bpp:
            return 1U;
        case gfx_pixel_format_rgb565:
            return 2U;
        case gfx_pixel_format_rgb888:
            return 3U;
        default:
            return 2U;
    }
}

static void app_lcd_spi_convert_1bpp_to_byte_packed(
    const uint8_t* source,
    uint8_t* output,
    uint16_t width,
    uint16_t height
)
{
    if (source == NULL || output == NULL || width == 0U || height == 0U) {
        return;
    }

    const size_t pixel_count = (size_t)width * (size_t)height;
    const size_t output_size = (pixel_count + 7U) / 8U;
    memset(output, 0x00U, output_size);

    for (size_t i = 0U; i < pixel_count; ++i) {
        const size_t bit_index = i % 8U;
        const size_t byte_index = i / 8U;
        const uint8_t value = source[i] ? 1U : 0U;
        if (value != 0U) {
            output[byte_index] |= (uint8_t)(1U << (7U - bit_index));
        }
    }
}

// ***********************************************************************************************************
// Function name : app_lcd_spi_init                                                                         *
// Description   : Initialize the Sharp display driver handle with the bus callbacks and display config.     *
// Argument      : (app_lcd_spi_config_t*) i_p_config: Display configuration.                                *
//                : (app_lcd_spi_set_chip_select) i_set_chip_select: Chip-select callback.                   *
//                : (app_lcd_spi_set_data_mode) i_set_data_mode: Data/command callback.                       *
//                : (app_lcd_spi_write) i_write: Byte transfer callback.                                      *
//                : (app_lcd_spi_delay) i_delay: Delay callback.                                              *
// Return value  : (app_lcd_spi_handle_t*) : Pointer to the created display handle or NULL on failure.      *
// ***********************************************************************************************************
app_lcd_spi_handle_t* app_lcd_spi_init(
    app_lcd_spi_config_t* i_p_config,
    app_lcd_spi_set_chip_select i_set_chip_select,
    app_lcd_spi_set_data_mode i_set_data_mode,
    app_lcd_spi_write i_write,
    app_lcd_spi_delay i_delay
)
{
    app_lcd_spi_handle_t* handle = (app_lcd_spi_handle_t*)calloc(1u, sizeof(app_lcd_spi_handle_t));
    if (handle == NULL || i_p_config == NULL) {
        if (handle != NULL) {
            free(handle);
        }
        return NULL;
    }

    handle->config = *i_p_config;
    handle->set_chip_select = i_set_chip_select;
    handle->set_data_mode = i_set_data_mode;
    handle->write = i_write;
    handle->delay = i_delay;

    if (handle->config.dummy_cycles == 0U) {
        handle->config.dummy_cycles = 1U;
    }

    if (handle->config.row_address_cmd == 0U) {
        handle->config.row_address_cmd = 0x2AU;
    }

    if (handle->config.col_address_cmd == 0U) {
        handle->config.col_address_cmd = 0x2BU;
    }

    return handle;
}

// ***********************************************************************************************************
// Function name : app_lcd_spi_write_region                                                                *
// Description   : Sends one display region using the application-specific SPI pattern.                     *
// Argument      : (void*) user_data: Driver handle passed by the engine.                                    *
//                : (uint16_t) x : Region start X coordinate.                                                  *
//                : (uint16_t) y : Region start Y coordinate.                                                  *
//                : (uint16_t) width : Region width in pixels.                                                  *
//                : (uint16_t) height : Region height in pixels.                                                *
//                : (const void*) pixels : Pixel array in the engine pixel format.                              *
//                : (gfx_pixel_format_t) pixel_format : Pixel format used by the source buffer.                   *
// Return value  : (int) : 0 on success, -1 on invalid input.                                               *
// ***********************************************************************************************************
static int app_lcd_spi_write_region(
    void* user_data,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    const void* pixels,
    gfx_pixel_format_t pixel_format
)
{
    app_lcd_spi_handle_t* handle = (app_lcd_spi_handle_t*)user_data;
    if (handle == NULL || pixels == NULL) {
        return -1;
    }

    const uint8_t* source = (const uint8_t*)pixels;
    const size_t bytes_per_pixel = app_lcd_spi_bytes_per_pixel(pixel_format);
    if (bytes_per_pixel == 0U) {
        return -1;
    }

    uint8_t packed_line[512];
    const size_t packed_line_size = (size_t)width * bytes_per_pixel;
    if (packed_line_size > sizeof(packed_line)) {
        return -1;
    }

    for (uint16_t row = 0U; row < height; ++row) {
        const uint16_t current_y = (uint16_t)(y + row);
        const uint16_t current_x = x;

        app_lcd_spi_set_window(handle, current_x, current_y, width, 1U);
        app_lcd_spi_send_command(handle, 0x2CU);
        app_lcd_spi_write_dummy(handle);

        if (pixel_format == gfx_pixel_format_1bpp) {
            app_lcd_spi_convert_1bpp_to_byte_packed(
                &source[(size_t)row * (size_t)width],
                packed_line,
                width,
                1U
            );
            app_lcd_spi_send_data(handle, packed_line, (uint16_t)((size_t)width + 7U) / 8U);
        } else {
            const size_t row_offset = (size_t)row * (size_t)width * bytes_per_pixel;
            app_lcd_spi_send_data(handle, &source[row_offset], (uint16_t)((size_t)width * bytes_per_pixel));
        }
    }

    return 0;
}

static int app_lcd_spi_flush(void* user_data)
{
    (void)user_data;
    return 0;
}

// ***********************************************************************************************************
// Function name : app_lcd_spi_bind_driver                                                                  *
// Description   : Binds the application SPI display implementation to the generic engine API.              *
// Argument      : (app_lcd_spi_handle_t*) i_p_handle: Display handle.                                      *
//                : (gfx_display_driver_t*) o_p_driver: Generic display driver object to fill.                 *
// Return value  : (int) : 0 on success, -1 on invalid input.                                               *
// ***********************************************************************************************************
int app_lcd_spi_bind_driver(app_lcd_spi_handle_t* i_p_handle, gfx_display_driver_t* o_p_driver)
{
    if (i_p_handle == NULL || o_p_driver == NULL) {
        return -1;
    }

    memset(o_p_driver, 0, sizeof(*o_p_driver));
    o_p_driver->user_data = i_p_handle;
    o_p_driver->pixel_format = i_p_handle->config.pixel_format;
    o_p_driver->init = NULL;
    o_p_driver->write_region = app_lcd_spi_write_region;
    o_p_driver->flush = app_lcd_spi_flush;
    return 0;
}
