/*
I2C Sensor Library.
Copyright (C) 2013 G.Pimblott

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Most of the credit goes to the ADAFruit unified sensor library which is available on GitHub

*/
#include <Wire.h>
#include "Sensor.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
void Sensor::updateI2C( int dataAddress, byte data)
{
	Wire.beginTransmission((uint8_t)deviceAddress);
	Wire.write(dataAddress);
	Wire.write(data);
	Wire.endTransmission();
}

/**************************************************************************/
/* Write a command to the sensor                                          */
/**************************************************************************/
void Sensor::writeCommand(byte reg, byte value)
{
  Wire.beginTransmission((uint8_t)deviceAddress);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)value);
  Wire.endTransmission();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void Sensor::writeI2C( byte data) {
	Wire.beginTransmission((uint8_t)deviceAddress);
	Wire.write(data);
	Wire.endTransmission();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
byte Sensor::readI2C() {

	Wire.requestFrom((uint8_t)deviceAddress, (uint8_t)1);
	return Wire.read();
}

byte Sensor::readWhoI2C() {
	writeI2C( (byte)0 );
	delay(100);
	Wire.requestFrom((uint8_t)deviceAddress, (uint8_t)1);
	return Wire.read();
}

/**************************************************************************/

/**************************************************************************/
void Sensor::read8(byte reg, uint8_t *value)
{
  Wire.beginTransmission((uint8_t)deviceAddress);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)deviceAddress, (byte)1);
  *value = Wire.read(); 
  Wire.endTransmission();
}

/**************************************************************************/

/**************************************************************************/
void Sensor::read16(byte reg, uint16_t *value)
{
  Wire.beginTransmission((uint8_t)deviceAddress);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)deviceAddress, (byte)2);
  *value = (Wire.read() << 8) | Wire.read();
  Wire.endTransmission();
}


/**************************************************************************/
/**************************************************************************/
void Sensor::readS16(byte reg, int16_t *value)
{
  uint16_t i;
  read16(reg, &i);
  *value = (int16_t)i;
}