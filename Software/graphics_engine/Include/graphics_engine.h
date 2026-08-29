#ifndef GRAPHICS_ENGINE_H_
#define GRAPHICS_ENGINE_H_

#include <stdbool.h>
#include <stdint.h>

#include "graphics_display.h"
#include "graphics_tile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gfx_engine_s gfx_engine_t;
typedef gfx_engine_t* gfx_engine_handle_t;

typedef struct gfx_engine_config_s {
    uint16_t width;
    uint16_t height;
    gfx_pixel_format_t pixel_format;
    const gfx_display_driver_t* display;
} gfx_engine_config_t;

// ***********************************************************************************************************
// Function name : gfx_engine_init                                                                           *
// Description   : Creates a graphics engine instance and initializes the frame buffers and callbacks.       *
// Input         : (gfx_engine_config_t*) config : Engine configuration to apply.                            *
// Return        : (gfx_engine_handle_t) : Engine handle or NULL on failure.                                 *
// ***********************************************************************************************************
gfx_engine_handle_t gfx_engine_init(gfx_engine_config_t* config);

// ***********************************************************************************************************
// Function name : gfx_engine_destroy                                                                        *
// Description   : Releases all resources owned by the graphics engine.                                      *
// Input         : (gfx_engine_handle_t) handle : Engine instance to destroy.                                *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_engine_destroy(gfx_engine_handle_t handle);

// ***********************************************************************************************************
// Function name : gfx_engine_clear                                                                          *
// Description   : Fills the current back buffer with a solid color.                                         *
// Input         : (gfx_engine_handle_t) handle : Engine instance to update.                                 *
//                 (uint32_t) color : Color value used to clear the buffer.                                  *
// Return        : (void) : No return value.                                                                 *
// ***********************************************************************************************************
void gfx_engine_clear(gfx_engine_handle_t handle, uint32_t color);

// ***********************************************************************************************************
// Function name : gfx_engine_add_tile                                                                       *
// Description   : Adds a tile to the engine render list so it is included in the next render pass.          *
// Input         : (gfx_engine_handle_t) handle : Engine instance receiving the tile.                        *
//                 (gfx_tile_handle_t) tile : Tile to add to the render list.                                   *
// Return        : (int) : 0 on success, -1 on invalid input.                                                *
// ***********************************************************************************************************
int gfx_engine_add_tile(gfx_engine_handle_t handle, gfx_tile_handle_t tile);

// ***********************************************************************************************************
// Function name : gfx_engine_remove_tile                                                                    *
// Description   : Removes a tile from the engine render list.                                               *
// Input         : (gfx_engine_handle_t) handle : Engine instance containing the tile.                       *
//                 (gfx_tile_handle_t) tile : Tile to remove from the list.                                     *
// Return        : (int) : 0 on success, -1 if the tile is not found or the input is invalid.                *
// ***********************************************************************************************************
int gfx_engine_remove_tile(gfx_engine_handle_t handle, gfx_tile_handle_t tile);

// ***********************************************************************************************************
// Function name : gfx_engine_render                                                                         *
// Description   : Renders all visible tiles into the back buffer and swaps the frame buffers.               *
// Input         : (gfx_engine_handle_t) handle : Engine instance to render.                                 *
// Return        : (bool) : true when rendering succeeds, false otherwise.                                   *
// ***********************************************************************************************************
bool gfx_engine_render(gfx_engine_handle_t handle);

// ***********************************************************************************************************
// Function name : gfx_engine_flush                                                                          *
// Description   : Sends the current front buffer data to the display driver.                                *
// Input         : (gfx_engine_handle_t) handle : Engine instance to flush.                                  *
// Return        : (bool) : true when the display flush succeeds, false otherwise.                           *
// ***********************************************************************************************************
bool gfx_engine_flush(gfx_engine_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif
