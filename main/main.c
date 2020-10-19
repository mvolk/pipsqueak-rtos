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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/rmt.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "sdkconfig.h"
#include "st7735r.h"
#include "adafruit_144_tft.h"
#include "psq4_temperature.h"
#include "psq4_time.h"
#include "psq4_spi_bus.h"
#include "psq4_gfx.h"
#include "psq4_ui.h"


// See https://www.esp32.com/viewtopic.php?t=1442#p6613
#define MAX_SPI_TRANS_SIZE_BYTES 4096


spi_bus_config_t spi_bus_cfg;
psq4_ui_params_t ui_params;


static void spi_bus_init()
{
    // TODO: forewarning that SPI mode 0 w/DMA may not work for reading MISO,
    // TODO: per ESP-IDF docs - may have to use Mode 1, or avoid using DMA?
    psq4_spi_bus_init(
        &spi_bus_cfg,
        CONFIG_PSQ4_SPI_MISO_GPIO,
        CONFIG_PSQ4_SPI_MOSI_GPIO,
        CONFIG_PSQ4_SPI_CLK_GPIO,
        CONFIG_PSQ4_SPI_HOST,
        CONFIG_PSQ4_SPI_DMA_CHANNEL,
        MAX_SPI_TRANS_SIZE_BYTES
    );
}


void app_main(void)
{
    spi_bus_init();
    psq4_temperature_tasks_init();
    ui_params.max_trans_size = MAX_SPI_TRANS_SIZE_BYTES;
    xTaskCreate(&psq4_ui_task, "uiTask", 2048, &ui_params, 5, NULL);
    xTaskCreate(&psq4_time_task, "timeTask", 2048, NULL, 5, NULL);
}
