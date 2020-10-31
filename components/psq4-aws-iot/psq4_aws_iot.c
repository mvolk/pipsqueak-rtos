#include "psq4_aws_iot.h"
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include <aws_iot_version.h>


extern void psq4_mqtt_init();

static const char *PSQ4_AWS_IOT_TAG = "psq4-aws-iot";

static void log_sdk_version() {
    if (strcmp(VERSION_TAG, "") == 0) {
        ESP_LOGI(
            PSQ4_AWS_IOT_TAG,
            "AWS IoT Embedded SDK version %d.%d.%d",
            VERSION_MAJOR,
            VERSION_MINOR,
            VERSION_PATCH
        );
    } else {
        ESP_LOGI(
            PSQ4_AWS_IOT_TAG,
            "AWS IoT Embedded SDK version %d.%d.%d-%s",
            VERSION_MAJOR,
            VERSION_MINOR,
            VERSION_PATCH,
            VERSION_TAG
        );
    }
}

void psq4_aws_iot_init() {
    log_sdk_version();
    psq4_mqtt_init();
}
