/*
 * Stripped-down ESP32-specific ESP-IDF driver for DS3231 high precision RTC module
 *
 * Amended from https://github.com/UncleRus/esp-idf-lib/blob/master/components/ds3231,
 * which in turn was ported from esp-open-rtos, and from
 * https://github.com/UncleRus/esp-idf-lib/blob/master/components/i2cdev
 *
 * In broad brush strokes, amendments include:
 * - Removal of alarm, square wave generator, 32kHz oscillator features
 * - Removal of features permitting reading of the internal temperature
 * - Removes support for platforms other than ESP32
 * - Removes thread safety and rests directly on the ESP-IDF I2C library. If multiple tasks
 *   are to exercise a single I2C port, external synchronization (mutex/semaphore) is
 *   required.
 * - Removes some runtime argument checking
 *
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
 *
 * Copyright (C) 2015 Richard A Burton <richardaburton@gmail.com>
 * Copyright (C) 2016 Bhuvanchandra DV <bhuvanchandra.dv@gmail.com>
 * Copyright (C) 2018 Ruslan V. Uss <unclerus@gmail.com
 * Copyright (C) 2018 Ruslan V. Uss <https://github.com/UncleRus>
 * Copyright (C) 2020 Michael Volk <michael-volk@hotmail.com>
 */

#ifndef DS3231_H
#define DS3231_H

#include <esp_err.h>
#include <driver/i2c.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_FREQ_HZ               400000

#define DS3231_STAT_OSCILLATOR    0x80

#define DS3231_ADDR_TIME          0x00
#define DS3231_ADDR_CONTROL       0x0e
#define DS3231_ADDR_STATUS        0x0f

#define DS3231_12HOUR_FLAG        0x40
#define DS3231_12HOUR_MASK        0x1f
#define DS3231_PM_FLAG            0x20
#define DS3231_MONTH_MASK         0x1f

enum {
    DS3231_SET = 0,
    DS3231_CLEAR,
    DS3231_REPLACE
};

typedef struct {
    i2c_port_t port;
    i2c_config_t cfg;
    uint8_t addr;
    uint32_t timeoutMs;
} DS3231_Info;

/**
 * @brief Initialize device descriptor
 * @param ds3231 DS3231 device descriptor
 * @param port I2C port - I2C_NUM_0 or I2C_NUM_1
 * @param sda_gpio SDA GPIO
 * @param scl_gpio SCL GPIO
 * @param timeoutMS timeout for message transmissions
 */
void ds3231_init_info(DS3231_Info *ds3231, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio, uint32_t timeoutMs);

/**
 * @brief Set the time on the RTC
 *
 * Timezone agnostic, pass whatever you like.
 * I suggest using GMT and applying timezone and DST when read back.
 *
 * @return ESP_OK to indicate success
 */
esp_err_t ds3231_set_time(DS3231_Info *ds3231, struct tm *time);

/**
 * @brief Get the time from the RTC, populates a supplied tm struct
 * @param dev Device descriptor
 * @param[out] time RTC time
 * @return ESP_OK to indicate success
 */
esp_err_t ds3231_get_time(DS3231_Info *ds3231, struct tm *time);

/**
 * @brief Check if oscillator has previously stopped
 *
 * E.g. no power/battery or disabled
 * sets flag to true if there has been a stop
 *
 * @param dev Device descriptor
 * @param[out] flag Stop flag
 * @return ESP_OK to indicate success
 */
esp_err_t ds3231_get_oscillator_stop_flag(DS3231_Info *ds3231, bool *flag);

/**
 * @brief Clear the oscillator stopped flag
 * @param dev Device descriptor
 * @return ESP_OK to indicate success
 */
esp_err_t ds3231_clear_oscillator_stop_flag(DS3231_Info *ds3231);

#ifdef __cplusplus
}
#endif

#endif // DS3231_H
