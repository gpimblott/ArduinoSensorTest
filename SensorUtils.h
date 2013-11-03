/*
Header file for the I2C Library.
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

*/
#include "Arduino.h"

#ifndef SENSORUTILS_H_
#define SENSORUTILS_H_


void writeI2C(int address, byte data);
void updateI2C(int deviceAddress , int dataaddress, byte data);
byte readI2C(int address );
byte readWhoI2C(int deviceAddress);

int readShortI2C();
int readWordI2C();
int readWordAndWaitI2C(int deviceAddress);

#endif /* SENSORUTILS_H_ */