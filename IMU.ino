
/*
Arduino Test harness for sensors.
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
#include <Wire.h>
#include "L3G4200D.h"
#include "HMC5883L.h"
#include "ADXL345.h"


//#define COMPASS
//#define GYRO
#define ACCEL

#ifdef ACCEL
ADXL345 accel;
#endif

// Store our compass as a variable.
#ifdef COMPASS
HMC5883L compass;
#endif

#ifdef GYRO
L3G4200D gyro;
#endif


/**
* Setup the various sensors
**/
void setup() {
	Serial.begin(9600);
	
	#ifdef GYRO
	if( setupL3G4200D() ) {
		Serial.println("L3G4200D Gyro setup ok");
		} else {
		Serial.println("L3G4200D Gyro setup FAILED");
	}
	#endif
	
	#ifdef COMPASS
	if( setupHMC5883L() ) {
		Serial.println("HMC5883L Compass setup ok");
		} else {
		Serial.println("HMC5883L Compass setup FAILED");
	}
	#endif
	
	#ifdef ACCEL
	setupADXL345();
	#endif
}



/**
* Main execution loop
*
* Keep reading the sensors and display the output
*
**/
void loop() {
	
	#ifdef GYRO
	// Digital Gyro
	readL3G4200D();
	#endif

	#ifdef COMPASS
	// Digital Compass
	readHMC5883L();
	#endif

	#ifdef ACCEL
	// Accelerometer
	readADXL345();
	#endif

	// Wait for a short time
	delay(100);
}


#ifdef ACCEL
/**
* setup the ADXL345 accelerometer
*/
void setupADXL345() {
	accel.powerOn();
}

/**
* Read some of the values from the accelerometer
*/
void readADXL345() {
	int x, y, z, i;
	double xyz[3], gains[3], gains_orig[3];
	
	accel.readAccel(&x, &y, &z);
	Serial.print("XYZ COUNTS: ");
	Serial.print(x, DEC);
	Serial.print(" ");
	Serial.print(y, DEC);
	Serial.print(" ");
	Serial.print(z, DEC);
	Serial.println("");

	accel.get_Gxyz(xyz);
	Serial.print("XYZ Gs: ");
	for(i = 0; i<3; i++){
		Serial.print(xyz[i], DEC);
		Serial.print(" ");
	}
	Serial.println("");
}

#endif


#ifdef GYRO
/**
* Setup the L3G4200D digital gyroscope
**/
boolean setupL3G4200D() {
	return gyro.setup( gyro.RANGE_250DPS);
}

/**
* Rad the gyro and output the result
*
**/
void readL3G4200D() {
	gyro.read();

	Serial.print("G ");
	Serial.print("X: ");
	Serial.print((int)gyro.g.x);
	Serial.print(" Y: ");
	Serial.print((int)gyro.g.y);
	Serial.print(" Z: ");
	Serial.println((int)gyro.g.z);
}
#endif


#ifdef COMPASS
/**
* Setup the HMC5883L digital compass
**/
boolean setupHMC5883L() {
	boolean result = true;
	int error = 0;
	compass = HMC5883L(); // Construct a new HMC5883 compass.
	
	Serial.println("Setting scale to +/- 1.3 Ga");
	error = compass.SetScale(1.3f); // Set the scale of the compass.
	
	Serial.println("Setting measurement mode to continous.");
	error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
	
	// If there is an error, print it out.
	if(error != 0) {
		Serial.println(compass.GetErrorText(error));
		result = false;
	}
	
	return result;
}

/**
* Read the digital compass and ouput the results
**/
void readHMC5883L() {
	
	// Retrive the raw values from the compass (not scaled).
	MagnetometerRaw raw = compass.ReadRawAxis();
	// Retrived the scaled values from the compass (scaled to the configured scale).
	MagnetometerScaled scaled = compass.ReadScaledAxis();
	
	// Values are accessed like so:
	int MilliGauss_OnThe_XAxis = scaled.XAxis;// (or YAxis, or ZAxis)

	// Calculate heading when the magnetometer is level, then correct for signs of axis.
	float heading = atan2(scaled.YAxis, scaled.XAxis);
	
	// Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
	// Find yours here: http://www.magnetic-declination.com/
	// Mine is: 2? 37' W, which is 2.617 Degrees, or (which we need) 0.0456752665 radians, I will use 0.0457
	// If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
	float declinationAngle = 0.0457;
	heading += declinationAngle;
	
	// Correct for when signs are reversed.
	if(heading < 0) {
		heading += 2*PI;
	}
	
	// Check for wrap due to addition of declination.
	if(heading > 2*PI) {
		heading -= 2*PI;
	}
	
	// Convert radians to degrees for readability.
	float headingDegrees = heading * 180/M_PI;

	// Output the data via the serial port.
	Serial.print("Raw:\t");
	Serial.print(raw.XAxis);
	Serial.print("   ");
	Serial.print(raw.YAxis);
	Serial.print("   ");
	Serial.print(raw.ZAxis);
	Serial.print("   \tScaled:\t");
	
	Serial.print(scaled.XAxis);
	Serial.print("   ");
	Serial.print(scaled.YAxis);
	Serial.print("   ");
	Serial.print(scaled.ZAxis);

	Serial.print("   \tHeading:\t");
	Serial.print(heading);
	Serial.print(" Radians   \t");
	Serial.print(headingDegrees);
	Serial.println(" Degrees   \t");
}

#endif