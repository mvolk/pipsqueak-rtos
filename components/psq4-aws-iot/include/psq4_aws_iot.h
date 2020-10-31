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

#ifndef PSQ4_AWS_IOT_H
#define PSQ4_AWS_IOT_H

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include <aws_iot_mqtt_client_interface.h>

#ifdef __cplusplus
extern "C" {
#endif


void psq4_aws_iot_init();


// Note that this blocks until the subscription is created
void psq4_mqtt_subscribe(
    const char *topic,
    enum QoS qos,
    pApplicationHandler_t handler,
    EventBits_t initializingEventBit,
    EventBits_t connectedEventBit
);


// Note that this blocks until the message is published
void psq4_mqtt_publish(
    const char *topic,
    enum QoS qos,
    const char *payload
);


#ifdef __cplusplus
}
#endif

#endif // PSQ4_AWS_IOT_H
