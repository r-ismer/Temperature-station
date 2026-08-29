#ifndef LCD_SPI_DISPLAY_H_
#define LCD_SPI_DISPLAY_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "graphics_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

// ***********************************************************************************************************
// Structure name : app_lcd_spi_handle_s                                                                     *
// Description   : Handle for the application-level Sharp display driver.                                    *
// ***********************************************************************************************************
typedef struct app_lcd_spi_handle_s app_lcd_spi_handle_t;

// ***********************************************************************************************************
// Structure name : app_lcd_spi_config_s                                                                     *
// Description   : Configuration used by the application-level display driver.                               *
// ***********************************************************************************************************
typedef struct app_lcd_spi_config_s {
    uint16_t width;
    uint16_t height;
    uint8_t dummy_cycles;
    uint8_t row_address_cmd;
    uint8_t col_address_cmd;
    gfx_pixel_format_t pixel_format;
} app_lcd_spi_config_t;

// ***********************************************************************************************************
// Function name : app_lcd_spi_set_chip_select                                                               *
// Description   : Set the chip-select line state for the display.                                           *
// Argument      : (bool) i_selected: true when selected, false when released.                               *
// Return value  : None                                                                                      *
// ***********************************************************************************************************
typedef void (*app_lcd_spi_set_chip_select)(bool i_selected);

// ***********************************************************************************************************
// Function name : app_lcd_spi_set_data_mode                                                                 *
// Description   : Select command or data mode on the display control line.                                  *
// Argument      : (bool) i_data_mode: true for data, false for command.                                     *
// Return value  : None                                                                                      *
// ***********************************************************************************************************
typedef void (*app_lcd_spi_set_data_mode)(bool i_data_mode);

// ***********************************************************************************************************
// Function name : app_lcd_spi_write                                                                         *
// Description   : Write an array of bytes to the display bus.                                               *
// Argument      : (const uint8_t*) i_p_data: Pointer to data to send.                                       *
//                : (size_t) i_size: Number of bytes to send.                                                *
// Return value  : None                                                                                      *
// ***********************************************************************************************************
typedef void (*app_lcd_spi_write)(const uint8_t* i_p_data, size_t i_size);

// ***********************************************************************************************************
// Function name : app_lcd_spi_delay                                                                         *
// Description   : Delay for the requested time in microseconds.                                             *
// Argument      : (uint32_t) i_delay_us: Delay in microseconds.                                             *
// Return value  : None                                                                                      *
// ***********************************************************************************************************
typedef void (*app_lcd_spi_delay)(uint32_t i_delay_us);

// ***********************************************************************************************************
// Function name : app_lcd_spi_init                                                                          *
// Description   : Initialize the Sharp display driver handle with the bus callbacks and display config.     *
// Argument      : (app_lcd_spi_config_t*) i_p_config: Display configuration.                                *
//                : (app_lcd_spi_set_chip_select) i_set_chip_select: Chip-select callback.                   *
//                : (app_lcd_spi_set_data_mode) i_set_data_mode: Data/command callback.                      *
//                : (app_lcd_spi_write) i_write: Byte transfer callback.                                     *
//                : (app_lcd_spi_delay) i_delay: Delay callback.                                             *
// Return value  : (app_lcd_spi_handle_t*) : Pointer to the created display handle or NULL on failure.       *
// ***********************************************************************************************************
app_lcd_spi_handle_t* app_lcd_spi_init(
    app_lcd_spi_config_t* i_p_config,
    app_lcd_spi_set_chip_select i_set_chip_select,
    app_lcd_spi_set_data_mode i_set_data_mode,
    app_lcd_spi_write i_write,
    app_lcd_spi_delay i_delay
);

// ***********************************************************************************************************
// Function name : app_lcd_spi_bind_driver                                                                  *
// Description   : Binds the application SPI display implementation to the generic engine API.              *
// Argument      : (app_lcd_spi_handle_t*) i_p_handle: Display handle.                                      *
//                : (gfx_display_driver_t*) o_p_driver: Generic display driver object to fill.                 *
// Return value  : (int) : 0 on success, -1 on invalid input.                                               *
// ***********************************************************************************************************
int app_lcd_spi_bind_driver(app_lcd_spi_handle_t* i_p_handle, gfx_display_driver_t* o_p_driver);

#ifdef __cplusplus
}
#endif

#endif
