/* Arduino SPIFlash Library v.3.1.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/05/2015
 * Modified by @boseji <salearj@hotmail.com> - 02/03/2017
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

#ifndef SPIFLASH_H
#define SPIFLASH_H
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Uncomment the code below to run a diagnostic if your flash 	  //
//                         does not respond                           //
//                                                                    //
//      Error codes will be generated and returned on functions       //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define RUNDIAGNOSTIC                                               //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//   Uncomment the code below to increase the speed of the library    //
//                  by disabling _notPrevWritten()                    //
//                                                                    //
// Make sure the sectors being written to have been erased beforehand //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//#define HIGHSPEED                                                   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//   Uncomment the code below to disable overflow and force data      //
//   to only be written to the last address of the flash memory       //
//    and not rollover to address 0x00 when the end is reached        //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//#define DISABLEOVERFLOW                                             //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//   Comment out the code below to disable DMA mode on SAMD based     //
//                            platforms                               //
//                                                                    //
//   Change the ZERO_SPISERCOM define below to use other SPI ports    //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//#define ENABLEZERODMA                                               //
//#define ZERO_SPISERCOM SERCOM4                                      //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define PRINTNAMECHANGEALERT

#include <Arduino.h>
#include "defines.h"
#include <SPI.h>


#define _delay_us(us) delayMicroseconds(us)

#define CHIP_SELECT   digitalWrite(csPin, LOW);
#define CHIP_DESELECT digitalWrite(csPin, HIGH);
#define xfer(n)   SPI.transfer(n)
#define BEGIN_SPI SPI.begin();

#define LIBVER 3
#define LIBSUBVER 1
#define BUGFIXVER 0

class SPIFlash {
public:
  //------------------------------------ Constructor ------------------------------------//
  //New Constructor to Accept the PinNames as a Chip select Parameter - @boseji <salearj@hotmail.com> 02.03.17
  SPIFlash(uint8_t cs);
  //----------------------------- Initial / Chip Functions ------------------------------//
  bool     begin(uint32_t flashChipSize = 0);
  void     setClock(uint32_t clockSpeed);
  bool     libver(uint8_t *b1, uint8_t *b2, uint8_t *b3);
  uint8_t  error(bool verbosity = false);
  uint16_t getManID();
  uint32_t getJEDECID();
  uint64_t getUniqueID();
  uint32_t getAddress(uint16_t size);
  uint16_t sizeofStr(String &inputStr);
  uint32_t getCapacity();
  uint32_t getMaxPage();
  float    functionRunTime();
  //-------------------------------- Write / Read Bytes ---------------------------------//
  bool     writeByte(uint32_t _addr, uint8_t data, bool errorCheck = true);
  uint8_t  readByte(uint32_t _addr, bool fastRead = false);
  //----------------------------- Write / Read Byte Arrays ------------------------------//
  bool     writeByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool errorCheck = true);
  bool     readByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool fastRead = false);
  //-------------------------------- Write / Read Chars ---------------------------------//
  bool     writeChar(uint32_t _addr, int8_t data, bool errorCheck = true);
  int8_t   readChar(uint32_t _addr, bool fastRead = false);
  //------------------------------ Write / Read Char Arrays -----------------------------//
  bool     writeCharArray(uint32_t _addr, char *data_buffer, size_t bufferSize, bool errorCheck = true);
  bool     readCharArray(uint32_t _addr, char *data_buffer, size_t buffer_size, bool fastRead = false);
  //-------------------------------- Write / Read Shorts --------------------------------//
  bool     writeShort(uint32_t _addr, int16_t data, bool errorCheck = true);
  int16_t  readShort(uint32_t _addr, bool fastRead = false);
  //-------------------------------- Write / Read Words ---------------------------------//
  bool     writeWord(uint32_t _addr, uint16_t data, bool errorCheck = true);
  uint16_t readWord(uint32_t _addr, bool fastRead = false);
  //-------------------------------- Write / Read Longs ---------------------------------//
  bool     writeLong(uint32_t _addr, int32_t data, bool errorCheck = true);
  int32_t  readLong(uint32_t _addr, bool fastRead = false);
  //--------------------------- Write / Read Unsigned Longs -----------------------------//
  bool     writeULong(uint32_t _addr, uint32_t data, bool errorCheck = true);
  uint32_t readULong(uint32_t _addr, bool fastRead = false);
  //-------------------------------- Write / Read Floats --------------------------------//
  bool     writeFloat(uint32_t _addr, float data, bool errorCheck = true);
  float    readFloat(uint32_t _addr, bool fastRead = false);
  //-------------------------------- Write / Read Strings -------------------------------//
  bool     writeStr(uint32_t _addr, String &data, bool errorCheck = true);
  bool     readStr(uint32_t _addr, String &data, bool fastRead = false);
  //------------------------------- Write / Read Anything -------------------------------//

  template <class T> bool writeAnything(uint32_t _addr, const T& data, bool errorCheck = true);
  template <class T> bool readAnything(uint32_t _addr, T& data, bool fastRead = false);
  //-------------------------------- Erase functions ------------------------------------//
  bool     eraseSection(uint32_t _addr, uint32_t _sz);
  bool     eraseSector(uint32_t _addr);
  bool     eraseBlock32K(uint32_t _addr);
  bool     eraseBlock64K(uint32_t _addr);
  bool     eraseChip();
  //-------------------------------- Power functions ------------------------------------//
  bool     suspendProg();
  bool     resumeProg();
  bool     powerDown();
  bool     powerUp();
  //------------------------------- Public variables ------------------------------------//

private:
  //------------------------------- Private functions -----------------------------------//
  void     _troubleshoot(uint8_t _code, bool printoverride = false);
  void     _printErrorCode();
  void     _printSupportLink();
  void     _endSPI();
  bool     _disableGlobalBlockProtect();
  bool     _isChipPoweredDown();
  bool     _prep(uint8_t opcode, uint32_t _addr, uint32_t size = 0);
  bool     _startSPIBus();
  bool     _beginSPI(uint8_t opcode);
  bool     _noSuspend();
  bool     _notBusy(uint32_t timeout = BUSY_TIMEOUT);
  bool     _notPrevWritten(uint32_t _addr, uint32_t size = 1);
  bool     _writeEnable(bool _troubleshootEnable = true);
  bool     _writeDisable();
  bool     _getJedecId();
  bool     _getManId(uint8_t *b1, uint8_t *b2);
  bool     _getSFDP();
  bool     _chipID();
  bool     _transferAddress();
  bool     _addressCheck(uint32_t _addr, uint32_t size = 1);
  bool     _enable4ByteAddressing();
  bool     _disable4ByteAddressing();
  uint8_t  _nextByte(char IOType, uint8_t data = NULLBYTE);
  uint16_t _nextInt(uint16_t = NULLINT);
  void     _nextBuf(uint8_t opcode, uint8_t *data_buffer, uint32_t size);
  uint8_t  _readStat1();
  uint8_t  _readStat2();
  uint8_t  _readStat3();
  template <class T> bool _write(uint32_t _addr, const T& value, uint32_t _sz, bool errorCheck, uint8_t _dataType);
  template <class T> bool _read(uint32_t _addr, T& value, uint32_t _sz, bool fastRead = false, uint8_t _dataType = 0x00);
  //template <class T> bool _writeErrorCheck(uint32_t _addr, const T& value);
  template <class T> bool _writeErrorCheck(uint32_t _addr, const T& value, uint32_t _sz, uint8_t _dataType = 0x00);
  //-------------------------------- Private variables ----------------------------------//
  #ifdef SPI_HAS_TRANSACTION
    SPISettings _settings;
  #endif
  //If multiple SPI ports are available this variable is used to choose between them (SPI, SPI1, SPI2 etc.)
  SPIClass *_spi;
  #if !defined (BOARD_RTL8195A)
  uint8_t     csPin;
  #else
  // Object declaration for the GPIO HAL type for csPin - @boseji <salearj@hotmail.com> 02.03.17
  gpio_t      csPin;
  #endif
  volatile uint8_t *cs_port;
  bool        pageOverflow, SPIBusState;
  bool        chipPoweredDown = false;
  bool        address4ByteEnabled = false;
  uint8_t     cs_mask, errorcode, stat1, stat2, stat3, _SPCR, _SPSR, _a0, _a1, _a2;
  char READ = 'R';
  char WRITE = 'W';
  float _spifuncruntime = 0;
  struct      chipID {
                bool supported;
                uint8_t manufacturerID;
                uint8_t memoryTypeID;
                uint8_t capacityID;
                uint32_t sfdp;
                uint32_t capacity;
                uint32_t eraseTime;
              };
              chipID _chip;
  uint32_t    currentAddress, _currentAddress = 0;
  uint32_t    _addressOverflow = false;
  uint8_t _uniqueID[8];
  const uint8_t _capID[14]   =
  {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x43, 0x4B, 0x00, 0x01};

  const uint32_t _memSize[14]  =
  {KB(64), KB(128), KB(256), KB(512), MB(1), MB(2), MB(4), MB(8), MB(16), MB(32), MB(8), MB(8), KB(256), KB(512)}; // To understand the _memSize definitions check defines.h
};

//--------------------------------- Public Templates ------------------------------------//

// Writes any type of data to a specific location in the flash memory.
// Takes three arguments -
//  1. _addr --> Any address from 0 to maxAddress
//  2. T& value --> Variable to write
//  3. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
//      Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
template <class T> bool SPIFlash::writeAnything(uint32_t _addr, const T& data, bool errorCheck) {
  return _write(_addr, data, sizeof(data), errorCheck, _STRUCT_);
}

// Reads any type of data from a specific location in the flash memory.
// Takes three arguments -
//  1. _addr --> Any address from 0 to maxAddress
//  2. T& value --> Variable to return data into
//  3. fastRead --> defaults to false - executes _beginFastRead() if set to true
template <class T> bool SPIFlash::readAnything(uint32_t _addr, T& data, bool fastRead) {
  return _read(_addr, data, sizeof(data), fastRead);
}

//---------------------------------- Private Templates ----------------------------------//

template <class T> bool SPIFlash::_writeErrorCheck(uint32_t _addr, const T& value, uint32_t _sz, uint8_t _dataType) {
  if (_isChipPoweredDown() || !_addressCheck(_addr, _sz) || !_notBusy()) {
    return false;
  }
  auto* p = (const uint8_t*)(const void*)&value;
  if (_dataType == _STRUCT_) {
    uint8_t _inByte[_sz];
    _beginSPI(JEDEC_READ_DATA);
    _nextBuf(JEDEC_READ_DATA, &(*_inByte), _sz);
    _endSPI();
    for (uint16_t i = 0; i < _sz; i++) {
      if (*p++ != _inByte[i]) {
        _troubleshoot(ERRORCHKFAIL);
        return false;
      }
      else {
        return true;
      }
    }
  }
  else {
    CHIP_SELECT
    _nextByte(WRITE, JEDEC_READ_DATA);
    _transferAddress();
    for (uint16_t i = 0; i < _sz; i++) {
      if (*p++ != _nextByte(READ)) {
        _troubleshoot(ERRORCHKFAIL);
        _endSPI();
        return false;
      }
    }
    _endSPI();
  }
  return true;
}

// Writes any type of data to a specific location in the flash memory.
// Takes four arguments -
//  1. _addr --> Any address from 0 to maxAddress
//  2. T& value --> Variable to write
//  3. _sz --> Size of variable in bytes (1 byte = 8 bits)
//  4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
//      Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)

template <class T> bool SPIFlash::_write(uint32_t _addr, const T& value, uint32_t _sz, bool errorCheck, uint8_t _dataType) {
  bool _retVal;
#ifdef RUNDIAGNOSTIC
  _spifuncruntime = micros();
#endif

  uint32_t _addrIn = _addr;
  if (!_prep(JEDEC_PROG_BYTE, _addrIn, _sz)) {
    return false;
  }
  _addrIn = _currentAddress;
  const uint8_t* p = ((const uint8_t*)(const void*)&value);
  if (!SPIBusState) {
    _startSPIBus();
  }
  CHIP_SELECT
  _nextByte(WRITE, JEDEC_PROG_BYTE);
  _transferAddress();
    //If data is only one byte (8 bits) long
  if (_sz == 0x01) {
    _nextByte(WRITE, *p);
    CHIP_DESELECT
  }
  else { //If data is longer than one byte (8 bits)
    uint32_t length = _sz;
    uint16_t maxBytes = SPI_PAGESIZE-(_addrIn % SPI_PAGESIZE);  // Force the first set of bytes to stay within the first page

    if (maxBytes > length) {
      for (uint16_t i = 0; i < length; ++i) {
        _nextByte(WRITE, *p++);
      }
      CHIP_DESELECT
    }
    else {
      uint32_t writeBufSz;
      uint16_t data_offset = 0;

      do {
        writeBufSz = (length<=maxBytes) ? length : maxBytes;

        for (uint16_t i = 0; i < writeBufSz; ++i) {
          _nextByte(WRITE, *p++);
        }
        CHIP_DESELECT
        if (!_addressOverflow) {
          _currentAddress += writeBufSz;
        }
        else {
          if (data_offset >= _addressOverflow) {
            _currentAddress = 0x00;
            _addressOverflow = false;
          }
        }
        data_offset += writeBufSz;
        length -= writeBufSz;
        maxBytes = SPI_PAGESIZE;   // Now we can do up to 256 bytes per loop
        if(!_notBusy() || !_writeEnable()) {
          return false;
        }
      } while (length > 0);
    }
  }

  if (!errorCheck) {
    _endSPI();
    return true;
  }
  else {
    _retVal =  _writeErrorCheck(_addr, value, _sz, _dataType);
  }
#ifdef RUNDIAGNOSTIC
  _spifuncruntime = micros() - _spifuncruntime;
#endif
  return _retVal;
}

// Reads any type of data from a specific location in the flash memory.
// Takes four arguments -
//  1. _addr --> Any address from 0 to maxAddress
//  2. T& value --> Variable to return data into
//  3. _sz --> Size of the variable in bytes (1 byte = 8 bits)
//  4. fastRead --> defaults to false - executes _beginFastRead() if set to true
template <class T> bool SPIFlash::_read(uint32_t _addr, T& value, uint32_t _sz, bool fastRead, uint8_t _dataType) {
  if (!_prep(JEDEC_READ_DATA, _addr, _sz)) {
    return false;
  }
  else {
    uint8_t* p = (uint8_t*)(void*)&value;

    if (_dataType == _STRING_) {
      char _inChar[_sz];
      _beginSPI(JEDEC_READ_DATA);
      _nextBuf(JEDEC_READ_DATA, (uint8_t*) &(*_inChar), _sz);
      _endSPI();
      for (uint16_t i = 0; i < _sz; i++) {
        *p++ = _inChar[i];
      }
    }
    else {
      CHIP_SELECT
      if (fastRead) {
        _nextByte(WRITE, JEDEC_READ_FAST);
      }
      else {
        _nextByte(WRITE, JEDEC_READ_DATA);
      }
      _transferAddress();
      for (uint16_t i = 0; i < _sz; i++) {
        *p++ =_nextByte(READ);
      }
      _endSPI();
    }
    return true;
  }
}

#endif // _SPIFLASH_H_
