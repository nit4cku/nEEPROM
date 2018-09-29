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
 * @file        nEEPROM.cpp
 * @summary     Generic EEPROM Interface
 * @version     1.0
 * @author      nitacku
 * @data        15 July 2018
 */


#include "nEEPROM.h"

CEEPROM::CEEPROM(const Address address, const uint16_t page_size, const uint16_t page_count)
    : m_i2c_address{address}
    , m_page_size{page_size}
    , m_memory_size{page_size * page_count}
{
    // Find least bytes needed to address all memory
    if (m_memory_size & 0xFF000000)
    {
        m_address_size = 4;
    }
    else if (m_memory_size & 0x00FF0000)
    {
        m_address_size = 3;
    }
    else if (m_memory_size & 0x0000FF00)
    {
        m_address_size = 2;
    }
    else
    {
        m_address_size = 1;
    }
}


void CEEPROM::Initialize(void)
{
    m_i2c_handle = nI2C->RegisterDevice(GetI2CAddress(), m_address_size, CI2C::Speed::FAST);
}


CEEPROM::status_t CEEPROM::Write(const uint32_t address, const uint8_t data[], const uint32_t byte_count)
{
	// Check that address is within range
    if (IsInRange(address, byte_count))
	{
        uint32_t bytes_remaining = byte_count;

        while (bytes_remaining > 0)
        {
            uint32_t offset = (byte_count - bytes_remaining);
            
            // Calculate page overlap
            uint16_t bytes = m_page_size - ((address + offset) % m_page_size);
            
            // Adjust if writing partial page last
            if (bytes > bytes_remaining)
            {
                bytes = bytes_remaining;
            }

            uint8_t status = I2CWrite(address + offset, &data[offset], bytes);
            
            if (status)
            {
                return (status_t)status; // Error
            }

            bytes_remaining -= bytes;
        }

        return STATUS_OK;
    }

	return STATUS_OUT_OF_BOUNDS; // Out-of-bounds
}


CEEPROM::status_t CEEPROM::Erase(const uint32_t address, const uint32_t byte_count)
{
	// Check that address is within range
    if (IsInRange(address, byte_count))
	{
		// Create empty array to overwrite page
		uint8_t data[m_page_size] = {0};
		uint32_t bytes_remaining = byte_count;

		while (bytes_remaining > 0)
        {
            uint16_t bytes;
            uint32_t offset = (byte_count - bytes_remaining);
            
            // Adjust if writing partial page last
            if (bytes_remaining < m_page_size)
            {
                bytes = bytes_remaining;
            }
            else
            {
                bytes = m_page_size;
            }
            
            uint8_t status = Write(address + offset, data, bytes);
            
            if (status)
            {
                return (status_t)status; // Error
            }
            
            bytes_remaining -= bytes;
        }
        
        return STATUS_OK;
	}

	return STATUS_OUT_OF_BOUNDS; // Out-of-bounds
}


CEEPROM::status_t CEEPROM::Read(const uint32_t address, uint8_t data[], const uint32_t byte_count, void(*callback)(const uint8_t error))
{
	// Check that address is within range
    if (IsInRange(address, byte_count))
	{
        return (status_t)I2CRead(address, data, byte_count, callback);
	}

	return STATUS_OUT_OF_BOUNDS; // Out-of-bounds
}


bool CEEPROM::IsInRange(const uint32_t address, const uint32_t byte_count)
{
    // Ensure no overflow
    if (address < (UINT_MAX - byte_count))
    {
        // Check that address is within bounds    
        if ((address + byte_count) < m_memory_size)
        {
            return true;
        }
    }
    
    return false;
}


uint8_t CEEPROM::I2CWrite(const uint32_t address, const uint8_t data[], const uint32_t bytes)
{
    return nI2C->Write(m_i2c_handle, address, data, bytes, 5); // 5ms delay between transmits
}


uint8_t CEEPROM::I2CRead(const uint32_t address, uint8_t data[], const uint32_t bytes, void(*callback)(const uint8_t error))
{
    return nI2C->Read(m_i2c_handle, address, data, bytes, callback);
}
