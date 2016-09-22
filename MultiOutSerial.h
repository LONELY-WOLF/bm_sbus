/*
MultiOutSerial.h (formerly NewSoftSerial.h) - 
Multi-instance software serial library for Arduino/Wiring
-- Interrupt-driven receive and other improvements by ladyada
   (http://ladyada.net)
-- Tuning, circular buffer, derivation from class Print/Stream,
   multi-instance support, porting to 8MHz processors,
   various optimizations, PROGMEM delay tables, inverse logic and 
   direct port writing by Mikal Hart (http://www.arduiniana.org)
-- Pin change interrupt macros by Paul Stoffregen (http://www.pjrc.com)
-- 20MHz processor support by Garrett Mace (http://www.macetech.com)
-- ATmega1280/2560 support by Brett Hagman (http://www.roguerobotics.com/)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

The latest version of this library can always be found at
http://arduiniana.org.
*/

#ifndef MultiOutSerial_h
#define MultiOutSerial_h

#include <inttypes.h>
#include <Stream.h>

/******************************************************************************
* Definitions
******************************************************************************/

#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

#define PARITY_NONE 0
#define PARITY_EVEN 1
#define PARITY_ODD 	2

class MultiOutSerial
{
private:
  // per object data
  uint8_t _transmitBitMask[4];
  volatile uint8_t *_transmitPortRegister[4];

  uint16_t _inverse_logic:1;
  uint8_t _stop_bits;
  uint8_t _parity;

  // private methods
  void setTX(uint8_t transmitPin, int num);
  
  static void sync();

public:
  // public methods
  MultiOutSerial(uint8_t transmitPin[], bool inverse_logic = false);
  ~MultiOutSerial();
  void begin(long speed, uint8_t parity, uint8_t stop_bits);

  virtual size_t write(uint8_t byte[]);
  virtual int read();
  virtual int available();
  virtual void flush();
  operator bool() { return true; }
};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round

#endif
