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

#include "psq4_wifi.h"

#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <esp_event.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include "psq4_constants.h"


static const char *PSQ4_WIFI_TAG = "psq4_system/wifi";
static uint32_t psq4_wifi_connect_retry = 0;


static void event_handler(
    void *user_data,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event)
{
    EventGroupHandle_t system_event_group = (EventGroupHandle_t) user_data;
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_STA_START) {
            esp_wifi_connect();
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            xEventGroupClearBits(system_event_group, PSQ4_WIFI_CONNECTED_BIT);
            xEventGroupClearBits(system_event_group, PSQ4_WIFI_INITIALIZING_BIT);
            esp_wifi_connect();
            ESP_LOGI(PSQ4_WIFI_TAG, "WiFi disconnected, attempting to connect");
        }
    } else if (event_base == IP_EVENT) {
        if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
            xEventGroupSetBits(system_event_group, PSQ4_WIFI_CONNECTED_BIT);
            xEventGroupClearBits(system_event_group, PSQ4_WIFI_INITIALIZING_BIT);
            ESP_LOGI(PSQ4_WIFI_TAG, "WiFi connection established in station mode");
            psq4_wifi_connect_retry = 0;
        }
    }
}


void psq4_wifi_init(EventGroupHandle_t system_event_group)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &event_handler,
        system_event_group
    ));
    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        system_event_group
    ));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_PSQ4_WIFI_SSID,
            .password = CONFIG_PSQ4_WIFI_PASSWORD,
	        .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(PSQ4_WIFI_TAG, "WiFi initialization complete, connection sequence in progress");
}
