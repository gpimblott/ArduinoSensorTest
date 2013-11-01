/*
 * SensorUtils.cpp
 *
 * Created: 01/11/2013 21:50:50
 *  Author: gordon
 */ 

#include "SensorUtils.h"

void writeI2C(int id , int address, int data)
{
	Wire.beginTransmission(id);
	Wire.write(address);
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