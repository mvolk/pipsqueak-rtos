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

#include "psq4_spi_bus.h"
#include <esp_log.h>

void psq4_spi_bus_init(
    spi_bus_config_t * bus_cfg,
    gpio_num_t miso,
    gpio_num_t mosi,
    gpio_num_t clk,
    spi_host_device_t host,
    int dma_channel,
    size_t max_transfer_size_bytes)
{
    bus_cfg->miso_io_num = miso;
    bus_cfg->mosi_io_num = mosi;
    bus_cfg->sclk_io_num = clk;
    bus_cfg->quadwp_io_num = -1;
    bus_cfg->quadhd_io_num = -1;
    bus_cfg->max_transfer_sz = max_transfer_size_bytes;
    esp_err_t ret = spi_bus_initialize(
        host,
        bus_cfg,
        dma_channel
    );
    ESP_ERROR_CHECK(ret);
}
