/*
BMP085.h - Header file for the Bosch BMP085 Arduino Library.
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
#ifndef _BMP085_
#define _BMP085_


// This sets up a BMP085
// Code from http://wiring.org.co/learning/libraries/bmp085.html
// Also made bug fixes based on BMP085 library from Jordi Munoz and Jose Julio

#define BMP085_I2C_ADDRESS 0x77

#define TEMPERATURE 0
#define PRESSURE 1
#define OVER_SAMPLING_SETTING 1 // use to be 3


/************************************************************************/
/*  Class definition for BMP085 pressure sensor                         */
/************************************************************************/
class BMP085
{
	private:
	boolean deviceState;
	
	byte overSamplingSetting;
	int ac1, ac2, ac3;
	unsigned int ac4, ac5, ac6;
	int b1, b2, mb, mc, md;
	
	long rawPressure;
	long rawTemperature;
	long rawAltitude;
	long pressure;
	
	float baroAltitude;
	float baroRawAltitude;
	float baroGroundAltitude;
	float baroSmoothFactor;
	
	byte pressureCount;
	float pressureFactor;
	boolean isReadPressure;
	float rawPressureSum;
	byte rawPressureSumCount;
	
	public:
	
	BMP085();
	void initialise();
	
	unsigned int readRawTemperature();
	void requestRawTemperature();
	long readRawPressure();
	void requestRawPressure();
	
	const float getBaroAltitude();
	void measureGroundBaro();
	void evaluateBaroAltitude();
	void measureBaroSum();
	void measureBaro();
	
	float filterSmooth(float currentData, float previousData, float smoothFactor);

};

#endif