/* Arduino SPIFlash Library v.3.1.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 24/02/2018
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

#include "SPIFlash.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Private functions used by read, write and erase operations     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//Checks to see if page overflow is permitted and assists with determining next address to read/write.
//Sets the global address variable
bool SPIFlash::_addressCheck(uint32_t _addr, uint32_t size) {
  uint32_t _submittedAddress = _addr;
  if (errorcode == VOYNICH_STATUS_UNKNOWNCAPACITY || errorcode == NORESPONSE) {
    return false;
  }
	if (!_chip.capacity) {
    _troubleshoot(VOYNICH_STATUS_CALLBEGIN);
    return false;
	}

  if (_submittedAddress + size >= _chip.capacity) {
  #ifdef DISABLEOVERFLOW
    _troubleshoot(VOYNICH_STATUS_OUTOFBOUNDS);
    return false;					// At end of memory - (!pageOverflow)
  #else
    _addressOverflow = ((_submittedAddress + size) - _chip.capacity);
    _currentAddress = _addr;
    return true;					// At end of memory - (pageOverflow)
  #endif
  }
  else {
    _addressOverflow = false;
    _currentAddress = _addr;
    return true;				// Not at end of memory if (address < _chip.capacity)
  }
}

// Checks to see if the block of memory has been previously written to
bool SPIFlash::_notPrevWritten(uint32_t _addr, uint32_t size) {
  uint8_t _dat;
  _beginSPI(JEDEC_READ_DATA);
  for (uint32_t i = 0; i < size; i++) {
    if (_nextByte(READ) != 0xFF) {
      CHIP_DESELECT;
      _troubleshoot(PREVWRITTEN);
      return false;
    }
  }
  CHIP_DESELECT
  return true;
}

//Double checks all parameters before calling a read or write. Comes in two variants
//Takes address and returns the address if true, else returns false. Throws an error if there is a problem.
bool SPIFlash::_prep(uint8_t opcode, uint32_t _addr, uint32_t size) {
  // If the flash memory is >= 256 MB enable 4-byte addressing
  if (_chip.manufacturerID == WINBOND_MANID && _addr >= MB(16)) {
    if (!_enable4ByteAddressing()) {    // If unable to enable 4-byte addressing
      return false;
    }
  }
  switch (opcode) {
    case JEDEC_PROG_BYTE:
    #ifndef HIGHSPEED
      if(_isChipPoweredDown() || !_addressCheck(_addr, size) || !_notPrevWritten(_addr, size) || !_notBusy() || !_writeEnable()) {
        return false;
      }
    #else
      if (_isChipPoweredDown() || !_addressCheck(_addr, size) || !_notBusy() || !_writeEnable()) {
        return false;
      }
    #endif
    return true;
    break;

    case ERASEFUNC:
    if(_isChipPoweredDown() || !_addressCheck(_addr, size) || !_notBusy() || !_writeEnable()) {
      return false;
    }
    return true;
    break;

    default:
      if (_isChipPoweredDown() || !_addressCheck(_addr, size) || !_notBusy()) {
        return false;
      }
    #ifdef ENABLEZERODMA
      _delay_us(3500L);
    #endif
    return true;
    break;
  }
}

// Transfer Address.
bool SPIFlash::_transferAddress() {
  if (address4ByteEnabled) {
    _nextByte(WRITE, ADDR_BITS_4(_currentAddress));
  }
  _nextByte(WRITE, ADDR_BITS_3(_currentAddress));
  _nextByte(WRITE, ADDR_BITS_2(_currentAddress));
  _nextByte(WRITE, ADDR_BITS_1(_currentAddress));
  return true;
}

bool SPIFlash::_startSPIBus() {
  SPI.beginTransaction(_settings);
  SPIBusState = true;
  return true;
}

// Initiates SPI operation - but data is not transferred yet. Always call _prep() before this function (especially when it involves writing or reading to/from an address)
bool SPIFlash::_beginSPI(uint8_t opcode) {
  if (!SPIBusState) {
    _startSPIBus();
  }

  // SPI data lines are left open until _endSPI() is called
  CHIP_SELECT
  switch (opcode) {
    case JEDEC_READ_DATA:
    _nextByte(WRITE, opcode);
    _transferAddress();
    break;

    case JEDEC_PROG_BYTE:
    _nextByte(WRITE, opcode);
    _transferAddress();
    break;

    case JEDEC_READ_FAST:
    _nextByte(WRITE, opcode);
    _nextByte(WRITE, DUMMYBYTE);
    _transferAddress();
    break;

    case JEDEC_ERASE_SECTOR:
    _nextByte(WRITE, opcode);
    _transferAddress();
    break;

    case JEDEC_ERASE_BLOCK_32:
    _nextByte(WRITE, opcode);
    _transferAddress();
    break;

    case JEDEC_ERASE_BLOCK_64:
    _nextByte(WRITE, opcode);
    _transferAddress();
    break;

    default:
    _nextByte(WRITE, opcode);
    break;
  }
  return true;
}


//Reads/Writes next byte. Call 'n' times to read/write 'n' number of bytes. Should be called after _beginSPI()
uint8_t SPIFlash::_nextByte(char IOType, uint8_t data) {
  return SPI.transfer(data);
}

//Reads/Writes next int. Call 'n' times to read/write 'n' number of integers. Should be called after _beginSPI()
uint16_t SPIFlash::_nextInt(uint16_t data) {
  return SPI.transfer16(data);
}

//Reads/Writes next data buffer. Should be called after _beginSPI()
void SPIFlash::_nextBuf(uint8_t opcode, uint8_t *data_buffer, uint32_t size) {
  uint8_t *_dataAddr = &(*data_buffer);
  switch (opcode) {
    case JEDEC_READ_DATA:
      for (uint16_t i = 0; i < size; i++) {
        *_dataAddr = SPI.transfer(NULLBYTE);
        _dataAddr++;
      }
      break;

    case JEDEC_PROG_BYTE:
      for (uint16_t i = 0; i < size; i++) {
        SPI.transfer(*_dataAddr);
        _dataAddr++;
      }
      break;
  }
}

//Stops all operations. Should be called after all the required data is read/written from repeated _nextByte() calls
void SPIFlash::_endSPI() {
  CHIP_DESELECT

  if (address4ByteEnabled) {          // If the previous operation enabled 4-byte addressing, disable it
    _disable4ByteAddressing();
  }

#ifdef SPI_HAS_TRANSACTION
  SPI.endTransaction();
#else
  interrupts();
#endif
  
  SPIBusState = false;
}

// Checks if status register 1 can be accessed - used to check chip status, during powerdown and power up and for debugging
uint8_t SPIFlash::_readStat1() {
  _beginSPI(JEDEC_READ_STATREG);
  stat1 = _nextByte(READ);
  CHIP_DESELECT
  return stat1;
}

// Checks if status register 2 can be accessed, if yes, reads and returns it
uint8_t SPIFlash::_readStat2() {
  _beginSPI(WINBOND_READ_STATREG_2);
  stat2 = _nextByte(READ);
  CHIP_DESELECT
  return stat2;
}

// Checks if status register 3 can be accessed, if yes, reads and returns it
uint8_t SPIFlash::_readStat3() {
  _beginSPI(WINBOND_READ_STATREG_3);
  stat3 = _nextByte(READ);
  CHIP_DESELECT
  return stat3;
}

// Checks to see if 4-byte addressing is already enabled and if not, enables it
bool SPIFlash::_enable4ByteAddressing() {
  if (_readStat3() & ADS) {
    return true;
  }
  _beginSPI(JEDEC_SET_4_BYTE_ADDR_ENABLE);
  CHIP_DESELECT
  if (_readStat3() & ADS) {
    address4ByteEnabled = true;
    return true;
  }
  else {
    _troubleshoot(UNABLETO4BYTE);
    return false;
  }
}

// Checks to see if 4-byte addressing is already disabled and if not, disables it
bool SPIFlash::_disable4ByteAddressing() {
  if (!(_readStat3() & ADS)) {      // If 4 byte addressing is disabled (default state)
    return true;
  }
  _beginSPI(JEDEC_SET_4_BYTE_ADDR_DISABLE);
  CHIP_DESELECT
  if (_readStat3() & ADS) {
    _troubleshoot(UNABLETO3BYTE);
    return false;
  }
  address4ByteEnabled = false;
  return true;
}

// Checks the erase/program suspend flag before enabling/disabling a program/erase suspend operation
bool SPIFlash::_noSuspend() {
  switch (_chip.manufacturerID) {
    case WINBOND_MANID:
    if(_readStat2() & SUS) {
      _troubleshoot(SYSSUSPEND);
  		return false;
    }
  	return true;
    break;

    case MICROCHIP_MANID:
    _readStat1();
    if(stat1 & WSE || stat1 & WSP) {
      _troubleshoot(SYSSUSPEND);
  		return false;
    }
  }
  return true;
}

// Checks to see if chip is powered down. If it is, retrns true. If not, returns false.
bool SPIFlash::_isChipPoweredDown() {
  if (chipPoweredDown) {
    _troubleshoot(CHIPISPOWEREDDOWN);
    return true;
  }
  else {
    return false;
  }
}

// Polls the status register 1 until busy flag is cleared or timeout
bool SPIFlash::_notBusy(uint32_t timeout) {
  _delay_us(SPI_WRITE_DELAY);
  uint32_t _time = micros();
  do {
    _readStat1();
    if (!(stat1 & BUSY))
    {
      return true;
    }
    _time++;
  } while ((micros() - _time) < timeout);
  if (timeout <= (micros() - _time)) {
    return false;
  }
  return true;
}

//Enables writing to chip by setting the JEDEC_SET_WRITE_ENABLE bit
bool SPIFlash::_writeEnable(bool _troubleshootEnable) {
  _beginSPI(JEDEC_SET_WRITE_ENABLE);
  CHIP_DESELECT
  if (!(_readStat1() & WRTEN)) {
    if (_troubleshootEnable) {
      _troubleshoot(CANTENWRITE);
    }
    return false;
  }
  return true;
}

//Disables writing to chip by setting the Write Enable Latch (WEL) bit in the Status Register to 0
//_writeDisable() is not required under the following conditions because the Write Enable Latch (WEL) flag is cleared to 0
// i.e. to write disable state:
// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, Block Erase, Chip Erase, Write Status Register,
// Erase Security Register and Program Security register
bool SPIFlash::_writeDisable() {
	_beginSPI(JEDEC_SET_WRITE_DISABLE);
  CHIP_DESELECT
	return true;
}

//Checks the device ID to establish storage parameters
bool SPIFlash::_getManId(uint8_t *b1, uint8_t *b2) {
	if(!_notBusy())
		return false;
	_beginSPI(JEDEC_READ_MANSIG);
  _nextByte(READ);
  _nextByte(READ);
  _nextByte(READ);
  *b1 = _nextByte(READ);
  *b2 = _nextByte(READ);
  CHIP_DESELECT
	return true;
}

//Checks for presence of chip by requesting JEDEC ID
bool SPIFlash::_getJedecId() {
  if(!_notBusy()) {
  	return false;
  }
  _beginSPI(JEDEC_READ_JEDECID);
	_chip.manufacturerID = _nextByte(READ);		// manufacturer id
	_chip.memoryTypeID = _nextByte(READ);		// memory type
	_chip.capacityID = _nextByte(READ);		// capacity
  CHIP_DESELECT
  if (!_chip.manufacturerID) {
    _troubleshoot(NORESPONSE);
    return false;
  }
  else {
    return true;
  }
}

bool SPIFlash::_getSFDP() {
  if(!_notBusy()) {
  	return false;
  }
  _currentAddress = 0x00;
  _beginSPI(JEDEC_READ_SFDP);
  _transferAddress();
  _nextByte(WRITE, DUMMYBYTE);

  ADDR_BITS_1(_chip.sfdp) = _nextByte(READ);
  ADDR_BITS_2(_chip.sfdp) = _nextByte(READ);
  ADDR_BITS_3(_chip.sfdp) = _nextByte(READ);
  ADDR_BITS_4(_chip.sfdp) = _nextByte(READ);
  CHIP_DESELECT

  return _chip.sfdp == VOYNICH_SFDP_SIGNATURE;
}

bool SPIFlash::_disableGlobalBlockProtect() {
  if (_chip.memoryTypeID == MICROCHIP_SST25) {
    _readStat1();
    uint8_t _tempStat1 = stat1 & 0xC3;
    _beginSPI(JEDEC_SET_WRITE_STATREG_ENABLE);
    CHIP_DESELECT
    _beginSPI(JEDEC_PROG_STATREG);
    _nextByte(WRITE, _tempStat1);
    CHIP_DESELECT
  }
  else if (_chip.memoryTypeID == MICROCHIP_SST26) {
    if(!_notBusy()) {
    	return false;
    }
    _writeEnable();
    _delay_us(10);
    _beginSPI(ULBPR);
    CHIP_DESELECT
    _delay_us(50);
    _writeDisable();
  }
  return true;
}

//Identifies the chip
bool SPIFlash::_chipID() {
  //Get Manfucturer/Device ID so the library can identify the chip
  //_getSFDP();
  if (!_getJedecId()) {
    return false;
  }

  if (_chip.manufacturerID == MICROCHIP_MANID) {
    _disableGlobalBlockProtect();
  }

  if (!_chip.capacity) {
    if (_chip.manufacturerID == WINBOND_MANID || _chip.manufacturerID == MICROCHIP_MANID || _chip.manufacturerID == CYPRESS_MANID || _chip.manufacturerID == ADESTO_MANID || _chip.manufacturerID == MICRON_MANID) {
      //Identify capacity
      for (uint8_t i = 0; i < sizeof(_capID); i++) {
        if (_chip.capacityID == _capID[i]) {
          _chip.capacity = (_memSize[i]);
          _chip.supported = true;
          return true;
        }
      }
      if (!_chip.capacity) {
        _troubleshoot(VOYNICH_STATUS_UNKNOWNCAPACITY);
        return false;
      }
    }
    else {
      _troubleshoot(VOYNICH_STATUS_UNKNOWNCHIP); //Error code for unidentified capacity
      return false;
    }
  }

  return true;
}
