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

#include "psq4_system.h"
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <esp_system.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include "psq4_constants.h"

extern void psq4_time_init(EventGroupHandle_t system_event_group);
extern time_t psq4_time_now();
void psq4_thermometers_init(EventGroupHandle_t system_event_group);
extern void psq4_wifi_init(EventGroupHandle_t system_event_group);

static psq4_system_t _psq4_system;


static void nvs_init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}


// Initializes the SPI bus, reboots on failure
static void spi_init()
{
    _psq4_system.spi_bus_cfg.miso_io_num = CONFIG_PSQ4_SPI_MISO_GPIO;
    _psq4_system.spi_bus_cfg.mosi_io_num = CONFIG_PSQ4_SPI_MOSI_GPIO;
    _psq4_system.spi_bus_cfg.sclk_io_num = CONFIG_PSQ4_SPI_CLK_GPIO;
    _psq4_system.spi_bus_cfg.quadwp_io_num = -1;
    _psq4_system.spi_bus_cfg.quadhd_io_num = -1;
    _psq4_system.spi_bus_cfg.max_transfer_sz = PSQ4_SPI_MAX_TRANS_SIZE_BYTES;
    esp_err_t ret = spi_bus_initialize(
        CONFIG_PSQ4_SPI_HOST,
        &_psq4_system.spi_bus_cfg,
        CONFIG_PSQ4_SPI_DMA_CHANNEL
    );
    ESP_ERROR_CHECK(ret);
}


psq4_system_handle_t psq4_system_init() {
    EventGroupHandle_t event_group = xEventGroupCreate();
    if (event_group == NULL) {
      ESP_LOGE("psq4-system", "Failed to create event group");
      esp_restart();
    }
    xEventGroupSetBits(event_group, PSQ4_WIFI_INITIALIZING_BIT);
    xEventGroupSetBits(event_group, PSQ4_CLOCK_INITIALIZING_BIT);
    xEventGroupSetBits(event_group, PSQ4_MQTT_INITIALIZING_BIT);
    xEventGroupSetBits(event_group, PSQ4_THERMO_BOARD_INITIALIZING_BIT);
    xEventGroupSetBits(event_group, PSQ4_THERMO_MEDIUM_INITIALIZING_BIT);
    _psq4_system.event_group = event_group;

    // Initialize NVS
    nvs_init();

    // Initialize the SPI bus (without adding devices to it)
    spi_init();

    // Initialize WiFi
    psq4_wifi_init(event_group);

    // Initialize temperature sensors
    psq4_thermometers_init(event_group);

    // Initialize timekeeping
    psq4_time_init(event_group);

    return &_psq4_system;
}


psq4_system_handle_t psq4_system() {
    return &_psq4_system;
}


esp_err_t psq4_system_await_wifi(TickType_t xTicksToWait)
{
    EventBits_t bits = xEventGroupWaitBits(
        _psq4_system.event_group,
        PSQ4_WIFI_CONNECTED_BIT,
        false,
        true,
        xTicksToWait
    );
    if ((bits & PSQ4_WIFI_CONNECTED_BIT) == PSQ4_WIFI_CONNECTED_BIT) {
        return ESP_OK;
    }
    return ESP_ERR_TIMEOUT;
}


esp_err_t psq4_system_await_clock(TickType_t xTicksToWait)
{
    EventBits_t bits = xEventGroupWaitBits(
        _psq4_system.event_group,
        PSQ4_CLOCK_READY_BIT,
        false,
        true,
        xTicksToWait
    );
    if ((bits & PSQ4_CLOCK_READY_BIT) == PSQ4_CLOCK_READY_BIT) {
        return ESP_OK;
    }
    return ESP_ERR_TIMEOUT;
}


time_t psq4_system_time() {
  return psq4_time_now();
}

