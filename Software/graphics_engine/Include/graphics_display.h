#ifndef GRAPHICS_DISPLAY_H_
#define GRAPHICS_DISPLAY_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum gfx_pixel_format_t {
    gfx_pixel_format_1bpp = 0,
    gfx_pixel_format_8bpp = 1,
    gfx_pixel_format_rgb565 = 2,
    gfx_pixel_format_rgb888 = 3
} gfx_pixel_format_t;

// ***********************************************************************************************************
// Function name : gfx_display_init_fn                                                                       *
// Description   : Initializes the display backend and board-specific hardware for the panel.                *
// Input         : (void*) user_data : Driver-specific state passed by the graphics engine.                  *
// Return        : (int) : 0 on success, negative value on failure.                                          *
// ***********************************************************************************************************
typedef int (*gfx_display_init_fn)(void* user_data);

// ***********************************************************************************************************
// Function name : gfx_display_write_region_fn                                                               *
// Description   : Sends one rectangular region of pixel data to the display.                                *
// Input         : (void*) user_data : Driver-specific state passed by the graphics engine.                  *
//                 (uint16_t) x : Start X coordinate of the region.                                          *
//                 (uint16_t) y : Start Y coordinate of the region.                                          *
//                 (uint16_t) width : Width of the region in pixels.                                         *
//                 (uint16_t) height : Height of the region in pixels.                                       *
//                 (const void*) pixels : Pixel data to send.                                                *
//                 (gfx_pixel_format_t) pixel_format : Pixel format of the source buffer.                    *
// Return        : (int) : 0 on success, negative value on failure.                                          *
// ***********************************************************************************************************
typedef int (*gfx_display_write_region_fn)(
    void* user_data,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    const void* pixels,
    gfx_pixel_format_t pixel_format
);

// ***********************************************************************************************************
// Function name : gfx_display_flush_fn                                                                      *
// Description   : Finalizes the current display transfer and triggers the panel refresh.                    *
// Input         : (void*) user_data : Driver-specific state passed by the graphics engine.                  *
// Return        : (int) : 0 on success, negative value on failure.                                          *
// ***********************************************************************************************************
typedef int (*gfx_display_flush_fn)(void* user_data);

// Structure for the display driver callbacks and configuration
typedef struct gfx_display_driver_t {
    void* user_data;
    gfx_pixel_format_t pixel_format;
    gfx_display_init_fn init;
    gfx_display_write_region_fn write_region;
    gfx_display_flush_fn flush;
} gfx_display_driver_t;

#ifdef __cplusplus
}
#endif

#endif
