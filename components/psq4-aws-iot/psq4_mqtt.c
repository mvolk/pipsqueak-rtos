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
 * The above copyright notice this permission notice, and the disclaimer below
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

#include "psq4_aws_iot.h"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_system.h>
#include <esp_log.h>

#include <aws_iot_config.h>
#include <aws_iot_log.h>
#include <aws_iot_version.h>
#include <aws_iot_mqtt_client_interface.h>

#include <psq4_system.h>
#include <psq4_constants.h>

static const char *PSQ4_AWS_IOT_MQTT_CLIENT_TAG = "psq4-aws-iot-mqtt-client";

extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t certificate_pem_crt_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t private_pem_key_start[] asm("_binary_private_pem_key_start");

// Ultimately sourced from sdkconfig values
static char mqtt_host_address[255] = AWS_IOT_MQTT_HOST;
static uint32_t mqtt_host_port = AWS_IOT_MQTT_PORT;

static AWS_IoT_Client client;


static void handle_message(
    AWS_IoT_Client *client,
    char *topic_name,
    uint16_t topic_name_len,
    IoT_Publish_Message_Params *params,
    void *handler)
{
    ESP_LOGI(PSQ4_AWS_IOT_MQTT_CLIENT_TAG, "Subscribe callback");
    ESP_LOGI(
      PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
      "%.*s\t%.*s",
      topic_name_len,
      topic_name,
      (int) params->payloadLen,
      (char *)params->payload
    );
    if (handler) {
        ((pApplicationHandler_t) handler)(client, topic_name, topic_name_len, params, NULL);
    }
}


static void handle_disconnect(AWS_IoT_Client *client, void *ignored)
{
    // The maintenance task will attempt to reconnect
    xEventGroupClearBits(psq4_system()->event_group, PSQ4_MQTT_CONNECTED_BIT);
    ESP_LOGW(PSQ4_AWS_IOT_MQTT_CLIENT_TAG, "MQTT connection lost");
}


static bool disconnected()
{
  EventBits_t eventBits = xEventGroupGetBits(psq4_system()->event_group);
  return (eventBits & PSQ4_MQTT_CONNECTED_BIT) != PSQ4_MQTT_CONNECTED_BIT;
}


static void connect()
{
    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;
    connectParams.keepAliveIntervalInSec = 10;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    connectParams.pClientID = CONFIG_AWS_IOT_THING_NAME;
    connectParams.clientIDLen = (uint16_t) strlen(CONFIG_AWS_IOT_THING_NAME);
    connectParams.isWillMsgPresent = false;
    size_t attempt_num = 1;
    IoT_Error_t rc;
    do {
        ESP_LOGI(
            PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
            "Connecting to AWS MQTT Message Broker, attempt #%d...",
            attempt_num++
        );
        rc = aws_iot_mqtt_connect(&client, &connectParams);
        if (SUCCESS != rc) {
            ESP_LOGW(
                PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
                "IoT Error %d while connecting to %s:%d",
                rc,
                mqtt_host_address,
                mqtt_host_port
            );
            // TODO: exponential backoff
            vTaskDelay(1000 / portTICK_RATE_MS);
        } else {
            xEventGroupSetBits(psq4_system()->event_group, PSQ4_MQTT_CONNECTED_BIT);
            xEventGroupClearBits(psq4_system()->event_group, PSQ4_MQTT_INITIALIZING_BIT);
            ESP_LOGI(
                PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
                "Successfully connected to %s:%d",
                mqtt_host_address,
                mqtt_host_port
            );
        }
    } while (SUCCESS != rc);
}


static void reconnect()
{
    size_t attempt_num = 1;
    IoT_Error_t rc;
    do {
        ESP_LOGI(
            PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
            "Reconnecting to AWS MQTT Message Broker, attempt #%d...",
            attempt_num++
        );
        rc = aws_iot_mqtt_attempt_reconnect(&client);
        if (NETWORK_RECONNECTED != rc) {
            ESP_LOGE(
                PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
                "IoT Error %d while reconnecting to %s:%d",
                rc,
                mqtt_host_address,
                mqtt_host_port
            );
            // TODO: exponential backoff
            vTaskDelay(1000 / portTICK_RATE_MS);
        } else {
            xEventGroupSetBits(psq4_system()->event_group, PSQ4_MQTT_CONNECTED_BIT);
            ESP_LOGI(
                PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
                "Successfully reconnected to %s:%d",
                mqtt_host_address,
                mqtt_host_port
            );
        }
    } while(NETWORK_RECONNECTED != rc);
}


// This task provides the CPU time needed to process
// inbound messages and reconnect after disconnection
static void mqtt_maintenance_task(void *ignored)
{
    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
    mqttInitParams.enableAutoReconnect = false;
    mqttInitParams.pHostURL = mqtt_host_address;
    mqttInitParams.port = mqtt_host_port;
    mqttInitParams.pRootCALocation = (const char *)aws_root_ca_pem_start;
    mqttInitParams.pDeviceCertLocation = (const char *)certificate_pem_crt_start;
    mqttInitParams.pDevicePrivateKeyLocation = (const char *)private_pem_key_start;
    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 5000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = handle_disconnect;
    mqttInitParams.disconnectHandlerData = NULL;
    IoT_Error_t rc = aws_iot_mqtt_init(&client, &mqttInitParams);
    if (SUCCESS != rc) {
        ESP_LOGE(
            PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
            "FATAL: aws_iot_mqtt_init failed with IoT Error %d",
            rc
        );
        abort();
    }

    // Accurate clock and WiFi are prerequisites
    xEventGroupWaitBits(
        psq4_system()->event_group,
        PSQ4_CLOCK_READY_BIT | PSQ4_WIFI_CONNECTED_BIT,
        false,
        true,
        portMAX_DELAY
    );

    // Establishing the connection
    connect();

    while (true) {
        // Reconnect if disconnected
        // This takes as long as it takes, but there's nothing else for
        // mqtt to do in the meantime, so this doesn't cannibalize time
        // needed by the mqtt system
        if (disconnected()) reconnect();

        // Yield CPU time to the MQTT client
        aws_iot_mqtt_yield(&client, 100);

        vTaskDelay(500.0 / portTICK_RATE_MS);
    }
}


void psq4_mqtt_init()
{
    xTaskCreatePinnedToCore(
        &mqtt_maintenance_task,
        "mqttMaintenanceTask",
        9216,
        NULL,
        5,
        NULL,
        1
    );
}


void psq4_mqtt_subscribe(
    const char *topic,
    enum QoS qos,
    pApplicationHandler_t handler,
    EventBits_t initializingEventBit,
    EventBits_t connectedEventBit)
{
    xEventGroupClearBits(psq4_system()->event_group, connectedEventBit);
    size_t attempt_num = 1;
    IoT_Error_t rc;
    do {
        xEventGroupWaitBits(
            psq4_system()->event_group,
            PSQ4_MQTT_CONNECTED_BIT,
            false,
            true,
            portMAX_DELAY
        );
        ESP_LOGI(
            PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
            "Subscribing to %s with QOS %d, attempt #%d...",
            topic,
            qos,
            attempt_num++
        );
        rc = aws_iot_mqtt_subscribe(
            &client,
            topic,
            strlen(topic),
            qos,
            handle_message,
            handler
        );
        if (SUCCESS != rc) {
            ESP_LOGW(
                PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
                "IoT Error %d while subscribing to topic %s",
                rc,
                topic
            );
            xEventGroupClearBits(psq4_system()->event_group, initializingEventBit);
            // TODO: exponential backoff
            vTaskDelay(1000 / portTICK_RATE_MS);
        } else {
            xEventGroupSetBits(psq4_system()->event_group, connectedEventBit);
            xEventGroupClearBits(psq4_system()->event_group, initializingEventBit);
            ESP_LOGI(
                PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
                "Successfully connected to %s:%d",
                mqtt_host_address,
                mqtt_host_port
            );
        }
    } while (SUCCESS != rc);
}


void psq4_mqtt_publish(
    const char *topic,
    enum QoS qos,
    const char *payload
) {
    IoT_Publish_Message_Params params;
    params.payload = (void *) payload;
    params.payloadLen = strlen(payload);
    params.qos = qos;
    params.isRetained = 0;
    uint32_t attempt_num = 1;
    IoT_Error_t rc = FAILURE;
    while (SUCCESS != rc) {
        xEventGroupWaitBits(
            psq4_system()->event_group,
            PSQ4_MQTT_CONNECTED_BIT,
            false,
            true,
            portMAX_DELAY
        );
        rc = aws_iot_mqtt_publish(&client, topic, strlen(topic), &params);
        if (SUCCESS != rc) {
            ESP_LOGW(
                PSQ4_AWS_IOT_MQTT_CLIENT_TAG,
                "MQTT publish to topic %s failed on attempt #%d with IoT Error %d",
                topic,
                attempt_num++,
                rc
            );
            xEventGroupSetBits(
                psq4_system()->event_group,
                PSQ4_MQTT_PUBLISH_FAILURE_BIT
            );
        } else {
            xEventGroupClearBits(
                psq4_system()->event_group,
                PSQ4_MQTT_PUBLISH_FAILURE_BIT
            );
        }
    }
}
