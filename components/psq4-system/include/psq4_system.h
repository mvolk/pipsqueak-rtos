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

#ifndef PSQ4_SYSTEM_H
#define PSQ4_SYSTEM_H

#include <time.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <driver/spi_master.h>
#include <esp_err.h>


typedef struct {
    spi_bus_config_t spi_bus_cfg;
    EventGroupHandle_t event_group;
} psq4_system_t;


typedef psq4_system_t* psq4_system_handle_t;


#ifdef __cplusplus
extern "C" {
#endif


/** @brief Initialize system functions */
psq4_system_handle_t psq4_system_init();


/** @brief Obtain a reference to the system state struct instance */
psq4_system_handle_t psq4_system();


/** @brief Wait for WiFi to be available */
esp_err_t psq4_system_await_wifi(TickType_t xTicksToWait);


/** @brief Wait for clock to be reliable */
esp_err_t  psq4_system_await_clock(TickType_t xTicksToWait);


/** @brief Return the current unix epoch time */
time_t psq4_system_time();


#ifdef __cplusplus
}
#endif

#endif // PSQ4_SYSTEM_H
