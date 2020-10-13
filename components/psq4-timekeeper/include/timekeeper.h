/*
 * Periodically synchronizes the system time to a peripheral DS3231 real time
 * clock.
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
 * Copyright (c) 2020 Michael Volk
 */

#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A task that maintains the system clock.
 *
 * Immediately sets the system timezone, synchronizes the ESP's RTC to the
 * time more precisely and persistently maintained by an external DS3231 RTC,
 * checks to see if the external RTC's battery might have died and sets up
 * syncing of both the system RTC and external RTC with NTP.
 *
 * Infrequently updates the system time to match the external RTC time.
 *
 * @param ds3231 DS3231 device descriptor
 */
void timekeeper_task(void * pvParameters);

/**
 * @brief Returns the current unix timestamp.
 * @return The number of seconds since Jan 1 1970 at midnight UTC
 */
time_t unix_timestamp();

#ifdef __cplusplus
}
#endif

#endif // TIMEKEEPER_H
