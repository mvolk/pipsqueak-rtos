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
 * Derived the MIT-licensed work of David Antliff
 * https://github.com/DavidAntliff/esp32-ds18b20-example
 * Original license reproduced above; original copyright reproduced below:
 * Copyright (c) 2017 David Antliff
 */

#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/rmt.h"
#include "ds18b20.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    const char * name;
    int oneWireGPIO;
    rmt_channel_t tx_channel;
    rmt_channel_t rx_channel;
    DS18B20_RESOLUTION resolution;
    QueueHandle_t queue;
} TemperatureSensor_t;

void senseTemperature(void * pvParameters);

#ifdef __cplusplus
}
#endif

#endif // TEMPERATURE_SENSOR_H
