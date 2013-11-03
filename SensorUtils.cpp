/*
 * SensorUtils.cpp
 *
 * Created: 01/11/2013 21:50:50
 *  Author: gordon
 */ 
#include <Wire.h>
#include "SensorUtils.h"

void updateI2C(int deviceAddress , int dataAddress, byte data)
{
	Wire.beginTransmission(deviceAddress);
	Wire.write(dataAddress);
	Wire.write(data);
	Wire.endTransmission();
}


void writeI2C(int address, byte data) {

	Wire.beginTransmission(address);
	Wire.write(data);
	Wire.endTransmission();
}

byte readI2C(int address) {

	Wire.requestFrom(address, 1);
	return Wire.read();
}

byte readWhoI2C(int deviceAddress) {

	// read the ID of the I2C device
	Wire.beginTransmission(deviceAddress);
	Wire.write((byte)0);
	Wire.endTransmission();
	delay(100);
	Wire.requestFrom(deviceAddress, 1);
	return Wire.read();
}

int readShortI2C() {

	return (signed short)readWordI2C();
}

int readWordI2C() {
	return (Wire.read() << 8) | Wire.read();
}

int readWordAndWaitI2C(int deviceAddress) {

	Wire.requestFrom(deviceAddress, 2); // request two bytes
	while(!Wire.available()); // wait until data available
	unsigned char msb = Wire.read();
	while(!Wire.available()); // wait until data available
	unsigned char lsb = Wire.read();
	return (((int)msb<<8) | ((int)lsb));
}