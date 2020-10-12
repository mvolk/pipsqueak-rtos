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
#include "driver/gpio.h"
#include "driver/rmt.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "temperatureSensor.h"

typedef struct
{
    const char * name;
    QueueHandle_t queue;
} TemperatureFeed_t;

uint8_t externalSensorData[2 * sizeof(float)];
StaticQueue_t externalSensorQueue;
TemperatureSensor_t externalSensor;
TemperatureFeed_t externalTemperatureFeed;

uint8_t internalSensorData[2 * sizeof(float)];
StaticQueue_t internalSensorQueue;
TemperatureSensor_t internalSensor;
TemperatureFeed_t internalTemperatureFeed;

void consumeTemperature(void * pvParameters) {
    TemperatureFeed_t * feed = (TemperatureFeed_t *) pvParameters;
    QueueHandle_t queue = feed->queue;
    float reading;
    while (true) {
      if (xQueueReceive(queue, &reading, 5000.0 / portTICK_PERIOD_MS) == pdTRUE) {
          printf("CONSUMED: %.3f C from %s\n", reading, feed->name);
      } else {
          printf("FAILED to read from %s queue (empty?)\n", feed->name);
      }
    }
}

void app_main(void)
{
    externalSensor.name = "External sensor";
    externalSensor.oneWireGPIO = (CONFIG_EXTERNAL_DS18B20_GPIO);
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
    internalSensor.oneWireGPIO = (CONFIG_INTERNAL_DS18B20_GPIO);
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
