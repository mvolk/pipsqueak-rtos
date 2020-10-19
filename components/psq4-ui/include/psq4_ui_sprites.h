#ifndef PSQ4_UI_SPRITES_H
#define PSQ4_UI_SPRITES_H

#include <freertos/FreeRTOS.h>
#include "psq4_gfx.h"


#ifdef __cplusplus
extern "C" {
#endif


// WiFi icon, green, 21x16px, signalling a healthy WiFi connection
const psq4_gfx_sprite_t psq4_ui_sprite_wifi_ok;


// WiFi icon, red, 21x16px, signalling an unhealthy WiFi connection
const psq4_gfx_sprite_t psq4_ui_sprite_wifi_fail;


#ifdef __cplusplus
}
#endif

#endif // PSQ4_UI_SPRITES_H
