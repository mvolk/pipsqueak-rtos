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
