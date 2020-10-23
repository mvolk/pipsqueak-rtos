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

#ifndef PSQ4_TIME_H
#define PSQ4_TIME_H

#include <time.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Returns the current unix timestamp.
 *
 * @return The number of seconds since Jan 1 1970 at midnight UTC
 */
time_t psq4_time_now();


/**
 * @brief Initialized the system timekeeping mechanisms.
 *
 * @param system_event_group
 */
void psq4_time_init(EventGroupHandle_t system_event_group);


#ifdef __cplusplus
}
#endif

#endif // PSQ4_TIME_H
