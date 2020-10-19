#ifndef PSQ4_UI_H
#define PSQ4_UI_H

#include <freertos/FreeRTOS.h>
#include <st7735r.h>
#include <psq4_gfx.h>


typedef struct {
    st7735r_device_handle_t display;
    psq4_gfx_canvas_t *canvas;
    size_t max_trans_size;
} psq4_ui_params_t;


void psq4_ui_paint_task(void * pvParameters);


void psq4_ui_flush_task(void * pvParameters);


#endif // PSQ4_UI_H
