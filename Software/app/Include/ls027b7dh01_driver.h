// **********************************************************************************************************
// File name        : ls027b7dh01_driver.h                                                                  *
// Author           : Richard I.                                                                            *
// Date             : 27/09/2026                                                                            *
// Description      : Driver for the Sharp LS027B7DH01 display module.                                      *
// **********************************************************************************************************
#ifndef _LS027B7DH01_DRIVER_H_
#define _LS027B7DH01_DRIVER_H_

// **********************************************************************************************************
//                                               Include                                                    *
// **********************************************************************************************************
#include "definitions.h"

// **********************************************************************************************************
//                                               Defines                                                    *
// **********************************************************************************************************
// Strucure forware declaration
typedef struct ls027b7dh01_handle_s ls027b7dh01_handle_t;

// VCOM mode
typedef enum
{
    VCOM_MODE_EXTERNAL = 0,
    VCOM_MODE_INTERNAL,
    VCOM_MODE_FRAME,
} ls027b7dh01_vcom_mode_e;

// **********************************************************************************************************
// Function     : ls027b7dh01_send_data                                                                     *
// Description  : Send data to the LS027B7SH01 display                                                      *
// Input        : (uint16_t) i_length   : The length of the data in bytes                                   *
//              : (uint8_t*) i_p_data   : The data to send                                                  *
// Return       : (status_e)    : The status of the operation                                               *
// **********************************************************************************************************
typedef status_e (*ls027b7dh01_send_data)(uint16_t i_length, const uint8_t* i_p_data);

// **********************************************************************************************************
// Function     : ls027b7dh01_toggle_vcom                                                                   *
// Description  : Toggle the VCOM line (only needed if the MODE is internal)                                *
// Input        : None                                                                                      *
// Return       : (status_e)    : The status of the operation                                               *
// **********************************************************************************************************
typedef status_e (*ls027b7dh01_toggle_vcom)(void);

// **********************************************************************************************************
//                                           Public fuctions                                                *
// **********************************************************************************************************
// **********************************************************************************************************
// Function     : ls027b7dh01_init                                                                          *
// Description  : Initialize the driver for the LS027B7DH01 display                                         *
// Input        : (ls027b7dh01_vcom_mode_e) i_vcom_mode : The mode for the VCOM                             *
//              : (ls027b7dh01_send_data) i_send_data   : Function to send the data to the display          *
//              : (ls027b7dh01_toggle_vcom) i_toggle_vcom   : Function to toggle the VCOM                   *
//              : (uint8_t*) i_framebuffer_1            : The first framebuffer (at least 14000 bytes)      *
//              : (uint8_t*) i_framebuffer_2            : The second frambuffer (at least 14000 bytes)      *
// Return       : (ls027b7dh01_handle_t*)   : A pointer on the handle newly created                         *
// **********************************************************************************************************
ls027b7dh01_handle_t* ls027b7dh01_init(ls027b7dh01_vcom_mode_e i_mode, 
                                       ls027b7dh01_send_data i_send_data,
                                       ls027b7dh01_toggle_vcom i_toggle_vcom,
                                       uint8_t* i_framebuffer_1, 
                                       uint8_t* i_framebuffer_2); 

// **********************************************************************************************************
// Function     : ls027b7dh01_deinit                                                                        *
// Description  : Deinitialize the driver for the LS027B7SH01 display                                       *
// Input        : (ls027b7dh01_handle_t*) i_p_handle    : The pointer on the handle                         *
// Return       : (status_e)    : The status of the operation                                               *
// **********************************************************************************************************
status_e ls027b7dh01_deinit(ls027b7dh01_handle_t* i_p_handle);

// **********************************************************************************************************
// Function     : ls027b7dh01_new_frame_available                                                           *
// Description  : Call this to update the frame on the display                                              *
// Input        : (ls027b7dh01_handle_t*) i_p_handle    : The pointer on the handle                         *
//              : (uint8_t) i_buffer_index              : The index of the buffer to send (either 0 or 1)   *
// Return       : (status_e)    : The status of the operation                                               *
// **********************************************************************************************************
status_e ls027b7dh01_new_frame_available(ls027b7dh01_handle_t* i_p_handle, uint8_t i_buffer_index);

#endif // _LS027B7DH01_DRIVER_H_