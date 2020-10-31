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
