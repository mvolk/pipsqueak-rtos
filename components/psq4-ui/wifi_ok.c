#include "psq4_ui_sprites.h"
#include "psq4_gfx.h"

static const psq4_gfx_dim_t dim = { 21, 16 };

static const uint8_t data[672] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB6, 0xB5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAE, 0x73, 0x75, 0x8C, 0xAE, 0x73, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x96, 0x31, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8C, 0x96, 0x11, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0xD6, 0x85, 0xAD, 0x75, 0x8C, 0x85, 0xCD, 0xBE, 0xD6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAE, 0x74, 0xE7, 0x7B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0x7B, 0xAE, 0x74, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9E, 0x32, 0x75, 0x8B, 0x75, 0x8C, 0xAE, 0x95, 0xE7, 0x9C, 0xFF, 0xFF, 0xE7, 0x9C, 0xAE, 0x95, 0x75, 0x8C, 0x75, 0x8C, 0x9E, 0x32, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x8D, 0xF0, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8C, 0x8D, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9E, 0x31, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8B, 0x9E, 0x31, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0x9E, 0x32, 0xEF, 0x9C, 0xFF, 0xFF, 0xE7, 0x7C, 0x85, 0xCE, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8C, 0x85, 0xCE, 0xE7, 0x7C, 0xFF, 0xFF, 0xEF, 0x9C, 0x9E, 0x52, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x9E, 0x32, 0x75, 0x8B, 0x75, 0x8C, 0xDF, 0x3A, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xBD, 0xB6, 0xB5, 0x9E, 0x32, 0x96, 0x10, 0x9E, 0x32, 0xB6, 0x95, 0xEF, 0xBD, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0x3A, 0x75, 0x8C, 0x75, 0x8C, 0x9E, 0x32, 0xFF, 0xFF,
    0x85, 0xEF, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8C, 0x8D, 0xEF, 0xDF, 0x5A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0x5A, 0x8D, 0xEF, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8C, 0x8D, 0xEF,
    0xA6, 0x52, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8B, 0x7D, 0xAC, 0x9E, 0x31, 0xB6, 0x95, 0xBE, 0xD7, 0xB6, 0xB5, 0x9E, 0x31, 0x75, 0xAC, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8B, 0xA6, 0x52,
    0xFF, 0xFF, 0xD7, 0x3A, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8C, 0xD7, 0x3A, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xDE, 0xAE, 0x94, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8C, 0xAE, 0x94, 0xF7, 0xDE, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xDE, 0xBE, 0xD6, 0x8D, 0xF0, 0x75, 0x8C, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8B, 0x75, 0x8C, 0x75, 0x8C, 0x8D, 0xF0, 0xBE, 0xD6, 0xF7, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xDE, 0xE7, 0x7C, 0xDF, 0x5B, 0xD7, 0x3A, 0xDF, 0x5B, 0xE7, 0x7C, 0xF7, 0xDE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

const psq4_gfx_sprite_t psq4_ui_sprite_wifi_ok = { (uint16_t *) data, dim };