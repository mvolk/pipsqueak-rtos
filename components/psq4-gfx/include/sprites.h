#ifndef GFX_SPRITES_H
#define GFX_SPRITES_H

#include <freertos/FreeRTOS.h>
#include "gfx.h"

// TODO switch to using uint16_t as the array type

// WiFi icon, green, 21x16px, signalling a healthy WiFi connection
const uint8_t gfx_sprite_wifi_ok_data[672];
const gfx_dim_t gfx_sprite_wifi_ok_dim;


// WiFi icon, red, 21x16px, signalling an unhealthy WiFi connection
const uint8_t gfx_sprite_wifi_fail_data[672];
const gfx_dim_t gfx_sprite_wifi_fail_dim;


#endif // GFX_SPRITES_H
