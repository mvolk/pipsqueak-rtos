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

#ifndef PSQ4_CONSTANTS_H
#define PSQ4_CONSTANTS_H

#include <freertos/FreeRTOS.h>


// Code-level constants, supplementary to sdkconfig

// See https://www.esp32.com/viewtopic.php?t=1442#p6613
#define PSQ4_SPI_MAX_TRANS_SIZE_BYTES 4096

#define PSQ4_WIFI_INITIALIZING_BIT            BIT0
#define PSQ4_WIFI_CONNECTED_BIT               BIT1
#define PSQ4_CLOCK_INITIALIZING_BIT           BIT2
#define PSQ4_CLOCK_READY_BIT                  BIT3
#define PSQ4_CLOCK_BATTERY_DEAD_BIT           BIT4
#define PSQ4_MQTT_INITIALIZING_BIT            BIT5
#define PSQ4_MQTT_CONNECTED_BIT               BIT6
#define PSQ4_MQTT_SUBSCRIBED_BIT              BIT7
#define PSQ4_THERMO_BOARD_INITIALIZING_BIT    BIT8
#define PSQ4_THERMO_BOARD_OK_BIT              BIT9
#define PSQ4_THERMO_MEDIUM_INITIALIZING_BIT   BIT10
#define PSQ4_THERMO_MEDIUM_OK_BIT             BIT11
#define PSQ4_MQTT_PUBLISH_FAILURE_BIT         BIT12


#endif // PSQ4_CONSTANTS_H
