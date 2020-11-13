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

#include "psq4_telemetry.h"

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <sdkconfig.h>
#include <aws_iot_mqtt_client_interface.h>
#include <psq4_system.h>
#include <psq4_constants.h>
#include <psq4_aws_iot.h>


#define TELEMETRY_TOPIC_TEMPLATE "data/pipsqueak/v4/telemetry/%s"
#define TELEMETRY_JSON_TEMPLATE "{\"timestamp\": %ld, \"temperature\": %.4f}"


static const char * PSQ4_TELEMETRY_TAG = "psq4-telemetry";
static TaskHandle_t psq4_temperature_telemetry_task;


static void temperature_telemetry_task(void *ignored)
{
    psq4_system_handle_t system = psq4_system();

    char topic[255];
    if (strlen(CONFIG_AWS_IOT_THING_NAME) > 200) {
      ESP_LOGE(
          PSQ4_TELEMETRY_TAG,
          "FATAL: AWS IoT Thing name exceeds 200 characters in length"
      );
      abort();
    }
    sprintf(topic, TELEMETRY_TOPIC_TEMPLATE, CONFIG_AWS_IOT_THING_NAME);

    // Reliable clock required for telemetry timestamps
    psq4_system_await_clock(portMAX_DELAY);

    time_t timestamp;
    uint32_t notification_value;
    float ewma_temperature;
    char json[100];
    BaseType_t wait_result;
    while((wait_result = xTaskNotifyWait(0, 0, &notification_value, portMAX_DELAY)) == pdTRUE) {
        memcpy((void *) &ewma_temperature, (const void *) &notification_value, sizeof(float));
        timestamp = psq4_system_time();
        sprintf(json, TELEMETRY_JSON_TEMPLATE, timestamp, ewma_temperature);
        ESP_LOGI(PSQ4_TELEMETRY_TAG, "Emitting a temperature change event: now %0.4f C", ewma_temperature);
        // Note: this blocks until complete, which with network operations could be a long
        // time... and that means we could lose datapoints. Fine for getting started, but...
        // TODO: ensure that significant datapoints are not lost
        psq4_mqtt_publish(topic, QOS1, json);
    };
    ESP_LOGE(
        PSQ4_TELEMETRY_TAG,
        "FATAL: failed to receive temperature reading from distributor, code %d",
        wait_result
    );
}


void psq4_telemetry_init()
{
    xTaskCreate(
        &temperature_telemetry_task,
        "temperatureTelemetryTask",
        9056,
        NULL,
        5,
        &psq4_temperature_telemetry_task
    );
    ESP_ERROR_CHECK(psq4_temperature_add_consumer(psq4_temperature_telemetry_task, 10000 / portTICK_PERIOD_MS));
}
