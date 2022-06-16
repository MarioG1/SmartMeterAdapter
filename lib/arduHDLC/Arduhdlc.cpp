/*
Arduhdlc is Arduino HDLC library, which can be used over any interface.
Copyright (C) 2015 Jarkko Hautakorpi et al. see LICENSE.txt

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Arduino.h"
#include "Arduhdlc.h"
#include "../crc16_x_25/crc16_x_25.h"

//#define LOGGING

/* HDLC Asynchronous timeout in millisecounds*/
#define HDLC_TIMEOUT 2000

/* HDLC Asynchronous framing */
/* The frame boundary octet is 01111110, (7E in hexadecimal notation) */
#define FRAME_BOUNDARY_OCTET 0x7E
#define FRAME_START_OCTET 0xA0

/* A "control escape octet", has the bit sequence '01111101', (7D hexadecimal) */
#define CONTROL_ESCAPE_OCTET 0x7D

/* If either of these two octets appears in the transmitted data, an escape octet is sent, */
/* followed by the original data octet with bit 5 inverted */
#define INVERT_OCTET 0x20

/* The frame check sequence (FCS) is a 16-bit CRC-CCITT */
/* AVR Libc CRC function is _crc_ccitt_update() */
/* Corresponding CRC function in Qt (www.qt.io) is qChecksum() */
#define CRC16_CCITT_INIT_VAL 0xFFFF

/* 16bit low and high bytes copier */
#define low(x)    ((x) & 0xFF)
#define high(x)   (((x)>>8) & 0xFF)

Arduhdlc::Arduhdlc (sendchar_type put_char, frame_handler_type hdlc_command_router, uint16_t max_frame_length) : sendchar_function(put_char), frame_handler(hdlc_command_router)
{
    this->frame_position = 0;
	this->max_frame_length = max_frame_length;
	this->receive_frame_buffer = (uint8_t *)malloc(max_frame_length+1); // char *ab = (char*)malloc(12);
    //this->frame_checksum = CRC16_CCITT_INIT_VAL;
    this->escape_character = false;
    this->frame_start = false;
    this->frame_start_time = millis();
}

/* Function to send a byte throug USART, I2C, SPI etc.*/
void Arduhdlc::sendchar(uint8_t data)
{
    (*this->sendchar_function)(data);
}

/* Function to find valid HDLC frame from incoming data */
void Arduhdlc::charReceiver(uint8_t data)
{
    crc16_x_25 crc16x25;

    if(((millis() - this->frame_start_time) > HDLC_TIMEOUT) && this->frame_start == true ) {
        this->frame_position = 0;
        this->frame_start = false;
         #ifdef LOGGING
            Serial.print("HDLC receive timeout accoured: ");
            Serial.print(millis() - this->frame_start_time);
            Serial.print("ms\n\r");
        #endif
    }
    
    if(this->frame_start == false) {
        if(this->frame_position >= 1) {
            if(data == FRAME_START_OCTET && receive_frame_buffer[this->frame_position-1] == FRAME_BOUNDARY_OCTET) {
                this->frame_start = true;
                this->frame_position = 0;
                this->frame_start_time = millis();

                #ifdef LOGGING
                    Serial.println("HDLC Frame Start");
                #endif
            }
        }
    } else if(this->frame_position >= 109) {
        if(crc16x25.calc_checksum(this->receive_frame_buffer,this->frame_position-2) == ((this->receive_frame_buffer[this->frame_position-1] << 8 ) | (this->receive_frame_buffer[this->frame_position-2] & 0xff)))  {
                (*frame_handler)(receive_frame_buffer,(uint8_t)(this->frame_position-2));
        } else {
            #ifdef LOGGING
                Serial.print("CRC Checkum error. Buffer Size: ");
                Serial.print(this->frame_position);
                Serial.print(" Last Received Char: ");
                Serial.print(data, HEX);
                Serial.print("\r\n");
            #endif
        }

        this->frame_position = 0;
        this->frame_start = false;
        return;
    }


    /* FRAME FLAG */
    /*if(data == FRAME_BOUNDARY_OCTET)
    {
        if(this->frame_buffer_overflow == true) {
            this->frame_buffer_overflow == false;
        }

        if(this->escape_character == true) {
            this->escape_character = false;
        } else if(this->frame_position >= 108)  {
            if(crc16x25.calc_checksum(this->receive_frame_buffer,this->frame_position-2) == ((this->receive_frame_buffer[this->frame_position-1] << 8 ) | (this->receive_frame_buffer[this->frame_position-2] & 0xff)))  {
                (*frame_handler)(receive_frame_buffer,(uint8_t)(this->frame_position-2));
            } else {
                #ifdef LOGGING
                    Serial.print("CRC Checkum error. Buffer Size: ");
                    Serial.print(this->frame_position);
                    Serial.print(" Last Received Char: ");
                    Serial.print(data, HEX);
                    Serial.print("\r\n");
                #endif
            }

            this->frame_position = 0;
            //this->frame_checksum = CRC16_CCITT_INIT_VAL;
            return;
        } else if(this->frame_position == 0) {
            return;
        }
    } else if(this->frame_buffer_overflow == true) {
        this->frame_position = 0;
        return;
    }

    
    if(this->escape_character)
    {
        this->escape_character = false;
        data ^= INVERT_OCTET;
    } 
    else if(data == CONTROL_ESCAPE_OCTET)
    {
        this->escape_character = true;
        return;
    }
    */

    receive_frame_buffer[this->frame_position] = data;

    //if(this->frame_position-2 >= 0) {
    //    this->frame_checksum = _crc_ccitt_update(this->frame_checksum, receive_frame_buffer[this->frame_position-2]);
    //}

    this->frame_position++;

    if(this->frame_position == this->max_frame_length)
    {
        this->frame_position = 0;
        this->frame_buffer_overflow == true;
        //this->frame_checksum = CRC16_CCITT_INIT_VAL;
        #ifdef LOGGING
            Serial.println("Max Frame Buffer size reached");
        #endif
    }
}

/* Wrap given data in HDLC frame and send it out byte at a time*/
void Arduhdlc::frameDecode(const char *framebuffer, uint8_t frame_length)
{
    uint8_t data;
    uint16_t fcs = CRC16_CCITT_INIT_VAL;

    this->sendchar((uint8_t)FRAME_BOUNDARY_OCTET);

    while(frame_length)
    {
        data = *framebuffer++;
        fcs = _crc_ccitt_update(fcs, data);
        if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
        {
            this->sendchar((uint8_t)CONTROL_ESCAPE_OCTET);
            data ^= INVERT_OCTET;
        }
        this->sendchar((uint8_t)data);
        frame_length--;
    }
    data = low(fcs);
    if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
    {
        this->sendchar((uint8_t)CONTROL_ESCAPE_OCTET);
        data ^= (uint8_t)INVERT_OCTET;
    }
    this->sendchar((uint8_t)data);
    data = high(fcs);
    if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
    {
        this->sendchar(CONTROL_ESCAPE_OCTET);
        data ^= INVERT_OCTET;
    }
    this->sendchar(data);
    this->sendchar(FRAME_BOUNDARY_OCTET);
}


