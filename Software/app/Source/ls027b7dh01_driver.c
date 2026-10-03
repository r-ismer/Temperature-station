// **********************************************************************************************************
// File name        : ls027b7dh01_driver.h                                                                  *
// Author           : Richard I.                                                                            *
// Date             : 27/09/2026                                                                            *
// Description      : Driver for the Sharp LS027B7DH01 display module.                                      *
// **********************************************************************************************************
// **********************************************************************************************************
//                                               Include                                                    *
// **********************************************************************************************************
#include "ls027b7dh01_driver.h"
#include "cmsis_os.h"

// **********************************************************************************************************
//                                               Defines                                                    *
// **********************************************************************************************************
// Display dimension
#define LS027B7DH01_HIGHT                       (240u)
#define LS027B7DH01_WIDTH                       (400u)

// Mode bits definition
#define LS027B7DH01_MODE_0_UPDATE               (1u << 7u)
#define LS027B7DH01_MODE_0_DISPLAY              (0u << 7u)
#define LS027B7DH01_MODE_1_INVERSION_FLAG       (1u << 6u)
#define LS027B7DH01_MODE_2_ALL_CLEAR_FLAG       (1u << 5u)

// Index to address (add 1 and reverse the order of the bits)
#define LS027B7DH01_INDEX_TO_ADDRESS(index)     (0xFF - (index) + 1u)

// Object definition
typedef struct
{
    // Mode
    ls027b7dh01_vcom_mode_e mode;
    bool_e toggle;

    // Functions
    ls027b7dh01_send_data send_data;
    ls027b7dh01_toggle_vcom toggle_vcom;

    // Framebuffers
    uint8_t* framebuffer_1;
    uint8_t* framebuffer_2;
} ls027b7dh01_handle_s;

// Toggling of the VCOM line
#define LS027B7DH01_VCOM_FREQUENCY_HS           (5u)
#define LS027B7DH01_VCOM_FREQUENCY_MS           (1000u / LS027B7DH01_VCOM_FREQUENCY_HS)

// Task attributes
static const osThreadAttr_t LS027B7DH01_VCOM_TOGGLE_TASK_ATTR = {
    .name = "ls027b7dh01_vcom_toggle_task",
    .stack_size = 256u,
    .priority = (osPriority_t) osPriorityNormal,
};

// **********************************************************************************************************
//                                     Private fuctions prototype                                           *
// **********************************************************************************************************
// **********************************************************************************************************
// Function     : ls027b7dh01_vcom_toggle_task                                                              *
// Description  : Task to toggle the VCOM line (only needed if the MODE is internal)                        *
// Input        : (void*) pvParameters : The parameters for the task                                        *
// Return       : None                                                                                      *
// **********************************************************************************************************
static void ls027b7dh01_vcom_toggle_task(void* pvParameters);

// **********************************************************************************************************
// Function     : ls027b7dh01_reverse_bits                                                                  *
// Description  : Reverse the bits in a byte                                                                *
// Input        : (uint8_t) i_byte : The byte to reverse                                                    *
// Return       : (uint8_t) : The reversed byte                                                             *
// **********************************************************************************************************   
static uint8_t ls027b7dh01_reverse_bits(uint8_t i_byte);

// **********************************************************************************************************
//                                           Public fuctions                                                *
// **********************************************************************************************************
// **********************************************************************************************************
// Function     : ls027b7dh01_init                                                                          *
// Description  : Initialize the driver for the LS027B7DH01 display                                         *
// **********************************************************************************************************
ls027b7dh01_handle_t* ls027b7dh01_init(ls027b7dh01_vcom_mode_e i_mode, 
                                       ls027b7dh01_send_data i_send_data,
                                       ls027b7dh01_toggle_vcom i_toggle_vcom,
                                       uint8_t* i_framebuffer_1, 
                                       uint8_t* i_framebuffer_2)
{
    // Variable(s) declaration
    ls027b7dh01_handle_s* r_p_handle;

    // Allocate the handle
    r_p_handle = (ls027b7dh01_handle_s*) malloc(sizeof(ls027b7dh01_handle_s));

    // Check the handle
    if (r_p_handle != NULL)
    {
        // Set the mode
        r_p_handle->mode = i_mode;
        toggle = TRUE;

        // Set the functions
        r_p_handle->send_data = i_send_data;
        r_p_handle->toggle_vcom = i_toggle_vcom;

        // Set the framebuffers
        r_p_handle->framebuffer_1 = i_framebuffer_1;
        r_p_handle->framebuffer_2 = i_framebuffer_2;

        // Check the mode to see if we need to start the toggling of the VCOM line
        if (r_p_handle->mode == VCOM_MODE_INTERNAL)
        {
            // Create the task for the toggling of the VCOM line
            osThreadNew(ls027b7dh01_vcom_toggle_task, (void*) r_p_handle, LS027B7DH01_VCOM_TOGGLE_TASK_ATTR);
        }
    } 
    else
    {
        // Set the status
        r_p_handle = NULL;
    }

    // Return the handle
    return (ls027b7dh01_handle_t*) r_p_handle;
}

// **********************************************************************************************************
// Function     : ls027b7dh01_deinit                                                                        *
// Description  : Deinitialize the driver for the LS027B7SH01 display                                       *
// **********************************************************************************************************
status_e ls027b7dh01_deinit(ls027b7dh01_handle_t* i_p_handle)
{
    // Variable(s) declaration
    status_e r_status;

    // Check the handle
    if (i_p_handle != NULL)
    {
        // Check the mode to see if we need to stop the toggling of the VCOM line
        if (i_p_handle->mode == VCOM_MODE_INTERNAL)
        {
            // Stop the toggling of the VCOM line
            // TODO
        }

        // Free the handle
        free(i_p_handle);

        // Set the status
        r_status = STATUS_OK;
    }
    else
    {
        // Set the status
        r_status = STATUS_ERROR;
    }
}

// **********************************************************************************************************
// Function     : ls027b7dh01_new_frame_available                                                           *
// Description  : Call this to update the frame on the display                                              *
// **********************************************************************************************************
status_e ls027b7dh01_new_frame_available(ls027b7dh01_handle_t* i_p_handle, uint8_t i_buffer_index)
{
    // Variable(s) declaration
    status_e r_status;
    ls027b7dh01_handle_s* p_handle;
    uint8_t buffer[402u];
    uint8_t index;
    uint8_t* p_framebuffer;

    // Retrieve the handle
    p_handle = (ls027b7dh01_handle_s*) i_p_handle;

    // Check the handle
    if (p_handle != NULL)
    {
        // Check the buffer index
        if (i_buffer_index == 0u || i_buffer_index == 1u)
        {
            // Retrieve the framebuffer
            if (i_buffer_index == 0u)
            {
                // Set the framebuffer to the first one
                p_framebuffer = p_handle->framebuffer_1;
            }
            else
            {
                // Set the framebuffer to the second one
                p_framebuffer = p_handle->framebuffer_2;
            }

            // Iterate over every line in the framebuffer and send them
            for (index = 0u ; index < LS027B7DH01_HIGHT && r_status == STATUS_OK ; index++)
            {
                // Reset the first byte of the message
                buffer[0u] = 0u;

                // Check if this the first line
                if (index == 0u)
                {
                    // Set the mode bits in the first byte of the buffer
                    buffer[0u] = LS027B7DH01_MODE_0_UPDATE;

                    // Check the toggle bit
                    if (p_handle->mode == VCOM_MODE_FRAME && p_handle->toggle == TRUE)
                    {
                        // Set the toggle bit in the first byte of the buffer
                        buffer[0u] |= LS027B7DH01_MODE_1_INVERSION_FLAG; 
                    }

                    // Set the toggle bit
                    p_handle->toggle ^= TRUE;
                }

                // Set the address in the second byte of the buffer
                buffer[1u] = ls027b7dh01_reverse_bits(index);

                // Copy the framebuffer into the buffer
                memcpy(buffer + 2u, p_framebuffer + index * LS027B7DH01_WIDTH, LS027B7DH01_WIDTH);

                // Send the data to the display
                r_status = p_handle->send_data(LS027B7DH01_WIDTH + 2u, buffer);
            }
        }
        else
        {
            // Set the status
            r_status = STATUS_ERROR;
        }
    }
    else
    {
        // Set the status
        r_status = STATUS_ERROR;
    }

    // Return the status of the operation
    return r_status;
}

// **********************************************************************************************************
//                                          Private fuctions                                                *
// **********************************************************************************************************
// **********************************************************************************************************
// Function     : ls027b7dh01_vcom_toggle_task                                                              *
// Description  : Task to toggle the VCOM line (only needed if the MODE is internal)                        *
// **********************************************************************************************************
static void ls027b7dh01_vcom_toggle_task(void* pvParameters)
{
    // Parameter(s) declaration
    ls027b7dh01_handle_s* p_handle;
    uint32_t tick;

    // Retrieve the handle
    p_handle = (ls027b7dh01_handle_s*) pvParameters;

    // Get the current tick
    tick = osKernelGetTickCount();
    
    // Infinite loop
    while (1)
    {
        // Increment the tick 
        tick += LS027B7DH01_VCOM_FREQUENCY_MS;

        // Wait until the next tick
        osDelayUntil(tick);

        // Toggle the VCOM line
        p_handle->toggle_vcom();
    }
}

// **********************************************************************************************************
// Function     : ls027b7dh01_reverse_bits                                                                  *
// Description  : Reverse the bits in a byte                                                                *
// **********************************************************************************************************   
static uint8_t ls027b7dh01_reverse_bits(uint8_t i_byte)
{
    // Variable(s) declaration
    uint8_t r_reversed;

    // Variable(s) initialization
    r_reversed = 0u;

    // Iterate over every bit in the byte
    for (uint8_t i = 0u; i < 8u; i++)
    {
        // Shift the reversed byte to the left and add the least significant bit of the input byte
        r_reversed <<= 1u;
        r_reversed |= (i_byte & 1u);
        i_byte >>= 1u;
    }

    // Return the reversed byte
    return r_reversed;
}
