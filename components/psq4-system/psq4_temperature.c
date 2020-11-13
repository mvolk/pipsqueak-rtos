/*
 * MIT License
 *
 * Copyright (c) 2017 David Antliff
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
 *
 * Derived from the MIT-licensed work of David Antliff:
 * https://github.com/DavidAntliff/esp32-ds18b20-example
 */

#include <math.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/event_groups.h>
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
    EventGroupHandle_t event_group;
} psq4_temperature_sensor_t;


#define PSQ4_TEMPERATURE_MAX_CONSUMERS 2
#define PSQ4_TEMPERATURE_INVALID -1024.0
#define PSQ4_TEMPERATURE_WEIGHT 0.2
#define PSQ4_TEMPERATURE_CHANGE_THRESHOLD 0.0125

static const char * PSQ4_TEMPERATURE_TAG = "psq4-system/thermometer";
static psq4_temperature_sensor_t psq4_temperature_sensor;
static TaskHandle_t psq4_temperature_distribute_task;
static TaskHandle_t psq4_temperature_consumers[PSQ4_TEMPERATURE_MAX_CONSUMERS];
static size_t psq4_temperature_consumer_count = 0;
static SemaphoreHandle_t psq4_temperature_consumer_mutex;


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
        "Single device optimizations enabled for %s",
        sensor->name
    );
    ds18b20_init_solo(device, owb);
    ds18b20_use_crc(device, true);
    ds18b20_set_resolution(device, sensor->resolution);

    int status_code;
    int error_count = 0;
    int read_attempt;
    bool first_reading = true;
    uint32_t notification_value;
    while (true) {
        ds18b20_convert_all(owb);

        // In this application all devices use the same resolution,
        // so use the first device to determine the delay
        ds18b20_wait_for_conversion(device);

        // Read the results immediately after conversion otherwise it may fail
        // (using printf before reading may take too long)

        float reading;
        read_attempt = 0;
        do {
            status_code = ds18b20_read_temp(device, &reading);
            if (status_code != DS18B20_OK) {
                ++error_count;
                ESP_LOGW(
                    PSQ4_TEMPERATURE_TAG,
                    "%s read attempt %d failed with code %d",
                    sensor->name,
                    ++read_attempt,
                    status_code
                );
                continue;
            }
        } while (status_code != DS18B20_OK && read_attempt <= 3);

        if (status_code == DS18B20_OK ) {
            ESP_LOGD(
                PSQ4_TEMPERATURE_TAG,
                "%s read attempt %d: %.3f C",
                sensor->name,
                read_attempt,
                reading
            );

            memcpy((void *) &notification_value, (const void *) &reading, sizeof(float));
            BaseType_t result = xTaskNotify(psq4_temperature_distribute_task, notification_value, eSetValueWithOverwrite);
            if (result != pdPASS) {
                ESP_LOGE(
                    PSQ4_TEMPERATURE_TAG,
                    "FATAL: Failed to emit %s temperature reading, code %d",
                    sensor->name,
                    result
                );
                esp_restart();
            }

            if (first_reading) {
                xEventGroupClearBits(sensor->event_group, PSQ4_THERMOMETER_INITIALIZING_BIT);
                first_reading = false;
            }

            xEventGroupSetBits(sensor->event_group, PSQ4_THERMOMETER_OK_BIT);
        } else {
            xEventGroupClearBits(sensor->event_group, PSQ4_THERMOMETER_OK_BIT);
        }
    }
}


static void psq4_temperature_distribute(void * pvParameters) {
    float distributed_temperature = PSQ4_TEMPERATURE_INVALID;
    float ewma_temperature = PSQ4_TEMPERATURE_INVALID;
    float current_temperature;
    uint32_t notification_value;
    BaseType_t wait_result;
    BaseType_t send_result;
    while((wait_result = xTaskNotifyWait(0, 0, &notification_value, portMAX_DELAY)) == pdTRUE) {
        memcpy((void *) &current_temperature, (const void *) &notification_value, sizeof(float));
        // EWMA smoothing
        if (ewma_temperature == PSQ4_TEMPERATURE_INVALID) {
            ewma_temperature = current_temperature;
        }
        ewma_temperature =
                ((1 - PSQ4_TEMPERATURE_WEIGHT) * ewma_temperature) +
                (PSQ4_TEMPERATURE_WEIGHT * current_temperature);
        if (fabs(ewma_temperature - distributed_temperature) > PSQ4_TEMPERATURE_CHANGE_THRESHOLD) {
            memcpy((void *) &notification_value, (const void *) &ewma_temperature, sizeof(float));
            size_t count = psq4_temperature_consumer_count;
            for (size_t i = 0; i < count; i++) {
                send_result = xTaskNotify(psq4_temperature_consumers[i], notification_value, eSetValueWithOverwrite);
                if (send_result != pdPASS) {
                    ESP_LOGE(
                        PSQ4_TEMPERATURE_TAG,
                        "FATAL: Failed to distribute temperature reading, code %d",
                        send_result
                    );
                    esp_restart();
                }
            }
            distributed_temperature = ewma_temperature;
        }
    };
    ESP_LOGE(
        PSQ4_TEMPERATURE_TAG,
        "FATAL: failed to receive temperature reading from sensor, code %d",
        wait_result
    );
}


esp_err_t psq4_temperature_add_consumer(TaskHandle_t task, TickType_t ticks_to_wait) {
    if (xSemaphoreTake(psq4_temperature_consumer_mutex, ticks_to_wait) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    esp_err_t result = ESP_OK;
    if (psq4_temperature_consumer_count == PSQ4_TEMPERATURE_MAX_CONSUMERS) {
        result = ESP_FAIL;
    } else {
        psq4_temperature_consumers[psq4_temperature_consumer_count] = task;
        psq4_temperature_consumer_count++;
    }
    if (xSemaphoreGive(psq4_temperature_consumer_mutex) != pdTRUE) {
        ESP_LOGE(PSQ4_TEMPERATURE_TAG, "FATAL: Failed to release temperature consumer mutex");
        esp_restart();
    }
    return result;
}


void psq4_temperature_init(EventGroupHandle_t system_event_group) {
    psq4_temperature_consumer_mutex = xSemaphoreCreateMutex();
    if (psq4_temperature_consumer_mutex == NULL) {
        ESP_LOGE(PSQ4_TEMPERATURE_TAG, "FATAL: Failed to create temperature consumer mutex");
        esp_restart();
    }
    psq4_temperature_sensor.name = "External sensor";
    psq4_temperature_sensor.oneWireGPIO = CONFIG_PSQ4_DS18B20_GPIO;
    psq4_temperature_sensor.tx_channel = RMT_CHANNEL_0;
    psq4_temperature_sensor.rx_channel = RMT_CHANNEL_1;
    psq4_temperature_sensor.resolution = DS18B20_RESOLUTION_12_BIT;
    psq4_temperature_sensor.event_group = system_event_group;
    xTaskCreate(
        &psq4_temperature_distribute,
        "distributeTemperatureTask",
        2048,
        NULL,
        5,
        &psq4_temperature_distribute_task
    );
    xTaskCreate(
        &psq4_temperature_sense,
        "senseTemperatureTask",
        2048,
        &psq4_temperature_sensor,
        5,
        NULL
    );
}
