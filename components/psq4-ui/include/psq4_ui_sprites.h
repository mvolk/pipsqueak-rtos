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

#ifndef PSQ4_UI_SPRITES_H
#define PSQ4_UI_SPRITES_H

#include <freertos/FreeRTOS.h>
#include "psq4_gfx.h"


#ifdef __cplusplus
extern "C" {
#endif


// WiFi icons, 21x16px
const psq4_gfx_sprite_t psq4_ui_sprite_wifi_connecting_1;
const psq4_gfx_sprite_t psq4_ui_sprite_wifi_connecting_2;
const psq4_gfx_sprite_t psq4_ui_sprite_wifi_connecting_3;
const psq4_gfx_sprite_t psq4_ui_sprite_wifi_ok;
const psq4_gfx_sprite_t psq4_ui_sprite_wifi_fail;

const psq4_gfx_sprite_t psq4_ui_sprite_mqtt_connecting;
const psq4_gfx_sprite_t psq4_ui_sprite_mqtt_fail;
const psq4_gfx_sprite_t psq4_ui_sprite_mqtt_ok;

const psq4_gfx_sprite_t psq4_ui_sprite_battery_dead_on;
const psq4_gfx_sprite_t psq4_ui_sprite_battery_dead_off;
const psq4_gfx_sprite_t psq4_ui_sprite_battery_ok;


#ifdef __cplusplus
}
#endif

#endif // PSQ4_UI_SPRITES_H
