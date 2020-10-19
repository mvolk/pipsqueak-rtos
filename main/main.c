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
#include "temperatureSensor.h"
#include "ds3231.h"
#include "timekeeper.h"
#include "psq4_spi_bus.h"
#include "st7735r.h"
#include "adafruit_144_tft.h"
#include "psq4_gfx.h"
#include "sdkconfig.h"
#include "psq4_ui.h"

typedef struct {
    const char * name;
    QueueHandle_t queue;
} TemperatureFeed_t;

// See https://www.esp32.com/viewtopic.php?t=1442#p6613
#define MAX_SPI_TRANS_SIZE_BYTES 4096

uint8_t externalSensorData[2 * sizeof(float)];
StaticQueue_t externalSensorQueue;
TemperatureSensor_t externalSensor;
TemperatureFeed_t externalTemperatureFeed;

uint8_t internalSensorData[2 * sizeof(float)];
StaticQueue_t internalSensorQueue;
TemperatureSensor_t internalSensor;
TemperatureFeed_t internalTemperatureFeed;

DS3231_Info ds3231;

spi_bus_config_t spi_bus_cfg;
psq4_ui_params_t ui_params;

void consumeTemperature(void *pvParameters) {
    TemperatureFeed_t * feed = (TemperatureFeed_t *) pvParameters;
    QueueHandle_t queue = feed->queue;
    float reading;
    while (true) {
        if (xQueueReceive(queue, &reading, 5000.0 / portTICK_PERIOD_MS) == pdTRUE) {
            ESP_LOGD("main", "CONSUMED: %.3f C from %s\n", reading, feed->name);
        } else {
            printf("FAILED to read from %s queue (empty?)\n", feed->name);
        }
    }
}


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

    ui_params.max_trans_size = MAX_SPI_TRANS_SIZE_BYTES;
    xTaskCreate(&psq4_ui_paint_task, "uiTask", 2048, &ui_params, 5, NULL);

    ds3231_init_info(&ds3231, I2C_NUM_0, 21, 22, 1000);
    xTaskCreate(&timekeeper_task, "timekeeperTask", 2048, &ds3231, 5, NULL);

    externalSensor.name = "External sensor";
    externalSensor.oneWireGPIO = (CONFIG_PSQ4_EXTERNAL_DS18B20_GPIO);
    externalSensor.tx_channel = RMT_CHANNEL_0;
    externalSensor.rx_channel = RMT_CHANNEL_1;
    externalSensor.resolution = DS18B20_RESOLUTION_12_BIT;
    QueueHandle_t externalTemperatureQueue = xQueueCreateStatic(2, sizeof(float), externalSensorData, &externalSensorQueue);
    externalSensor.queue = externalTemperatureQueue;
    xTaskCreate(&senseTemperature, "senseExternalTemperatureTask", 2048, &externalSensor, 5, NULL);

    externalTemperatureFeed.queue = externalTemperatureQueue;
    externalTemperatureFeed.name = "External sensor";
    xTaskCreate(&consumeTemperature, "externalTemperatureConsumeTask", 2048, &externalTemperatureFeed, 5, NULL);

    internalSensor.name = "Internal sensor";
    internalSensor.oneWireGPIO = (CONFIG_PSQ4_INTERNAL_DS18B20_GPIO);
    internalSensor.tx_channel = RMT_CHANNEL_2;
    internalSensor.rx_channel = RMT_CHANNEL_3;
    internalSensor.resolution = DS18B20_RESOLUTION_9_BIT;
    QueueHandle_t internalTemperatureQueue = xQueueCreateStatic(2, sizeof(float), internalSensorData, &internalSensorQueue);
    internalSensor.queue = internalTemperatureQueue;
    xTaskCreate(&senseTemperature, "senseInternalTemperatureTask", 2048, &internalSensor, 5, NULL);

    internalTemperatureFeed.queue = internalTemperatureQueue;
    internalTemperatureFeed.name = "Internal sensor";
    xTaskCreate(&consumeTemperature, "internalTemperatureConsumeTask", 2048, &internalTemperatureFeed, 5, NULL);
}
