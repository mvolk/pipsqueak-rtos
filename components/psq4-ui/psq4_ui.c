/*
 * MIT License
 *
 * Copyright (c) 2020 Michael Volk
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, this permission notice, and the disclaimer below
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "psq4_ui.h"
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_err.h>
#include <esp_log.h>
#include <sdkconfig.h>
#include <tft.h>
#include <st7789.h>
#include <psq4_gfx.h>
#include <adafruit_114_tft.h>
#include "psq4_ui_sprites.h"
#include <psq4_constants.h>
#include <psq4_system.h>


// TODO: consider pinning tasks to different CPUs

#define PSQ4_UI_COLOR_WHITE 0xFFFF

#define PSQ4_UI_COLOR_BG PSQ4_UI_COLOR_WHITE


static const char * PSQ4_UI_TAG = "psq4-ui";

static psq4_gfx_coords_t wifi_sprite_coords;
static const psq4_gfx_sprite_t * wifi_sprite;
static psq4_gfx_bounds_t wifi_sprite_bounds;

static psq4_gfx_coords_t mqtt_sprite_coords;
static const psq4_gfx_sprite_t * mqtt_sprite;
static psq4_gfx_bounds_t mqtt_sprite_bounds;

// static const psq4_gfx_coords_t clock_sprite_coords = { 52, 110 };
// static psq4_gfx_sprite_t * clock_sprite;
// static psq4_gfx_bounds_t clock_sprite_bounds;

static psq4_gfx_coords_t rtc_battery_sprite_coords;
static const psq4_gfx_sprite_t * rtc_battery_sprite;
static psq4_gfx_bounds_t rtc_battery_sprite_bounds;

static psq4_gfx_dim_t canvas_dim;
static psq4_gfx_canvas_t canvas;
static tft_handle_t tft;
static SemaphoreHandle_t mutex;


static bool psq4_ui_wifi_status_indicator(EventBits_t event_bits, uint8_t phase)
{
    const psq4_gfx_sprite_t * sprite = NULL;
    bool ok = false;
    wifi_sprite_coords.x = canvas_dim.w - 23;
    wifi_sprite_coords.y = canvas_dim.h - 17;

    if ((event_bits & PSQ4_WIFI_INITIALIZING_BIT) == PSQ4_WIFI_INITIALIZING_BIT) {
        if (phase == 0 || phase == 1) {
            sprite = &psq4_ui_sprite_wifi_connecting_1;
        } else if (phase == 2 || phase == 3) {
            sprite = &psq4_ui_sprite_wifi_connecting_2;
        } else {
            sprite = &psq4_ui_sprite_wifi_connecting_3;
        }
    } else if ((event_bits & PSQ4_WIFI_CONNECTED_BIT) == PSQ4_WIFI_CONNECTED_BIT) {
        sprite = &psq4_ui_sprite_wifi_ok;
        ok = true;
    } else {
        if (phase == 2 || phase == 5) {
            psq4_gfx_fill_rect(&canvas, PSQ4_UI_COLOR_BG, &wifi_sprite_bounds);
        } else {
            sprite = &psq4_ui_sprite_wifi_fail;
        }
    }

    if (sprite && wifi_sprite != sprite) {
        psq4_gfx_render_sprite(
            &canvas,
            sprite,
            &wifi_sprite_coords,
            &wifi_sprite_bounds
        );
        wifi_sprite = sprite;
    }

    return ok;
}

static bool psq4_ui_mqtt_status_indicator(EventBits_t event_bits, uint8_t phase)
{
    const psq4_gfx_sprite_t * sprite = NULL;
    bool ok = false;
    mqtt_sprite_coords.x = canvas_dim.w - 49;
    mqtt_sprite_coords.y = canvas_dim.h - 17;

    if ((event_bits & PSQ4_MQTT_INITIALIZING_BIT) == PSQ4_MQTT_INITIALIZING_BIT) {
        sprite = &psq4_ui_sprite_mqtt_connecting;
    } else if ((event_bits & PSQ4_MQTT_CONNECTED_BIT) == PSQ4_MQTT_CONNECTED_BIT) {
        sprite = &psq4_ui_sprite_mqtt_ok;
        ok = true;
    } else {
        if (phase == 2 || phase == 5) {
            psq4_gfx_fill_rect(&canvas, PSQ4_UI_COLOR_BG, &mqtt_sprite_bounds);
        } else {
            sprite = &psq4_ui_sprite_mqtt_fail;
        }
    }

    if (sprite && wifi_sprite != sprite) {
        psq4_gfx_render_sprite(
            &canvas,
            sprite,
            &mqtt_sprite_coords,
            &mqtt_sprite_bounds
        );
        mqtt_sprite = sprite;
    }

    return ok;
}


static bool psq4_ui_rtc_status_indicator(EventBits_t event_bits, uint8_t phase)
{
    // TODO: implement
    return true;
}

static bool psq4_ui_rtc_battery_status_indicator(EventBits_t event_bits, uint8_t phase)
{
    const psq4_gfx_sprite_t * sprite = NULL;
    bool ok = false;
    rtc_battery_sprite_coords.x = canvas_dim.w - 101;
    rtc_battery_sprite_coords.y = canvas_dim.h - 17;

    if ((event_bits & PSQ4_CLOCK_BATTERY_DEAD_BIT) == PSQ4_CLOCK_BATTERY_DEAD_BIT) {
        if (phase == 2 || phase == 5) {
            sprite = &psq4_ui_sprite_battery_dead_off;
        } else {
            sprite = &psq4_ui_sprite_battery_dead_on;
        }
    } else {
        sprite = &psq4_ui_sprite_battery_ok;
        ok = true;
    }

    if (rtc_battery_sprite != sprite) {
        psq4_gfx_render_sprite(
            &canvas,
            sprite,
            &rtc_battery_sprite_coords,
            &rtc_battery_sprite_bounds
        );
        rtc_battery_sprite = sprite;
    }

    return ok;
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

    const char * task_name = pcTaskGetTaskName(NULL);
    size_t stack_rem = uxTaskGetStackHighWaterMark(NULL);
    ESP_LOGI(PSQ4_UI_TAG, "Stack remaining for task '%s' is %d bytes prior to loop entry", pcTaskGetTaskName(NULL), uxTaskGetStackHighWaterMark(NULL));

    while (true) {
        // Wait for update batches
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
            xSemaphoreGive(mutex);
        }
        esp_err_t ret = psq4_gfx_flush(
            &canvas,
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
            tft16_render(
                tft,
                buffer,
                bounds.x0,
                bounds.y0,
                bounds.x1,
                bounds.y1
            );
        }
        len_bytes = 0;
        if (uxTaskGetStackHighWaterMark(NULL) != stack_rem) {
            stack_rem = uxTaskGetStackHighWaterMark(NULL);
            ESP_LOGI(PSQ4_UI_TAG, "Stack remaining for task '%s' is %d bytes", task_name, stack_rem);
        }
    }
}


void psq4_ui_task(void * pvParameters)
{
    mutex = xSemaphoreCreateMutex();
    st7789_params_t params;
    params.host = CONFIG_PSQ4_SPI_HOST;
    params.gpio_cs = CONFIG_PSQ4_DISPLAY_CS_GPIO;
    params.gpio_dc = CONFIG_PSQ4_DISPLAY_DC_GPIO;
    params.gpio_rst = CONFIG_PSQ4_DISPLAY_RST_GPIO;
    params.gpio_bckl = CONFIG_PSQ4_DISPLAY_BCKL_GPIO;
    tft = adafruit_114_tft_init(&params);
    canvas_dim.w = tft->info.width;
    canvas_dim.h = tft->info.height;
    psq4_gfx_init(&canvas, &canvas_dim);

    // Paint the background color over the entire canvas
    psq4_gfx_bounds_t canvas_bounds = {0, 0, canvas_dim.w - 1, canvas_dim.h - 1};
    psq4_gfx_fill_rect(&canvas, PSQ4_UI_COLOR_BG, &canvas_bounds);

    // Start flushing to the display
    xTaskCreate(&psq4_ui_flush_task, "flushUITask", 2048, pvParameters, 5, NULL);

    // Keep the UI up-to-date
    uint8_t phase = 0;
    EventBits_t event_bits;
    bool ok;
    while (true) {
        ok = true;
        if (xSemaphoreTake(mutex, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        event_bits = xEventGroupGetBits(psq4_system()->event_group);

        ok = ok && psq4_ui_wifi_status_indicator(event_bits, phase);
        ok = ok && psq4_ui_mqtt_status_indicator(event_bits, phase);
        ok = ok && psq4_ui_rtc_status_indicator(event_bits, phase);
        ok = ok && psq4_ui_rtc_battery_status_indicator(event_bits, phase);

        xSemaphoreGive(mutex);
        phase = (phase + 1) % 6;
        vTaskDelay(250.0 / portTICK_PERIOD_MS);
    }
}
