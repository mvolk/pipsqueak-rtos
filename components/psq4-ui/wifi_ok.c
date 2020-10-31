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

#include "psq4_ui_sprites.h"
#include "psq4_gfx.h"

static const psq4_gfx_dim_t dim = { 21, 16 };

static const uint8_t data[672] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x85, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x74, 0xAD, 0x43, 0xE5, 0x74, 0xAD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x64, 0x6A, 0x43, 0xE5, 0x43, 0xE5, 0x3B, 0xE5, 0x64, 0x6A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x8D, 0x30, 0x4C, 0x06, 0x43, 0xE5, 0x4C, 0x07, 0x8D, 0x30, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x8D, 0x90, 0xD6, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xD6, 0xFA, 0x8D, 0x90, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x85, 0x6F, 0x5C, 0xC8, 0x5C, 0xC8, 0x95, 0xB1, 0xDF, 0x3B, 0xFF, 0xFF, 0xDF, 0x3B, 0x95, 0xB1, 0x5C, 0xC8, 0x5C, 0xC8, 0x85, 0x4E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x75, 0x2C, 0x5C, 0xC8, 0x5C, 0xC8, 0x5C, 0xA8, 0x5C, 0xC8, 0x5C, 0xA8, 0x5C, 0xC8, 0x5C, 0xC8, 0x5C, 0xC9, 0x5C, 0xC8, 0x5C, 0xA8, 0x5C, 0xC8, 0x75, 0x2C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7D, 0x2D, 0x5C, 0xC8, 0x5C, 0xC8, 0x5C, 0xC8, 0x5C, 0xC8, 0x5C, 0xC8, 0x5C, 0xC8, 0x5C, 0xC8, 0x5C, 0xC8, 0x5C, 0xC8, 0x5C, 0xA8, 0x5C, 0xC8, 0x7D, 0x4D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0x95, 0xD0, 0xE7, 0x5C, 0xFF, 0xFF, 0xD7, 0x1A, 0x6C, 0xEA, 0x5C, 0xC8, 0x5C, 0xC8, 0x5C, 0xC9, 0x5C, 0xC8, 0x5C, 0xA8, 0x5C, 0xA8, 0x5C, 0xC8, 0x6C, 0xEB, 0xD7, 0x1A, 0xFF, 0xFF, 0xE7, 0x5C, 0x95, 0xD0, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x95, 0xD0, 0x65, 0x2A, 0x6D, 0x2A, 0xCE, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0x5C, 0x9D, 0xD2, 0x7D, 0x4E, 0x75, 0x2D, 0x7D, 0x4E, 0x9D, 0xD2, 0xE7, 0x7C, 0xFF, 0xFF, 0xFF, 0xFF, 0xCE, 0xF9, 0x65, 0x2A, 0x65, 0x2A, 0x95, 0xD0, 0xFF, 0xFF,
    0x7D, 0x6D, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x6D, 0x2A, 0x7D, 0x6D, 0xD7, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xD7, 0x19, 0x7D, 0x6D, 0x65, 0x2A, 0x65, 0x2A, 0x6D, 0x2A, 0x6D, 0x2A, 0x7D, 0x6D,
    0x95, 0xD0, 0x65, 0x2A, 0x6D, 0x2A, 0x6D, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x6D, 0x2A, 0x8D, 0xB0, 0xA6, 0x34, 0xB6, 0x75, 0xA6, 0x34, 0x8D, 0xB0, 0x6D, 0x2B, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x95, 0xD0,
    0xFF, 0xFF, 0xCE, 0xF9, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x6D, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x6D, 0x2A, 0x6D, 0x2A, 0x6D, 0x2A, 0x6D, 0x2A, 0x6D, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x6D, 0x2A, 0xCE, 0xF9, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xBE, 0xA6, 0x13, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x6D, 0x2A, 0x6D, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x6D, 0x2A, 0x65, 0x2A, 0x6D, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0xA6, 0x13, 0xF7, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xBE, 0xB6, 0x75, 0x85, 0x8E, 0x65, 0x2A, 0x65, 0x2A, 0x6D, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x65, 0x2A, 0x6D, 0x2A, 0x85, 0x8E, 0xB6, 0x75, 0xF7, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xBE, 0xDF, 0x5B, 0xD7, 0x1A, 0xCE, 0xF9, 0xD7, 0x1A, 0xDF, 0x5B, 0xF7, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

const psq4_gfx_sprite_t psq4_ui_sprite_wifi_ok = { (uint16_t *) data, dim };
