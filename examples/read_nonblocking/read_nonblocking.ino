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
 * @file        read_nonblocking.ino
 * @summary     Non-blocking read example.
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

// Global buffer for received data
// Could be allocated locally as long as the buffer remains in scope for the
// duration of the read. Easiest guarantee is to simply make global.
uint8_t g_rx_buffer[50];


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

    // Perform non-blocking read
    // RxCallback() will be invoked upon completion of transfer
    // Read EEPROM starting at address 0x100 into g_rx_buffer
    g_eeprom.Read(start_address, g_rx_buffer, sizeof(g_rx_buffer), RxCallback);

    delay(500);
}


void RxCallback(const uint8_t error)
{
    // Check that no errors occurred
    if (error == 0)
    {
        PrintBuffer(g_rx_buffer, sizeof(g_rx_buffer));
    }
    else
    {
        /*
        Error values are identical to native Arduino Wire library as follows:
        0:success
        1:data too long to fit in transmit buffer
        2:received NACK on transmit of address
        3:received NACK on transmit of data
        4:timeout - slave took too long to respond
        */
        
        Serial.print("Communication Error #: ");
        Serial.println(error);
    }
}
