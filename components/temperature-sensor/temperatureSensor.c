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
 *
 * Derived the MIT-licensed work of David Antliff
 * https://github.com/DavidAntliff/esp32-ds18b20-example
 * Original license reproduced above; original copyright reproduced below:
 * Copyright (c) 2017 David Antliff
 */

#include "temperatureSensor.h"
#include "owb.h"
#include "owb_rmt.h"
#include "freertos/task.h"

void senseTemperature(void * pvParameters) {
    TemperatureSensor_t * sensor = (TemperatureSensor_t *) pvParameters;

    // Stable readings require a brief period before communication
    vTaskDelay(2000.0 / portTICK_PERIOD_MS);

    // Create a 1-Wire bus, using the RMT timeslot driver
    OneWireBus * owb;
    owb_rmt_driver_info rmt_driver_info;
    owb = owb_rmt_initialize(&rmt_driver_info, sensor->oneWireGPIO, sensor->tx_channel, sensor->rx_channel);
    owb_use_crc(owb, true);  // enable CRC check for ROM code

    // Find connected device
    printf("Find %s device:\n", sensor->name);
    // OneWireBus_ROMCode device_rom_code;
    int num_devices = 0;
    OneWireBus_SearchState search_state = {0};
    bool found = false;
    owb_search_first(owb, &search_state, &found);
    while (found)
    {
        char rom_code_s[17];
        owb_string_from_rom_code(search_state.rom_code, rom_code_s, sizeof(rom_code_s));
        printf("  %d : %s\n", num_devices, rom_code_s);
        // if (num_devices == 0) {
        //     device_rom_code = search_state.rom_code;
        // }
        ++num_devices;
        owb_search_next(owb, &search_state, &found);
    }
    printf("Found %d %s device%s\n", num_devices, sensor->name, num_devices == 1 ? "" : "s");

    // For a single device only:
    OneWireBus_ROMCode rom_code;
    owb_status status = owb_read_rom(owb, &rom_code);
    if (status == OWB_STATUS_OK)
    {
        char rom_code_s[OWB_ROM_CODE_STRING_LENGTH];
        owb_string_from_rom_code(rom_code, rom_code_s, sizeof(rom_code_s));
        printf("Single %s device %s present\n", sensor->name, rom_code_s);
    }
    else
    {
        printf("An error occurred reading ROM code for %s: %d\n", sensor->name, status);
    }

    DS18B20_Info * device = ds18b20_malloc();
    printf("Single device optimisations enabled for %s\n", sensor->name);
    ds18b20_init_solo(device, owb);
    ds18b20_use_crc(device, true);
    ds18b20_set_resolution(device, sensor->resolution);

    int error_count = 0;
    int sample_count = 0;
    while (true) {
        ds18b20_convert_all(owb);

        // In this application all devices use the same resolution,
        // so use the first device to determine the delay
        ds18b20_wait_for_conversion(device);

        // Read the results immediately after conversion otherwise it may fail
        // (using printf before reading may take too long)
        float reading;
        if (ds18b20_read_temp(device, &reading) != DS18B20_OK) {
            ++error_count;
        }

        // Print results in a separate loop, after all have been read
        printf("%s sample %d: %.3f C (%d errors)\n", sensor->name, ++sample_count, reading, error_count);

        if (xQueueSend(sensor->queue, &reading, 250.0 / portTICK_PERIOD_MS) != pdTRUE) {
            printf("FAILED to enqueue reading for %s\n", sensor->name);
        }
    }
}
