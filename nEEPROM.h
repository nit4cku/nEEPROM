/*
 * Copyright (c) 2017 nitacku
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
 * @file        nEEPROM.h
 * @summary     Generic EEPROM Interface
 * @version     1.0
 * @author      nitacku
 * @data        15 July 2018
 */

#ifndef _EEPROM_H_
#define _EEPROM_H_

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <limits.h>
#include <inttypes.h>
#include <nI2C.h>

class CEEPROM
{
    public:
    
    enum status_t : uint8_t
    {
        STATUS_OK               = 0,
        STATUS_OUT_OF_BOUNDS    = 255,
    };
    
    enum class Address : uint8_t
    {
        A0 = 0x50,
        A1 = 0x51,
        A2 = 0x52,
        A3 = 0x53,
        A4 = 0x54,
        A5 = 0x55,
        A6 = 0x56,
        A7 = 0x57,
    };
    
    private:
    const Address m_i2c_address;
    const uint16_t m_page_size;
    const uint32_t m_memory_size;
    uint32_t m_address_size;

    CI2C::Handle m_i2c_handle;
    
    public:
    // Default constructor
    CEEPROM(const Address address, const uint16_t page_size, const uint16_t page_count);
    
    void Initialize(void);
    status_t Write(const uint32_t address, const uint8_t data[], const uint32_t byte_count);
    status_t Erase(const uint32_t address, const uint32_t byte_count);
    status_t Read(const uint32_t address, uint8_t data[], const uint32_t byte_count, void(*callback)(const uint8_t error) = nullptr);

    private:
    
    // Return integral value of Enumeration
    template<typename T> constexpr uint8_t getValue(const T e)
    {
        return static_cast<uint8_t>(e);
    }
    
    bool IsInRange(const uint32_t address, const uint32_t byte_count);
    uint8_t GetI2CAddress(void) { return getValue(m_i2c_address); }
    uint8_t I2CWrite(const uint32_t address, const uint8_t data[], const uint32_t bytes);
    uint8_t I2CRead(const uint32_t address, uint8_t data[], const uint32_t bytes, void(*callback)(const uint8_t error) = nullptr);
};
    
#endif
