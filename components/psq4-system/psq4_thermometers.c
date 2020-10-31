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
 *
 * Derived from the MIT-licensed work of David Antliff
 * https://github.com/DavidAntliff/esp32-ds18b20-example
 * Original license reproduced above; original copyright reproduced below:
 * Copyright (c) 2017 David Antliff
 */

#include "psq4_thermometers.h"
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/rmt.h>
#include <esp_log.h>
#include <owb.h>
#include <owb_rmt.h>
#include "psq4_constants.h"
#include <ds18b20.h>


typedef struct {
    const char * name;
    int oneWireGPIO;
    rmt_channel_t tx_channel;
    rmt_channel_t rx_channel;
    DS18B20_RESOLUTION resolution;
    QueueHandle_t queue;
} psq4_temperature_sensor_t;


typedef struct {
    const char * name;
    QueueHandle_t queue;
    EventGroupHandle_t system_event_group;
    EventBits_t initializingBit;
    EventBits_t okBit;
} psq4_temperature_feed_t;


static const char * PSQ4_TEMPERATURE_TAG = "psq4-system/thermometers";

static uint8_t psq4_temperature_external_data[2 * sizeof(float)];
static StaticQueue_t psq4_temperature_external_queue;
static psq4_temperature_sensor_t psq4_temperature_external_sensor;
static psq4_temperature_feed_t psq4_temperature_external_feed;

static uint8_t psq4_temperature_internal_data[2 * sizeof(float)];
static StaticQueue_t psq4_temperature_internal_queue;
static psq4_temperature_sensor_t psq4_temperature_internal_sensor;
static psq4_temperature_feed_t psq4_temperature_internal_feed;


static void psq4_temperature_consume(void *pvParameters)
{
    psq4_temperature_feed_t * feed = (psq4_temperature_feed_t *) pvParameters;
    QueueHandle_t queue = feed->queue;
    float reading;
    BaseType_t status_code;
    while (true) {
        status_code = xQueueReceive(queue, &reading, 5000.0 / portTICK_PERIOD_MS);
        if (status_code == pdTRUE) {
            xEventGroupSetBits(feed->system_event_group, feed->okBit);
            ESP_LOGD(
                PSQ4_TEMPERATURE_TAG,
                "CONSUMED: %.3f C from %s",
                reading,
                feed->name
            );
        } else {
            xEventGroupClearBits(feed->system_event_group, feed->okBit);
            ESP_LOGE(
                PSQ4_TEMPERATURE_TAG,
                "FAILED to read from %s queue, code %d",
                feed->name,
                status_code
            );
        }
        xEventGroupClearBits(feed->system_event_group, feed->initializingBit);
    }
}


static void psq4_temperature_sense(void * pvParameters)
{
    psq4_temperature_sensor_t * sensor = (psq4_temperature_sensor_t *) pvParameters;

    // Stable readings require a brief period before communication
    vTaskDelay(2000.0 / portTICK_PERIOD_MS);

    // Create a 1-Wire bus, using the RMT timeslot driver
    OneWireBus * owb;
    owb_rmt_driver_info rmt_driver_info;
    owb = owb_rmt_initialize(
        &rmt_driver_info,
        sensor->oneWireGPIO,
        sensor->tx_channel,
        sensor->rx_channel
    );
    owb_use_crc(owb, true);

    // Find connected device
    int num_devices = 0;
    OneWireBus_SearchState search_state = {0};
    bool found = false;
    owb_search_first(owb, &search_state, &found);
    while (found) {
        char rom_code_s[17];
        owb_string_from_rom_code(
            search_state.rom_code,
            rom_code_s,
            sizeof(rom_code_s)
        );
        ESP_LOGI(
            PSQ4_TEMPERATURE_TAG,
            "Seeking %s device, found candidate #%d: %s",
            sensor->name,
            ++num_devices,
            rom_code_s
        );
        ++num_devices;
        owb_search_next(owb, &search_state, &found);
    }
    ESP_LOGI(
        PSQ4_TEMPERATURE_TAG,
        "Found %d potential %s device%s",
        num_devices,
        sensor->name,
        num_devices == 1 ? "" : "s"
    );

    // For a single device only:
    OneWireBus_ROMCode rom_code;
    owb_status status = owb_read_rom(owb, &rom_code);
    if (status == OWB_STATUS_OK) {
        char rom_code_s[OWB_ROM_CODE_STRING_LENGTH];
        owb_string_from_rom_code(rom_code, rom_code_s, sizeof(rom_code_s));
        ESP_LOGI(
            PSQ4_TEMPERATURE_TAG,
            "Single %s device (%s) present",
            sensor->name,
            rom_code_s
        );
    } else {
        ESP_LOGE(
            PSQ4_TEMPERATURE_TAG,
            "An error occurred reading ROM code for %s: %d",
            sensor->name,
            status
        );
    }

    DS18B20_Info * device = ds18b20_malloc();
    ESP_LOGI(
        PSQ4_TEMPERATURE_TAG,
        "Single device optimisations enabled for %s",
        sensor->name
    );
    ds18b20_init_solo(device, owb);
    ds18b20_use_crc(device, true);
    ds18b20_set_resolution(device, sensor->resolution);

    int status_code = 0;
    int error_count = 0;
    int sample_count = 0;
    while (true) {
        ds18b20_convert_all(owb);

        // In this application all devices use the same resolution,
        // so use the first device to determine the delay
        ds18b20_wait_for_conversion(device);

        // Read the results immediately after conversion otherwise it may fail
        // (using printf before reading may take too long)

        float reading;
        status_code = ds18b20_read_temp(device, &reading);
        if (status_code != DS18B20_OK) {
            ++error_count;
            ESP_LOGE(
                PSQ4_TEMPERATURE_TAG,
                "%s sample %d failed with code %d",
                sensor->name,
                ++sample_count,
                status_code
            );
            continue;
        }

        // Print results in a separate loop, after all have been read
        ESP_LOGD(
            PSQ4_TEMPERATURE_TAG,
            "%s sample %d: %.3f C (%d errors)",
            sensor->name,
            ++sample_count,
            reading,
            error_count
        );

        if (xQueueSend(sensor->queue, &reading, 250.0 / portTICK_PERIOD_MS) != pdTRUE) {
            ESP_LOGE(
                PSQ4_TEMPERATURE_TAG,
                "FAILED to enqueue %s temperature reading",
                sensor->name
            );
        }
    }
}

void psq4_thermometers_init(EventGroupHandle_t system_event_group) {
    psq4_temperature_external_sensor.name = "External sensor";
    psq4_temperature_external_sensor.oneWireGPIO = (CONFIG_PSQ4_EXTERNAL_DS18B20_GPIO);
    psq4_temperature_external_sensor.tx_channel = RMT_CHANNEL_0;
    psq4_temperature_external_sensor.rx_channel = RMT_CHANNEL_1;
    psq4_temperature_external_sensor.resolution = DS18B20_RESOLUTION_12_BIT;
    QueueHandle_t external_queue =
        xQueueCreateStatic(
            2,
            sizeof(float),
            psq4_temperature_external_data,
            &psq4_temperature_external_queue
        );
    psq4_temperature_external_sensor.queue = external_queue;
    xTaskCreate(
        &psq4_temperature_sense,
        "senseExternalTemperatureTask",
        2048,
        &psq4_temperature_external_sensor,
        5,
        NULL
    );

    psq4_temperature_external_feed.queue = external_queue;
    psq4_temperature_external_feed.name = "External sensor";
    psq4_temperature_external_feed.system_event_group = system_event_group;
    psq4_temperature_external_feed.initializingBit = PSQ4_THERMO_MEDIUM_INITIALIZING_BIT;
    psq4_temperature_external_feed.okBit = PSQ4_THERMO_MEDIUM_OK_BIT;
    xTaskCreate(
        &psq4_temperature_consume,
        "externalTemperatureConsumeTask",
        2048,
        &psq4_temperature_external_feed,
        5,
        NULL
    );

    psq4_temperature_internal_sensor.name = "Internal sensor";
    psq4_temperature_internal_sensor.oneWireGPIO = (CONFIG_PSQ4_INTERNAL_DS18B20_GPIO);
    psq4_temperature_internal_sensor.tx_channel = RMT_CHANNEL_2;
    psq4_temperature_internal_sensor.rx_channel = RMT_CHANNEL_3;
    psq4_temperature_internal_sensor.resolution = DS18B20_RESOLUTION_9_BIT;
    QueueHandle_t internal_queue =
        xQueueCreateStatic(
            2,
            sizeof(float),
            psq4_temperature_internal_data,
            &psq4_temperature_internal_queue
        );
    psq4_temperature_internal_sensor.queue = internal_queue;
    xTaskCreate(
        &psq4_temperature_sense,
        "senseInternalTemperatureTask",
        2048,
        &psq4_temperature_internal_sensor,
        5,
        NULL
    );

    psq4_temperature_internal_feed.queue = internal_queue;
    psq4_temperature_internal_feed.name = "Internal sensor";
    psq4_temperature_internal_feed.system_event_group = system_event_group;
    psq4_temperature_internal_feed.initializingBit = PSQ4_THERMO_BOARD_INITIALIZING_BIT;
    psq4_temperature_internal_feed.okBit = PSQ4_THERMO_BOARD_OK_BIT;
    xTaskCreate(
        &psq4_temperature_consume,
        "internalTemperatureConsumeTask",
        2048,
        &psq4_temperature_internal_feed,
        5,
        NULL
    );
}
