/*
 * Copyright (c) 2014-2016 Freescale Semiconductor
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/*==============================================================================
Revision History:
                        Modification       Tracking
Author (core ID)        Date D/M/Y         Number     Description of Changes
Tomas Babinec           31-Mar-2015                   Init version
Xuyen Dao               05-Aug-2016        VSDK-251   Update template
==============================================================================*/

#ifndef OV10635CONFIG_H
#define OV10635CONFIG_H

#ifdef __cplusplus
extern "C"{
#endif

/**
* @file           ov10635_viu_config.h
* @brief          definition of ov10635 camera registers configuration 
*/

/*==============================================================================
*                               INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==============================================================================*/


/*==============================================================================
*                               CONSTANTS
==============================================================================*/


/*==============================================================================
*                            DEFINES AND MACROS
==============================================================================*/
#define FPS 30

/*==============================================================================
*                                  ENUMS
==============================================================================*/


/*==============================================================================
*                        STRUCTURES AND OTHER TYPEDEFS
==============================================================================*/


/*==============================================================================
*                        GLOBAL VARIABLE DECLARATIONS
==============================================================================*/
static uint8_t OvViu_Table[] =
{
  0x01,0x03,0x01,
  0x30,0x1b,0xff,
  0x30,0x1c,0xff,
  0x30,0x1a,0xff,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x0c,0x61,
  0x30,0x21,0x03,
  0x30,0x11,0x02,
  0x69,0x00,0x0c,
  0x69,0x01,0x01,
  0x30,0x33,0x08,
  0x35,0x03,0x10,
  0x30,0x2d,0x2f,
  0x30,0x25,0x03,
  0x30,0x03,0x20,
  0x30,0x04,0x23,
  0x30,0x05,0x20,
  0x30,0x06,0x91,
  0x36,0x00,0x74,
  0x36,0x01,0x2b,
  0x36,0x12,0x00,
  0x36,0x11,0x67,
  0x36,0x33,0xba,
  0x36,0x02,0x2f,
  0x36,0x03,0x00,
  0x36,0x30,0xa8,
  0x36,0x31,0x16,
  0x37,0x14,0x10,
  0x37,0x1d,0x01,
  0x43,0x00,0x38,
  0x30,0x07,0x01,
  0x30,0x24,0x01,
  0x30,0x20,0x0b,
  0x37,0x02,0x10,
  0x37,0x03,0x24,
  0x37,0x04,0x19,
  0x37,0x09,0x28,
  0x37,0x0d,0x00,
  0x37,0x12,0x00,
  0x37,0x13,0x20,
  0x37,0x15,0x04,
  0x38,0x1d,0x40,
  0x38,0x1c,0x00,
  0x38,0x24,0x10,
  0x38,0x15,0x8c,
  0x38,0x04,0x05,
  0x38,0x05,0x1f,
  0x38,0x00,0x00,
  0x38,0x01,0x00,
  0x38,0x06,0x03,
  0x38,0x07,0x29,
  0x38,0x02,0x00,
  0x38,0x03,0x06,
  0x38,0x08,0x05,
  0x38,0x09,0x00,
  0x38,0x0a,0x03,
  0x38,0x0b,0x20,
  0x38,0x0c,0x07,
  0x38,0x0d,0x71,
  0x38,0x0e,0x03,
  0x38,0x0f,0x48,
  0x38,0x11,0x08,
  0x38,0x1f,0x0c,
  0x36,0x21,0x63,
  0x50,0x05,0x08,
  0x56,0xd5,0x00,
  0x56,0xd6,0x80,
  0x56,0xd7,0x00,
  0x56,0xd8,0x00,
  0x56,0xd9,0x00,
  0x56,0xda,0x80,
  0x56,0xdb,0x00,
  0x56,0xdc,0x00,
  0x56,0xe8,0x00,
  0x56,0xe9,0x7f,
  0x56,0xea,0x00,
  0x56,0xeb,0x7f,
  0x51,0x00,0x00,
  0x51,0x01,0x80,
  0x51,0x02,0x00,
  0x51,0x03,0x80,
  0x51,0x04,0x00,
  0x51,0x05,0x80,
  0x51,0x06,0x00,
  0x51,0x07,0x80,
  0x51,0x08,0x00,
  0x51,0x09,0x00,
  0x51,0x0a,0x00,
  0x51,0x0b,0x00,
  0x51,0x0c,0x00,
  0x51,0x0d,0x00,
  0x51,0x0e,0x00,
  0x51,0x0f,0x00,
  0x51,0x10,0x00,
  0x51,0x11,0x80,
  0x51,0x12,0x00,
  0x51,0x13,0x80,
  0x51,0x14,0x00,
  0x51,0x15,0x80,
  0x51,0x16,0x00,
  0x51,0x17,0x80,
  0x51,0x18,0x00,
  0x51,0x19,0x00,
  0x51,0x1a,0x00,
  0x51,0x1b,0x00,
  0x51,0x1c,0x00,
  0x51,0x1d,0x00,
  0x51,0x1e,0x00,
  0x51,0x1f,0x00,
  0x56,0xd0,0x00,
  0x50,0x06,0x24,
  0x56,0x08,0x00,
  0x52,0xd7,0x06,
  0x52,0x8d,0x08,
  0x52,0x93,0x12,
  0x52,0xd3,0x12,
  0x52,0x88,0x06,
  0x52,0x89,0x20,
  0x52,0xc8,0x06,
  0x52,0xc9,0x20,
  0x52,0xcd,0x04,
  0x53,0x81,0x00,
  0x53,0x82,0xff,
  0x55,0x89,0x76,
  0x55,0x8a,0x47,
  0x55,0x8b,0xef,
  0x55,0x8c,0xc9,
  0x55,0x8d,0x49,
  0x55,0x8e,0x30,
  0x55,0x8f,0x67,
  0x55,0x90,0x3f,
  0x55,0x91,0xf0,
  0x55,0x92,0x10,
  0x55,0xa2,0x6d,
  0x55,0xa3,0x55,
  0x55,0xa4,0xc3,
  0x55,0xa5,0xb5,
  0x55,0xa6,0x43,
  0x55,0xa7,0x38,
  0x55,0xa8,0x5f,
  0x55,0xa9,0x4b,
  0x55,0xaa,0xf0,
  0x55,0xab,0x10,
  0x55,0x81,0x52,
  0x53,0x00,0x01,
  0x53,0x01,0x00,
  0x53,0x02,0x00,
  0x53,0x03,0x0e,
  0x53,0x04,0x00,
  0x53,0x05,0x0e,
  0x53,0x06,0x00,
  0x53,0x07,0x36,
  0x53,0x08,0x00,
  0x53,0x09,0xd9,
  0x53,0x0a,0x00,
  0x53,0x0b,0x0f,
  0x53,0x0c,0x00,
  0x53,0x0d,0x2c,
  0x53,0x0e,0x00,
  0x53,0x0f,0x59,
  0x53,0x10,0x00,
  0x53,0x11,0x7b,
  0x53,0x12,0x00,
  0x53,0x13,0x22,
  0x53,0x14,0x00,
  0x53,0x15,0xd5,
  0x53,0x16,0x00,
  0x53,0x17,0x13,
  0x53,0x18,0x00,
  0x53,0x19,0x18,
  0x53,0x1a,0x00,
  0x53,0x1b,0x26,
  0x53,0x1c,0x00,
  0x53,0x1d,0xdc,
  0x53,0x1e,0x00,
  0x53,0x1f,0x02,
  0x53,0x20,0x00,
  0x53,0x21,0x24,
  0x53,0x22,0x00,
  0x53,0x23,0x56,
  0x53,0x24,0x00,
  0x53,0x25,0x85,
  0x53,0x26,0x00,
  0x53,0x27,0x20,
  0x56,0x09,0x01,
  0x56,0x0a,0x40,
  0x56,0x0b,0x01,
  0x56,0x0c,0x40,
  0x56,0x0d,0x00,
  0x56,0x0e,0xfa,
  0x56,0x0f,0x00,
  0x56,0x10,0xfa,
  0x56,0x11,0x02,
  0x56,0x12,0x80,
  0x56,0x13,0x02,
  0x56,0x14,0x80,
  0x56,0x15,0x01,
  0x56,0x16,0x2c,
  0x56,0x17,0x01,
  0x56,0x18,0x2c,
  0x56,0x3b,0x01,
  0x56,0x3c,0x01,
  0x56,0x3d,0x01,
  0x56,0x3e,0x01,
  0x56,0x3f,0x03,
  0x56,0x40,0x03,
  0x56,0x41,0x03,
  0x56,0x42,0x05,
  0x56,0x43,0x09,
  0x56,0x44,0x05,
  0x56,0x45,0x05,
  0x56,0x46,0x05,
  0x56,0x47,0x05,
  0x56,0x51,0x00,
  0x56,0x52,0x80,
  0x52,0x1a,0x01,
  0x52,0x1b,0x03,
  0x52,0x1c,0x06,
  0x52,0x1d,0x0a,
  0x52,0x1e,0x0e,
  0x52,0x1f,0x12,
  0x52,0x20,0x16,
  0x52,0x23,0x02,
  0x52,0x25,0x04,
  0x52,0x27,0x08,
  0x52,0x29,0x0c,
  0x52,0x2b,0x12,
  0x52,0x2d,0x18,
  0x52,0x2f,0x1e,
  0x52,0x41,0x04,
  0x52,0x42,0x01,
  0x52,0x43,0x03,
  0x52,0x44,0x06,
  0x52,0x45,0x0a,
  0x52,0x46,0x0e,
  0x52,0x47,0x12,
  0x52,0x48,0x16,
  0x52,0x4a,0x03,
  0x52,0x4c,0x04,
  0x52,0x4e,0x08,
  0x52,0x50,0x0c,
  0x52,0x52,0x12,
  0x52,0x54,0x18,
  0x52,0x56,0x1e,
  0x46,0x05,0x08,
  0x46,0x06,0x07,
  0x46,0x07,0x71,
  0x46,0x0a,0x02,
  0x46,0x0b,0x70,
  0x46,0x0c,0x00,
  0x46,0x20,0x0e,
  0x47,0x00,0x04,
  0x47,0x01,0x00,
  0x47,0x02,0x01,
  0x47,0x08,0x01,
  0x40,0x04,0x08,
  0x40,0x05,0x18,
  0x40,0x01,0x04,
  0x40,0x50,0x20,
  0x40,0x51,0x22,
  0x40,0x57,0x9c,
  0x40,0x5a,0x00,
  0x42,0x02,0x02,
  0x30,0x23,0x10,
  0x01,0x00,0x01,
  0x01,0x00,0x01,
  0x6f,0x0e,0x00,
  0x6f,0x0f,0x00,
  0x46,0x0e,0x08,
  0x46,0x0f,0x01,
  0x46,0x10,0x00,
  0x46,0x11,0x01,
  0x46,0x12,0x00,
  0x46,0x13,0x01,
  0x46,0x05,0x08,
  0x46,0x08,0x00,
  0x46,0x09,0x08,
  0x68,0x04,0x00,
  0x68,0x05,0x06,
  0x68,0x06,0x00,
  0x51,0x20,0x00,
  0x35,0x10,0x00,
  0x35,0x04,0x00,
  0x68,0x00,0x00,
  0x6f,0x0d,0x00,
  0x50,0x00,0xff,
  0x50,0x01,0xbf,
  0x50,0x02,0xfe,
  0x50,0x3d,0x00,
  0xc4,0x50,0x01,
  0xc4,0x52,0x04,
  0xc4,0x53,0x00,
  0xc4,0x54,0x00,
  0xc4,0x55,0x00,
  0xc4,0x56,0x00,
  0xc4,0x57,0x00,
  0xc4,0x58,0x00,
  0xc4,0x59,0x00,
  0xc4,0x5b,0x00,
  0xc4,0x5c,0x00,
  0xc4,0x5d,0x00,
  0xc4,0x5e,0x00,
  0xc4,0x5f,0x00,
  0xc4,0x60,0x00,
  0xc4,0x61,0x01,
  0xc4,0x62,0x01,
  0xc4,0x64,0x88,
  0xc4,0x65,0x00,
  0xc4,0x66,0x8a,
  0xc4,0x67,0x00,
  0xc4,0x68,0x86,
  0xc4,0x69,0x00,
  0xc4,0x6a,0x40,
  0xc4,0x6b,0x50,
  0xc4,0x6c,0x30,
  0xc4,0x6d,0x28,
  0xc4,0x6e,0x60,
  0xc4,0x6f,0x40,
  0xc4,0x7c,0x01,
  0xc4,0x7d,0x38,
  0xc4,0x7e,0x00,
  0xc4,0x7f,0x00,
  0xc4,0x80,0x00,
  0xc4,0x81,0xff,
  0xc4,0x82,0x00,
  0xc4,0x83,0x40,
  0xc4,0x84,0x00,
  0xc4,0x85,0x18,
  0xc4,0x86,0x00,
  0xc4,0x87,0x18,
  0xc4,0x88,0x34,
  0xc4,0x89,0x00,
  0xc4,0x8a,0x34,
  0xc4,0x8b,0x00,
  0xc4,0x8c,0x00,
  0xc4,0x8d,0x04,
  0xc4,0x8e,0x00,
  0xc4,0x8f,0x04,
  0xc4,0x90,0x07,
  0xc4,0x92,0x20,
  0xc4,0x93,0x08,
  0xc4,0x98,0x02,
  0xc4,0x99,0x00,
  0xc4,0x9a,0x02,
  0xc4,0x9b,0x00,
  0xc4,0x9c,0x02,
  0xc4,0x9d,0x00,
  0xc4,0x9e,0x02,
  0xc4,0x9f,0x60,
  0xc4,0xa0,0x04,
  0xc4,0xa1,0x00,
  0xc4,0xa2,0x06,
  0xc4,0xa3,0x00,
  0xc4,0xa4,0x00,
  0xc4,0xa5,0x10,
  0xc4,0xa6,0x00,
  0xc4,0xa7,0x40,
  0xc4,0xa8,0x00,
  0xc4,0xa9,0x80,
  0xc4,0xaa,0x0d,
  0xc4,0xab,0x00,
  0xc4,0xac,0x0f,
  0xc4,0xad,0xc0,
  0xc4,0xb4,0x01,
  0xc4,0xb5,0x01,
  0xc4,0xb6,0x00,
  0xc4,0xb7,0x01,
  0xc4,0xb8,0x00,
  0xc4,0xb9,0x01,
  0xc4,0xba,0x01,
  0xc4,0xbb,0x00,
  0xc4,0xbe,0x02,
  0xc4,0xbf,0x33,
  0xc4,0xc8,0x03,
  0xc4,0xc9,0xd0,
  0xc4,0xca,0x0e,
  0xc4,0xcb,0x00,
  0xc4,0xcc,0x10,
  0xc4,0xcd,0x18,
  0xc4,0xce,0x10,
  0xc4,0xcf,0x18,
  0xc4,0xd0,0x04,
  0xc4,0xd1,0x80,
  0xc4,0xe0,0x04,
  0xc4,0xe1,0x02,
  0xc4,0xe2,0x01,
  0xc4,0xe4,0x10,
  0xc4,0xe5,0x20,
  0xc4,0xe6,0x30,
  0xc4,0xe7,0x40,
  0xc4,0xe8,0x50,
  0xc4,0xe9,0x60,
  0xc4,0xea,0x70,
  0xc4,0xeb,0x80,
  0xc4,0xec,0x90,
  0xc4,0xed,0xa0,
  0xc4,0xee,0xb0,
  0xc4,0xef,0xc0,
  0xc4,0xf0,0xd0,
  0xc4,0xf1,0xe0,
  0xc4,0xf2,0xf0,
  0xc4,0xf3,0x80,
  0xc4,0xf4,0x00,
  0xc4,0xf5,0x20,
  0xc4,0xf6,0x02,
  0xc4,0xf7,0x00,
  0xc4,0xf8,0x04,
  0xc4,0xf9,0x0b,
  0xc4,0xfa,0x00,
  0xc4,0xfb,0x01,
  0xc4,0xfc,0x01,
  0xc4,0xfd,0x00,
  0xc4,0xfe,0x04,
  0xc4,0xff,0x02,
  0xc5,0x00,0x68,
  0xc5,0x01,0x74,
  0xc5,0x02,0x70,
  0xc5,0x03,0x80,
  0xc5,0x04,0x05,
  0xc5,0x05,0x80,
  0xc5,0x06,0x03,
  0xc5,0x07,0x80,
  0xc5,0x08,0x01,
  0xc5,0x09,0xc0,
  0xc5,0x0a,0x01,
  0xc5,0x0b,0xa0,
  0xc5,0x0c,0x01,
  0xc5,0x0d,0x2c,
  0xc5,0x0e,0x01,
  0xc5,0x0f,0x0a,
  0xc5,0x10,0x00,
  0xc5,0x11,0x01,
  0xc5,0x12,0x01,
  0xc5,0x13,0x80,
  0xc5,0x14,0x04,
  0xc5,0x15,0x00,
  0xc5,0x18,0x03,
  0xc5,0x19,0x48,
  0xc5,0x1a,0x07,
  0xc5,0x1b,0x70,
  0xc2,0xe0,0x00,
  0xc2,0xe1,0x51,
  0xc2,0xe2,0x00,
  0xc2,0xe3,0xd6,
  0xc2,0xe4,0x01,
  0xc2,0xe5,0x5e,
  0xc2,0xe9,0x01,
  0xc2,0xea,0x7a,
  0xc2,0xeb,0x90,
  0xc2,0xed,0x00,
  0xc2,0xee,0x7a,
  0xc2,0xef,0x64,
  0xc3,0x08,0x00,
  0xc3,0x09,0x00,
  0xc3,0x0a,0x00,
  0xc3,0x0c,0x00,
  0xc3,0x0d,0x01,
  0xc3,0x0e,0x00,
  0xc3,0x0f,0x00,
  0xc3,0x10,0x01,
  0xc3,0x11,0x60,
  0xc3,0x12,0xff,
  0xc3,0x13,0x08,
  0xc3,0x14,0x01,
  0xc3,0x15,0x7f,
  0xc3,0x16,0xff,
  0xc3,0x17,0x0b,
  0xc3,0x18,0x00,
  0xc3,0x19,0x0c,
  0xc3,0x1a,0x00,
  0xc3,0x1b,0xe0,
  0xc3,0x1c,0x00,
  0xc3,0x1d,0x14,
  0xc3,0x1e,0x00,
  0xc3,0x1f,0xc5,
  0xc3,0x20,0xff,
  0xc3,0x21,0x4b,
  0xc3,0x22,0xff,
  0xc3,0x23,0xf0,
  0xc3,0x24,0xff,
  0xc3,0x25,0xe8,
  0xc3,0x26,0x00,
  0xc3,0x27,0x46,
  0xc3,0x28,0xff,
  0xc3,0x29,0xd2,
  0xc3,0x2a,0xff,
  0xc3,0x2b,0xe4,
  0xc3,0x2c,0xff,
  0xc3,0x2d,0xbb,
  0xc3,0x2e,0x00,
  0xc3,0x2f,0x61,
  0xc3,0x30,0xff,
  0xc3,0x31,0xf9,
  0xc3,0x32,0x00,
  0xc3,0x33,0xd9,
  0xc3,0x34,0x00,
  0xc3,0x35,0x2e,
  0xc3,0x36,0x00,
  0xc3,0x37,0xb1,
  0xc3,0x38,0xff,
  0xc3,0x39,0x64,
  0xc3,0x3a,0xff,
  0xc3,0x3b,0xeb,
  0xc3,0x3c,0xff,
  0xc3,0x3d,0xe8,
  0xc3,0x3e,0x00,
  0xc3,0x3f,0x48,
  0xc3,0x40,0xff,
  0xc3,0x41,0xd0,
  0xc3,0x42,0xff,
  0xc3,0x43,0xed,
  0xc3,0x44,0xff,
  0xc3,0x45,0xad,
  0xc3,0x46,0x00,
  0xc3,0x47,0x66,
  0xc3,0x48,0x01,
  0xc3,0x49,0x00,
  0x67,0x00,0x04,
  0x67,0x01,0x7b,
  0x67,0x02,0xfd,
  0x67,0x03,0xf9,
  0x67,0x04,0x3d,
  0x67,0x05,0x71,
  0x67,0x06,0x71,
  0x67,0x08,0x05,
  0x38,0x22,0x50,
  0x6f,0x06,0x6f,
  0x6f,0x07,0x00,
  0x6f,0x0a,0x6f,
  0x6f,0x0b,0x00,
  0x6f,0x00,0x03,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x42,0xf0,
  0x30,0x1b,0xf0,
  0x30,0x1c,0xf0,
  0x30,0x1a,0xf0,
  0xc4,0xbc,0x01,
  0xc4,0xbd,0x60,
  0xc4,0xa0,0x03,
  0xc4,0xa2,0x04,
  0x30,0x11,0x42,
  0x30,0x04,0x21,
  0x37,0x02,0x20,
  0x37,0x03,0x48,
  0x37,0x04,0x32,

  // additions
  //pixel clock = 94.5sMHz
  0x30,0x04,0x01,
  0x30,0x03,0x0E,

  //VSYNC length
  0x47,0x01,0x04,

  // Change of format
  0x43,0x00,0x3A,

#if 0
  // Test Pattern
  0x30,0x42,0xF9, // to do after sw reset and before enabling test pattern
                  // (see p38)
  0x50,0x00,0x78, // Turn off certain ISP blocks for consistent patterns

  // Colour bar
  0x50,0x3D,0x80  // enabling digital pattern + choice of pattern
#endif

}; //OvViu_Table[]


/*=============================================================================
*                              FUNCTION PROTOTYPES
==============================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* OV10635CONFIG_H */

/** @} */
