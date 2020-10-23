#include "psq4_ui.h"
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>
#include <sdkconfig.h>
#include <adafruit_144_tft.h>
#include "psq4_ui_sprites.h"


// TODO: consider pinning tasks to different CPUs

#define PSQ4_UI_COLOR_WHITE 0xFFFF

#define PSQ4_UI_COLOR_BG PSQ4_UI_COLOR_WHITE


const char * PSQ4_UI_TAG = "psq4-ui";
const psq4_gfx_coords_t psq4_ui_wifi_sprite_coords = { 104, 110 };
psq4_gfx_bounds_t psq4_ui_wifi_sprite_bounds;
psq4_gfx_dim_t psq4_ui_canvas_dim = {
    CONFIG_PSQ4_DISPLAY_WIDTH,
    CONFIG_PSQ4_DISPLAY_HEIGHT
};
psq4_gfx_canvas_t psq4_ui_canvas;
st7735r_device_t psq4_ui_st7735r;


// TODO: this really needs to be handled by a separate "blinker" task
static void psq4_ui_wifi_status_indicator()
{
    psq4_gfx_render_sprite(
        &psq4_ui_canvas,
        &psq4_ui_sprite_wifi_fail,
        &psq4_ui_wifi_sprite_coords,
        &psq4_ui_wifi_sprite_bounds
    );
    vTaskDelay(500.0 / portTICK_PERIOD_MS);
    psq4_gfx_fill_rect(&psq4_ui_canvas, 0xFFFF, &psq4_ui_wifi_sprite_bounds);
    vTaskDelay(250.0 / portTICK_PERIOD_MS);
}


static void psq4_ui_flush_task(void * pvParameters)
{
    psq4_ui_params_t * params = (psq4_ui_params_t *) pvParameters;
    psq4_gfx_bounds_t bounds;
    size_t len_bytes;
    size_t buffer_len_bytes = params->max_trans_size;
    uint16_t *buffer = (uint16_t *) malloc(buffer_len_bytes);
    if (!buffer) {
        ESP_LOGE(
            PSQ4_UI_TAG,
            "Failed to allocate a %d-byte buffer",
            buffer_len_bytes
        );
        // Returning from the task prompts a restart
        return;
    }

    while (true) {
        esp_err_t ret = psq4_gfx_flush(
            &psq4_ui_canvas,
            buffer,
            buffer_len_bytes,
            &bounds,
            &len_bytes
        );
        ESP_ERROR_CHECK(ret);
        ESP_LOGD(
            PSQ4_UI_TAG,
            "Flushing {%d, %d} to {%d, %d}",
            bounds.x0,
            bounds.y0,
            bounds.x1,
            bounds.y1
        );
        if (len_bytes > 0) {
            adafruit_144_tft_paint(
                &psq4_ui_st7735r,
                buffer,
                bounds.x0,
                bounds.y0,
                bounds.x1,
                bounds.y1
            );
        }
        len_bytes = 0;
    }
}


void psq4_ui_task(void * pvParameters)
{
    // TODO: pre-calculate sprite bounds, so that blinker can start by
    // TODO: clearing existing content if entering before an off cycle?

    psq4_ui_st7735r.host = CONFIG_PSQ4_SPI_HOST;
    psq4_ui_st7735r.gpio_cs = CONFIG_PSQ4_DISPLAY_CS_GPIO;
    psq4_ui_st7735r.gpio_dc = CONFIG_PSQ4_DISPLAY_DC_GPIO;
    psq4_ui_st7735r.gpio_rst = CONFIG_PSQ4_DISPLAY_RST_GPIO;
    psq4_ui_st7735r.gpio_bckl = CONFIG_PSQ4_DISPLAY_BCKL_GPIO;
    adafruit_144_tft_init(&psq4_ui_st7735r);
    psq4_gfx_init(&psq4_ui_canvas, &psq4_ui_canvas_dim);

    // Paint the background color over the entire canvas
    psq4_gfx_bounds_t canvas_bounds = {
        0,
        0,
        psq4_ui_canvas_dim.w - 1,
        psq4_ui_canvas_dim.h - 1
    };
    psq4_gfx_fill_rect(&psq4_ui_canvas, PSQ4_UI_COLOR_BG, &canvas_bounds);

    // Start flushing to the display
    xTaskCreate(&psq4_ui_flush_task, "flushUITask", 1024, pvParameters, 5, NULL);

    // Keep the UI up-to-date
    while (true) {
        psq4_ui_wifi_status_indicator();
    }
}
