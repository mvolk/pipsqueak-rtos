#include "timekeeper.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include <time.h>
#include <esp_log.h>
#include <esp_sntp.h>
#include <ds3231.h>

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

const char * TAG = "timekeeper";

static time_t last_sync_from_sntp = 0;
static time_t last_sync_from_rtc = 0;
static time_t last_sync_of_rtc = 0;

time_t unix_timestamp() {
  time_t now;
  time(&now);
  return now;
}

void sync_from_external_rtc(DS3231_Info *ds3231) {
    struct tm time;
    struct timezone tz = { 0, 0 };
    ds3231_get_time(ds3231, &time);
    time_t now = mktime(&time);
    struct timeval tv = { now, USEC_GUESS };
    settimeofday(&tv, &tz);
    last_sync_from_rtc = now;
    ESP_LOGD(TAG, "Internal clock synced to external RTC; unix_timestamp=%ld", now);
}

void sync_to_external_rtc(DS3231_Info *ds3231) {
  struct tm time;
  time_t now = unix_timestamp();
  memcpy(&time, localtime(&now), sizeof(struct tm));
  ds3231_set_time(ds3231, &time);
  last_sync_of_rtc = now;
  ESP_LOGD(TAG, "External RTC synced to internal clock; unix_timestamp=%ld", now);
}

void sntp_sync_notification_cb(struct timeval *tv)
{
    last_sync_from_sntp = unix_timestamp();
    ESP_LOGD(TAG, "Internal clock synchronized via SNTP; unix_timestamp=%ld", last_sync_from_sntp);
}

void set_up_sntp() {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(sntp_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}

void timekeeper_task(void *pvParameters) {
    DS3231_Info *ds3231 = (DS3231_Info *)pvParameters;

    // Set system timezone to GMT
    setenv("TZ", "GMT+0", 1);
    tzset();

    // Set the internal clock using the external RTC
    sync_from_external_rtc(ds3231);

    // Check to see if the external RTC shut down unexpectedly
    bool oscillator_stopped;
    ds3231_get_oscillator_stop_flag(ds3231, &oscillator_stopped);
    if (oscillator_stopped) {
        ESP_LOGE(TAG, "DS3231 RTC oscillator stopped, which means that the DS3231's battery is likely dead");
        ds3231_clear_oscillator_stop_flag(ds3231);
    }

    // Use SNTP to correct clock drift
    set_up_sntp();

    // We'll be maintaining time via SNTP with our external RTC for backup.
    // If we recently got time via SNTP, correct the external RTC's (small) drift
    // If we haven't recently had an SNTP update,
    // If we fail to obtain time from
    // NTP, sync the ESP's clock to the external RTC hourly. This reduces clock
    // drift if the device cannot obtain an SNTP connection over a long period of
    // time
    while(true) {
        vTaskDelay((ONE_MINUTE * MS_FACTOR) / portTICK_PERIOD_MS);
        time_t now = unix_timestamp();
        time_t since_sync_from_sntp = now - last_sync_from_sntp;
        time_t since_sync_from_rtc = now - last_sync_from_rtc;
        time_t since_sync_of_rtc = now - last_sync_of_rtc;
        // Sync the RTC to SNTP if SNTP is working
        if (since_sync_from_sntp < (5 * ONE_MINUTE) && since_sync_of_rtc > ONE_HOUR) {
            sync_to_external_rtc(ds3231);
        }
        // Sync from RTC if SNTP isn't working out
        if (since_sync_from_sntp > (2 * ONE_HOUR) && since_sync_from_rtc > ONE_HOUR) {
            sync_from_external_rtc(ds3231);
        }
    }
}
