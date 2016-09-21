/*
MultiOutSerial.cpp (formerly NewSoftSerial.cpp) - 
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

// 
// Includes
// 
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <Arduino.h>
#include "MultiOutSerial.h"
#include <util/delay_basic.h>

//
// Private methods
//

/* static */ 
void MultiOutSerial::sync()
{
  while(!(TIFR2 & 2)) 
  {
  }
  TIFR2 = 2; // Reset OC flag
}

//
// Constructor
//
MultiOutSerial::MultiOutSerial(uint8_t transmitPin[], bool inverse_logic /* = false */) : 
  _inverse_logic(inverse_logic)
{
  for(int i = 0; i < 4; i++)
  {
    setTX(transmitPin[i], i);
  }
  TCCR2B = 2; // CLK / 8 (2 MHz)
  TCNT2 = 0; // Count = 0
  OCR2A = 20; // Up to 20
  TIFR2 = 2; // Reset OC flag
  TCCR2A = 2; // Start
}

//
// Destructor
//
MultiOutSerial::~MultiOutSerial()
{
  TCCR2A = 0; // Stop timer
}

void MultiOutSerial::setTX(uint8_t tx, int num)
{
  // First write, then set output. If we do this the other way around,
  // the pin would be output low for a short while before switching to
  // output high. Now, it is input with pullup for a short while, which
  // is fine. With inverse logic, either order is fine.
  digitalWrite(tx, _inverse_logic ? LOW : HIGH);
  pinMode(tx, OUTPUT);
  _transmitBitMask[num] = digitalPinToBitMask(tx);
  uint8_t port = digitalPinToPort(tx);
  _transmitPortRegister[num] = portOutputRegister(port);
}

//
// Public methods
//

void MultiOutSerial::begin(long speed, int stop_bits, int parity)
{
  _stop_bits = stop_bits;
  _parity = parity;
  TCCR2B = 2; // CLK / 8 (2 MHz)
  TCNT2 = 0; // Count = 0
  OCR2A = 20; // Up to 20
  TIFR2 = 2; // Reset OC flag
  TCCR2A = 2; // Start
}


// Read data from buffer
int MultiOutSerial::read()
{
  return -1;
}

int MultiOutSerial::available()
{

  return -1;
}

size_t MultiOutSerial::write(uint8_t b[])
{
  //uint8_t oldSREG = SREG;

  if (_inverse_logic)
  {
    b[0] = ~b[0];
    b[1] = ~b[1];
    b[2] = ~b[2];
    b[3] = ~b[3];
  }
  //cli();  // turn off interrupts for a clean txmit

  TCNT2 = 0; // Count = 0
  TIFR2 = 2; // Reset OC flag

  // Write the start bit
  if (_inverse_logic)
  {
    *_transmitPortRegister[0] |= _transmitBitMask[0];
    *_transmitPortRegister[1] |= _transmitBitMask[1];
    *_transmitPortRegister[2] |= _transmitBitMask[2];
    *_transmitPortRegister[3] |= _transmitBitMask[3];
  }
  else
  {
    *_transmitPortRegister[0] &= ~_transmitBitMask[0];
    *_transmitPortRegister[1] &= ~_transmitBitMask[1];
    *_transmitPortRegister[2] &= ~_transmitBitMask[2];
    *_transmitPortRegister[3] &= ~_transmitBitMask[3];
  }
  
  sync();

  // Write each of the 8 bits
  for (uint8_t i = 8; i > 0; --i)
  {
    uint8_t mask = 1;
    for(uint8_t n = 0; n < 0; n++)
    {
      if (b[n] & mask) // choose bit
      {
        *_transmitPortRegister[n] |= _transmitBitMask[n]; // send 1
      }
      else
      {
        *_transmitPortRegister[n] &= ~_transmitBitMask[n]; // send 0
      }
      mask <<= 1;
    }
    sync();
  }

  // restore pin to natural state
  if (_inverse_logic)
  {
    *_transmitPortRegister[0] &= ~_transmitBitMask[0];
    *_transmitPortRegister[1] &= ~_transmitBitMask[1];
    *_transmitPortRegister[2] &= ~_transmitBitMask[2];
    *_transmitPortRegister[3] &= ~_transmitBitMask[3];
  }
  else
  {
    *_transmitPortRegister[0] |= _transmitBitMask[0];
    *_transmitPortRegister[1] |= _transmitBitMask[1];
    *_transmitPortRegister[2] |= _transmitBitMask[2];
    *_transmitPortRegister[3] |= _transmitBitMask[3];
  }

  //SREG = oldSREG; // turn interrupts back on
  sync();
  
  return 1;
}

void MultiOutSerial::flush()
{
  // There is no tx buffering, simply return
}
