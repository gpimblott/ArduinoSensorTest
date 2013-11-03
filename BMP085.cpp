/*
BMP085.cpp - Bosch BMP085 Arduino Library.
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
#include <Arduino.h>
#include <Wire.h>

#include "SensorUtils.h"
#include "BMP085.h"


/************************************************************************/
/* Constructor                                                          */
/************************************************************************/
BMP085::BMP085() {
	
	overSamplingSetting = OVER_SAMPLING_SETTING;
	ac1 = 0, ac2 = 0, ac3 = 0;
	ac4 = 0, ac5 = 0, ac6 = 0;
	b1 = 0, b2 = 0, mb = 0, mc = 0, md = 0;
	rawPressure = 0, rawTemperature = 0;
	pressureCount = 0;
	pressureFactor = 1/5.255;
	isReadPressure = false;
	rawPressureSum = 0;
	rawPressureSumCount = 0;
	
	baroAltitude      = 0.0;
	baroRawAltitude   = 0.0;
	baroGroundAltitude = 0.0;
	baroSmoothFactor   = 0.02;
}

void BMP085::requestRawPressure() {
	updateI2C(BMP085_I2C_ADDRESS, 0xF4, 0x34+(overSamplingSetting<<6));
}

long BMP085::readRawPressure() {

	writeI2C(BMP085_I2C_ADDRESS, 0xF6);
	Wire.requestFrom(BMP085_I2C_ADDRESS, 3); // request three bytes
	return (((unsigned long)Wire.read() << 16) | ((unsigned long)Wire.read() << 8) | ((unsigned long)Wire.read())) >> (8-overSamplingSetting);
}

void BMP085::requestRawTemperature() {
	updateI2C(BMP085_I2C_ADDRESS, 0xF4, 0x2E);
}

unsigned int BMP085::readRawTemperature() {
	writeI2C(BMP085_I2C_ADDRESS, 0xF6);
	return readWordAndWaitI2C(BMP085_I2C_ADDRESS);
}

// ***********************************************************
// Define all the virtual functions declared in the main class
// ***********************************************************
void BMP085::initialise() {

	// oversampling setting
	// 0 = ultra low power
	// 1 = standard
	// 2 = high
	// 3 = ultra high resolution
	overSamplingSetting = OVER_SAMPLING_SETTING;
	pressure = 0;
	baroGroundAltitude = 0;
	pressureFactor = 1/5.255;
	
	if (readWhoI2C(BMP085_I2C_ADDRESS) == 0) {
		deviceState = false;
	}
	
	writeI2C(BMP085_I2C_ADDRESS, 0xAA);
	Wire.requestFrom(BMP085_I2C_ADDRESS, 22);
	ac1 = readShortI2C();
	ac2 = readShortI2C();
	ac3 = readShortI2C();
	ac4 = readWordI2C();
	ac5 = readWordI2C();
	ac6 = readWordI2C();
	b1 = readShortI2C();
	b2 = readShortI2C();
	mb = readShortI2C();
	mc = readShortI2C();
	md = readShortI2C();
	requestRawTemperature(); // setup up next measure() for temperature
	isReadPressure = false;
	pressureCount = 0;
	measureBaro();
	delay(5); // delay for temperature
	measureBaro();
	delay(10); // delay for pressure
	measureGroundBaro();
	// check if measured ground altitude is valid
	while (abs(rawAltitude - baroGroundAltitude) > 10) {
		delay(26);
		measureGroundBaro();
	}
	baroAltitude = baroGroundAltitude;
}

void BMP085::measureBaro() {
	measureBaroSum();
	evaluateBaroAltitude();
}

void BMP085::measureBaroSum() {
	// switch between pressure and tempature measurements
	// each loop, since it's slow to measure pressure
	if (isReadPressure) {
		rawPressureSum += readRawPressure();
		rawPressureSumCount++;
		if (pressureCount == 4) {
			requestRawTemperature();
			pressureCount = 0;
			isReadPressure = false;
		}
		else {
			requestRawPressure();
		}
		pressureCount++;
	}
	else { // select must equal TEMPERATURE
		rawTemperature = (long)readRawTemperature();
		requestRawPressure();
		isReadPressure = true;
	}
}

void BMP085::evaluateBaroAltitude() {
	long x1, x2, x3, b3, b5, b6, p;
	unsigned long b4, b7;
	int32_t tmp;

	//calculate true temperature
	x1 = ((long)rawTemperature - ac6) * ac5 >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;

	if (rawPressureSumCount == 0) { // may occur at init time that no pressure have been read yet!
		return;
	}
	rawPressure = rawPressureSum / rawPressureSumCount;
	rawPressureSum = 0.0;
	rawPressureSumCount = 0;
	
	//calculate true pressure
	b6 = b5 - 4000;
	x1 = (b2 * (b6 * b6 >> 12)) >> 11;
	x2 = ac2 * b6 >> 11;
	x3 = x1 + x2;
	
	// Real Bosch formula - b3 = ((((int32_t)ac1 * 4 + x3) << overSamplingSetting) + 2) >> 2;
	// The version below is the same, but takes less program space
	tmp = ac1;
	tmp = (tmp * 4 + x3) << overSamplingSetting;
	b3 = (tmp + 2) >> 2;
	
	x1 = ac3 * b6 >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (uint32_t) (x3 + 32768)) >> 15;
	b7 = ((uint32_t) rawPressure - b3) * (50000 >> overSamplingSetting);
	p = b7 < 0x80000000 ? (b7 << 1) / b4 : (b7 / b4) << 1;
	
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	pressure = (p + ((x1 + x2 + 3791) >> 4));
	
	rawAltitude = 44330 * (1 - pow(pressure/101325.0, pressureFactor)); // returns absolute baroAltitude in meters
	baroAltitude = filterSmooth(rawAltitude, baroAltitude, baroSmoothFactor);
}

const float BMP085::getBaroAltitude() {
	return baroAltitude - baroGroundAltitude;
}

void BMP085::measureGroundBaro() {
	// measure initial ground pressure (multiple samples)
	float altSum = 0.0;
	for (int i=0; i < 25; i++) {
		measureBaro();
		altSum += baroRawAltitude;
		delay(12);
	}
	baroGroundAltitude = altSum / 25;
}

// Low pass filter, kept as regular C function for speed
float BMP085::filterSmooth(float currentData, float previousData, float smoothFactor)
{
	if (smoothFactor != 1.0) //only apply time compensated filter if smoothFactor is applied
	{
		return (previousData * (1.0 - smoothFactor) + (currentData * smoothFactor));
	}
	return currentData; //if smoothFactor == 1.0, do not calculate, just bypass!
}