/*
 * Copyright (c) 2018 PhotonicFusion LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * @file        read_blocking.ino
 * @summary     Blocking read example.
 * @version     1.0
 * @author      nitacku
 * @data        27 May 2018
 */

#include <nEEPROM.h>

enum EEPROM : uint32_t
{
    PAGE_SIZE = 64,
    PAGE_COUNT = 512,
};

// Initialize for an EEPROM with an I2C address of 0x50, page size of 64 bytes,
// and with 512 total pages. This is valid for a 32768 byte EEPROM.
CEEPROM g_eeprom{CEEPROM::Address::A0, EEPROM::PAGE_SIZE, EEPROM::PAGE_COUNT}; // 32768 (64 * 512) bytes


void setup(void)
{
    // Initialize EEPROM
    g_eeprom.Initialize();
    
    // Initialize Serial communication
    Serial.begin(9600);
}

// Prints buffer in HEX format columns
void PrintBuffer(const uint8_t* buffer, const uint32_t length)
{
    const uint8_t column_width = 10;
    const char* format_string = PSTR("%02X ");

    // Print data received
    for (uint8_t index = 0; index < length; index++)
    {
        char hex[6];
        
        if ((index % column_width) == 0)
        {
            Serial.println();
        }

        // Convert value to hexadecimal format
        snprintf_P(hex, 4, format_string, buffer[index]);
        Serial.print(hex);
    }

    Serial.println();
}


void loop(void)
{
    const uint16_t start_address = 0x0100;
    uint8_t rx_buffer[50];

    // Read EEPROM starting at address 0x100 into rx_buffer
    uint8_t status = g_eeprom.Read(start_address, rx_buffer, sizeof(rx_buffer));
    
    // Check if any errors occurred
    if (!status)
    {
        PrintBuffer(rx_buffer, sizeof(rx_buffer));
    }
    else
    {
        Serial.print("Communication Error: ");
        Serial.println(status);
    }

    delay(500);
}
