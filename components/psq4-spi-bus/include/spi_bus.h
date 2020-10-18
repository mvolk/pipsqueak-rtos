/*
 * Pipsqueak v4 SPI bus component
 *
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
 */

#ifndef PSQ4_SPI_BUS_H
#define PSQ4_SPI_BUS_H

#include <freertos/FreeRTOS.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

/**
 * @brief Initializes the SPI bus.
 *
 * Failure is fatal and results in a software restart. If
 * this method returns, it succeeded.
 *
 * @param bus_cfg The bus configuration that this function
 *        will populate.
 * @param miso IO pin carrying SPI MISO signal
 * @param mosi IO pin carrying SPI MOSI signal
 * @param clk IO pin carrying SPI clock signal
 * @param host SPI Host (HSPI or VSPI)
 * @param dma_channel 1 or 2
 * @param max_transfer_size_bytes The largest payload that
 *        will need to be transmitted over this bus.
 */
void spi_bus_init(
    spi_bus_config_t * bus_cfg,
    gpio_num_t miso,
    gpio_num_t mosi,
    gpio_num_t clk,
    spi_host_device_t host,
    int dma_channel,
    size_t max_transfer_size_bytes
);

#endif // PSQ4_SPI_BUS_H
