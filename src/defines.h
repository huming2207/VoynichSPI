/* Arduino SPIFlash Library v.3.1.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/05/2015
 * Modified by Prajwal Bhattaram - 24/02/2018
 *
 * This file is part of the Arduino SPIFlash Library. This library is for
 * Winbond NOR flash memory modules. In its current form it enables reading
 * and writing individual data variables, structs and arrays from and to various locations;
 * reading and writing pages; continuous read functions; sector, block and chip erase;
 * suspending and resuming programming/erase and powering down for low power operation.
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License v3.0
 * along with the Arduino SPIFlash Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

// Standard JEDEC instructions
#define	JEDEC_READ_MANSIG             0x90
#define JEDEC_READ_DATA               0x03
#define JEDEC_READ_FAST               0x0B
#define JEDEC_READ_STATREG            0x05
#define JEDEC_READ_JEDECID            0x9F
#define JEDEC_READ_SFDP               0x5A
#define JEDEC_READ_UNIQUE_ID          0x4B
#define JEDEC_READ_ID                 0x90   // Not in JEDEC standard but widely used???

#define JEDEC_ERASE_SECTOR            0x20
#define JEDEC_ERASE_BLOCK_32          0x52
#define JEDEC_ERASE_BLOCK_64          0xD8
#define JEDEC_ERASE_CHIP              0x60

#define JEDEC_PROG_BYTE               0x02
#define JEDEC_PROG_STATREG            0x01

#define JEDEC_SET_WRITE_DISABLE           0x04
#define JEDEC_SET_WRITE_STATREG_ENABLE    0x50
#define JEDEC_SET_WRITE_ENABLE            0x06
#define JEDEC_SET_4_BYTE_ADDR_ENABLE      0xB7
#define JEDEC_SET_4_BYTE_ADDR_DISABLE     0xE9
#define JEDEC_SET_SUSPEND                 0x75        // Not in JEDEC stand but widely used???
#define JEDEC_SET_RESUME                  0x7A
#define JEDEC_SET_POWERDOWN               0xB9
#define JEDEC_SET_RELEASE                 0xAB


// Misc
#define SPI_PAGESIZE  256
#define SPI_WRITE_DELAY   0x02

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                     General size definitions                       //
//            B = Bytes; KiB = Kilo Bytes; MiB = Mega Bytes           //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define B(x)          uint32_t(x*BYTE)
#define KB(x)         uint32_t(x*KiB)
#define MB(x)         uint32_t(x*MiB)

// Winbond specific
#define WINBOND_MANID               0xEF
#define WINBOND_READ_STATREG_2      0x35
#define WINBOND_READ_STATREG_3      0x15
#define WINBOND_PROG_STATREG_2      0x31
#define WINBOND_PROG_STATREG_3      0x11


//~~~~~~~~~~~~~~~~~~~~~~~~ Microchip ~~~~~~~~~~~~~~~~~~~~~~~~//
  #define MICROCHIP_MANID       0xBF
  #define MICROCHIP_SST25       0x25
  #define MICROCHIP_SST26       0x26
  #define ULBPR                 0x98    //Global Block Protection Unlock (Ref sections 4.1.1 & 5.37 of datasheet)

//~~~~~~~~~~~~~~~~~~~~~~~~ Cypress ~~~~~~~~~~~~~~~~~~~~~~~~//
  #define CYPRESS_MANID         0x01

//~~~~~~~~~~~~~~~~~~~~~~~~ Adesto ~~~~~~~~~~~~~~~~~~~~~~~~//
  #define ADESTO_MANID         0x1F

//~~~~~~~~~~~~~~~~~~~~~~~~ Micron ~~~~~~~~~~~~~~~~~~~~~~~~//
  #define MICRON_MANID         0x20
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//							Definitions 							  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#define BUSY          0x01
#define SPI_CLK       20000000        //Hz equivalent of 20MHz
#define WRTEN         0x02
#define SUS           0x80
#define WSE           0x04
#define WSP           0x08
#define ADS           0x01            // Current Address mode in Status register 3
#define DUMMYBYTE     0xEE
#define NULLBYTE      0x00
#define NULLINT       0x0000
#define NO_CONTINUE   0x00
#define PASS          0x01
#define FAIL          0x00
#define NOOVERFLOW    false
#define NOERRCHK      false
#define VERBOSE       true
#define PRINTOVERRIDE true
#define ERASEFUNC     0xEF
#if defined (SIMBLEE)
#define BUSY_TIMEOUT  100L
#elif defined ENABLEZERODMA
#define BUSY_TIMEOUT  3500L
#else
#define BUSY_TIMEOUT  1000L
#endif
#define arrayLen(x)   (sizeof(x) / sizeof(*x))
#define lengthOf(x)   (sizeof(x))/sizeof(byte)
#define BYTE          1L
#define KiB           1024L
#define MiB           KiB * KiB
#define S             1000L

#if defined (ARDUINO_ARCH_ESP8266)
#define CS 15
#else
#define CS SS
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     					   List of Error codes						  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#define VOYNICH_STATUS_SUCCESS              0x00
#define VOYNICH_STATUS_CALLBEGIN            0x01
#define VOYNICH_STATUS_UNKNOWNCHIP          0x02
#define VOYNICH_STATUS_UNKNOWNCAPACITY      0x03
#define VOYNICH_STATUS_CHIPBUSY             0x04
#define VOYNICH_STATUS_OUTOFBOUNDS          0x05
#define CANTENWRITE          0x06
#define PREVWRITTEN          0x07
#define LOWRAM               0x08
#define SYSSUSPEND           0x09
#define ERRORCHKFAIL         0x0A
#define NORESPONSE           0x0B
#define UNSUPPORTEDFUNC      0x0C
#define UNABLETO4BYTE        0x0D
#define UNABLETO3BYTE        0x0E
#define CHIPISPOWEREDDOWN    0x0F
#define UNKNOWNERROR         0xFE

 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
 //     					   List of Supported data types						  //
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#define _BYTE_              0x01
#define _CHAR_              0x02
#define _WORD_              0x03
#define _SHORT_             0x04
#define _ULONG_             0x05
#define _LONG_              0x06
#define _FLOAT_             0x07
#define _STRING_            0x08
#define _BYTEARRAY_         0x09
#define _CHARARRAY_         0x0A
#define _STRUCT_            0x0B

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                        Bit shift macros                            //
//                      Thanks to @VitorBoss                          //
//          https://github.com/Marzogh/SPIFlash/issues/76             //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define ADDR_BITS_1(param) (uint8_t)(((char *)&(param))[0]) //0x000y
#define ADDR_BITS_2(param) (uint8_t)(((char *)&(param))[1]) //0x00y0
#define ADDR_BITS_3(param) (uint8_t)(((char *)&(param))[2]) //0x0y00
#define ADDR_BITS_4(param) (uint8_t)(((char *)&(param))[3]) //0xy000
#define ADDR_BITS_12(param) (uint16_t)(((int *)&(param))[0]) //0x00yy
#define ADDR_BITS_34(param) (uint16_t)(((int *)&(param))[1]) //0xyy00
#define VOYNICH_SFDP_SIGNATURE 0x50444653
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
