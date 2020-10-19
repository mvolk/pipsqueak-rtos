#include "sprites.h"
#include "gfx.h"

const gfx_dim_t gfx_sprite_wifi_fail_dim = { 21, 16 };

const uint8_t gfx_sprite_wifi_fail_data[672] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5, 0x54, 0xDA, 0x06, 0xED, 0x13, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0xDA, 0x88, 0xD9, 0x64, 0xDA, 0x67, 0xFF, 0x5D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xED, 0x13, 0xD9, 0xE6, 0xEC, 0xD2, 0xFF, 0xDE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xBA, 0xFF, 0x9E, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0x5D, 0xFF, 0xDE, 0xFF, 0xFF, 0xFF, 0x9E, 0xFE, 0xBA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF6, 0x79, 0xF5, 0xD6, 0xF5, 0xD6, 0xFF, 0xDF, 0xFF, 0xFF, 0xD9, 0x84, 0xFF, 0xFF, 0xFF, 0xDF, 0xF5, 0xB6, 0xF5, 0xD6, 0xF6, 0x79, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF6, 0x38, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xD6, 0xFF, 0xFF, 0xFF, 0x9E, 0xD9, 0x63, 0xFF, 0x9E, 0xFF, 0xFF, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xD6, 0xF6, 0x38, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF6, 0x79, 0xF5, 0xB6, 0xF5, 0xD6, 0xF5, 0xD6, 0xFF, 0xFF, 0xF6, 0x38, 0xD1, 0x84, 0xF6, 0x38, 0xFF, 0xFF, 0xF5, 0xD6, 0xF5, 0xB6, 0xF5, 0xD6, 0xF6, 0x79, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xF6, 0x79, 0xFF, 0xBE, 0xFF, 0xFF, 0xFF, 0x9E, 0xF6, 0x17, 0xF5, 0xD6, 0xFF, 0xFF, 0xEC, 0xF3, 0xD9, 0x84, 0xEC, 0xF3, 0xFF, 0xFF, 0xF5, 0xD6, 0xF6, 0x17, 0xFF, 0x9E, 0xFF, 0xFF, 0xFF, 0xBE, 0xF6, 0x79, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xF6, 0x79, 0xF5, 0xD6, 0xF5, 0xD6, 0xFF, 0x7D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0xFF, 0xFF, 0xE3, 0xCE, 0xD9, 0x84, 0xE3, 0xCE, 0xFF, 0xFF, 0xFF, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x5D, 0xF5, 0xD6, 0xF5, 0xD6, 0xF6, 0x79, 0xFF, 0xFF,
    0xF6, 0x18, 0xF5, 0xD6, 0xF5, 0xB6, 0xF5, 0xD6, 0xF5, 0xD6, 0xF6, 0x38, 0xFF, 0x7D, 0xFF, 0xFF, 0xFF, 0xFF, 0xE3, 0x0B, 0xD9, 0x63, 0xE3, 0x0A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7D, 0xF6, 0x38, 0xF5, 0xB6, 0xF5, 0xB6, 0xF5, 0xD6, 0xF5, 0xB6, 0xF6, 0x18,
    0xF6, 0x99, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xF7, 0xFF, 0xFF, 0xDA, 0x68, 0xD9, 0x64, 0xDA, 0x68, 0xFF, 0xFF, 0xF5, 0xF7, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xB6, 0xF6, 0x79,
    0xFF, 0xFF, 0xFF, 0x5D, 0xF5, 0xD6, 0xF5, 0xB6, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xB6, 0xF6, 0x38, 0xFF, 0xFF, 0xD9, 0xE6, 0xD9, 0x84, 0xD9, 0xE6, 0xFF, 0xFF, 0xF6, 0x38, 0xF5, 0xB6, 0xF5, 0xD6, 0xF5, 0xB6, 0xF5, 0xD6, 0xF5, 0xD6, 0xFF, 0x5D, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFE, 0xBA, 0xF5, 0xD6, 0xF5, 0xD6, 0xF5, 0xD6, 0xF6, 0x99, 0xFF, 0xFF, 0xD1, 0x64, 0xD9, 0x64, 0xD1, 0x84, 0xFF, 0xFF, 0xF6, 0x99, 0xF5, 0xD6, 0xF5, 0xB6, 0xF5, 0xD6, 0xF6, 0xBA, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFE, 0xFB, 0xF6, 0x38, 0xFE, 0xDB, 0xFF, 0xFF, 0xE2, 0xC9, 0xDA, 0x26, 0xE2, 0xC9, 0xFF, 0xFF, 0xFE, 0xDB, 0xF6, 0x38, 0xFE, 0xFB, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE4, 0x50, 0xDB, 0x0B, 0xE4, 0x50, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
