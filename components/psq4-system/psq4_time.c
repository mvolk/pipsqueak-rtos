/*
 * MIT License
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
 * Copyright (c) 2020 Michael Volk
 */

#include "psq4_time.h"
#include <string.h>
#include <freertos/task.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_sntp.h>
#include <ds3231.h>
#include "psq4_constants.h"


// We read to whole second resolution, meaning that at the moment we get
// "48 seconds" (for example) as the second of the minute, it is at least
// 48 but could be as much as 48.9999. Assuming the processing time is
// negligible, if we guess that the actual time is Ts.5, where "Ts" is
// the whole number of seconds we read, then we should be off by no more
// than half a second plus or minus between the two clocks.
#define USEC_GUESS 500000

#define ONE_MINUTE 60 // in seconds
#define ONE_HOUR 3600 // in seconds
#define MS_FACTOR 1000.0

#define SET_ENV_OVERWRITE 1


static const char * PSQ4_TIME_TAG = "psq4-time";
static DS3231_Info psq4_time_ds3231;
static time_t psq4_time_last_sync_from_sntp = 0;
static time_t psq4_time_last_sync_from_rtc = 0;
static time_t psq4_time_last_sync_of_rtc = 0;
static bool psq4_time_rtc_unreliable = false;
static EventGroupHandle_t event_group;


time_t psq4_time_now() {
  time_t now;
  time(&now);
  return now;
}


static void psq4_time_sync_from_external_rtc()
{
    struct tm time;
    struct timezone tz = { 0, 0 };
    ds3231_get_time(&psq4_time_ds3231, &time);
    time_t now = mktime(&time);
    struct timeval tv = { now, USEC_GUESS };
    settimeofday(&tv, &tz);
    psq4_time_last_sync_from_rtc = now;
    ESP_LOGI(
        PSQ4_TIME_TAG,
        "Internal clock synced to external RTC; unix_timestamp=%ld",
        now
    );
}


static void psq4_time_sync_to_external_rtc()
{
    struct tm time;
    time_t now = psq4_time_now();
    memcpy(&time, localtime(&now), sizeof(struct tm));
    ds3231_set_time(&psq4_time_ds3231, &time);
    psq4_time_last_sync_of_rtc = now;
    // We can now clear the stop flag, as we have a good value again
    if (psq4_time_rtc_unreliable) {
        ds3231_clear_oscillator_stop_flag(&psq4_time_ds3231);
        psq4_time_rtc_unreliable = false;
    }
    ESP_LOGI(
        PSQ4_TIME_TAG,
        "External RTC synced to internal clock; unix_timestamp=%ld",
        now
    );
}


static void psq4_time_sntp_sync_notification_cb(struct timeval *tv)
{
    psq4_time_last_sync_from_sntp = psq4_time_now();
    ESP_LOGI(
        PSQ4_TIME_TAG,
        "Internal clock synchronized via SNTP; unix_timestamp=%ld; tv_sec=%ld",
        psq4_time_last_sync_from_sntp,
        tv->tv_sec
    );
    xEventGroupSetBits(event_group, PSQ4_CLOCK_READY_BIT);
}


static void psq4_time_set_up_sntp()
{
#ifdef CONFIG_PSQ4_USE_SNTP
    if (!CONFIG_PSQ4_USE_SNTP) return;
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(psq4_time_sntp_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
#endif
}


void psq4_time_task(void *ignored)
{
    // Initialize the RTC interface
    ds3231_init_info(
        &psq4_time_ds3231,
        I2C_NUM_0,
        CONFIG_PSQ4_I2C_SDA_GPIO,
        CONFIG_PSQ4_I2C_SCL_GPIO,
        1000
    );

    // Set system timezone to UTC
    setenv("TZ", "UTC+0", SET_ENV_OVERWRITE);
    tzset();

    // Check to see if the external RTC shut down unexpectedly
    ds3231_get_oscillator_stop_flag(&psq4_time_ds3231, &psq4_time_rtc_unreliable);
    if (psq4_time_rtc_unreliable) {
        xEventGroupSetBits(event_group, PSQ4_CLOCK_BATTERY_DEAD_BIT);
        ESP_LOGE(
            PSQ4_TIME_TAG,
            "DS3231 RTC oscillator stopped, which means that the DS3231's battery is likely dead"
        );
    } else {
        // Set the internal clock using the external RTC
        psq4_time_sync_from_external_rtc();
        xEventGroupSetBits(event_group, PSQ4_CLOCK_READY_BIT);
    }

    // Use SNTP to correct clock drift
    psq4_time_set_up_sntp();

    // We'll be maintaining time via SNTP with our external RTC for backup.
    // If we recently got time via SNTP, use that to keep the external RTC's
    // small but present drift in check.
    // If we haven't recently had an SNTP update, and the RTC's clock battery
    // either hasn't failed or has failed but the clock has since been set,
    // then use the RTC to correct the more significant drift of the system's
    // clock to reduce drift if the device cannot obtain an SNTP connection
    // over a long period of time.
    while(true) {
        vTaskDelay((ONE_MINUTE * MS_FACTOR) / portTICK_PERIOD_MS);
        time_t now = psq4_time_now();
        time_t since_sync_from_sntp = now - psq4_time_last_sync_from_sntp;
        time_t since_sync_from_rtc = now - psq4_time_last_sync_from_rtc;
        time_t since_sync_of_rtc = now - psq4_time_last_sync_of_rtc;
        // Sync the RTC to SNTP if SNTP is working
        if (since_sync_from_sntp < (5 * ONE_MINUTE) && since_sync_of_rtc > ONE_HOUR) {
            psq4_time_sync_to_external_rtc();
        }
        // Sync from RTC if SNTP is not working and RTC is usable
        if (
            since_sync_from_sntp > (2 * ONE_HOUR) &&
            since_sync_from_rtc > ONE_HOUR &&
            !psq4_time_rtc_unreliable
           )
        {
            psq4_time_sync_from_external_rtc();
        }
    }
}

void psq4_time_init(EventGroupHandle_t system_event_group)
{
   event_group = system_event_group;
    xTaskCreate(&psq4_time_task, "timeTask", 2048, system, 5, NULL);
}
