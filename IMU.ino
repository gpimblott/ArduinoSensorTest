
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
#include "BMP085.h"


#define COMPASS
//#define GYRO
//#define ACCEL
#define PRESSURE

#ifdef ACCEL
ADXL345 accel;
#endif

#ifdef COMPASS
HMC5883L compass = HMC5883L();
#endif

#ifdef GYRO
L3G4200D gyro;
#endif

#ifdef PRESSURE
BMP085 bmp = BMP085(10085);
#endif

/********************************************************/
/* Helper routine to output sensor details
/********************************************************/
void displaySensorDetails(sensor_t sensor)
{
	Serial.println("------------------------------------");
	Serial.print ("Sensor: "); Serial.println(sensor.name);
	Serial.print ("Driver Ver: "); Serial.println(sensor.version);
	Serial.print ("Unique ID: "); Serial.println(sensor.sensor_id);
	Serial.print ("Max Value: "); Serial.println(sensor.max_value);
	Serial.print ("Min Value: "); Serial.println(sensor.min_value);
	Serial.print ("Resolution: "); Serial.println(sensor.resolution);
	Serial.println("------------------------------------");
	Serial.println("");
	delay(500);
}

/**
 * Setup the BMP085 pressure sensor
 **/
#ifdef PRESSURE

void setupBMP085() {
	Serial.println("Initialising BMP085");
	if(!bmp.begin())
	{
		/* There was a problem detecting the BMP085 ... check your connections */
		Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
		while(1);
	}
	
	sensor_t sensor;
	bmp.getSensor(&sensor);
	
	displaySensorDetails(sensor);
}

/*****************************************************************/
/* Read from the BMP085 pressure sensor
/*****************************************************************/
void readBMP085() {
	
	/* Get a new sensor event */
	sensors_event_t event;
	bmp.getEvent(&event);
	
	/* Display the results (barometric pressure is measure in hPa) */
	if (event.pressure)
	{
		/* Display atmospheric pressue in hPa */
		Serial.print("Pressure: ");
		Serial.print(event.pressure);
		Serial.println(" hPa");
		
		/* Calculating altitude with reasonable accuracy requires pressure *
		* sea level pressure for your position at the moment the data is *
		* converted, as well as the ambient temperature in degress *
		* celcius. If you don't have these values, a 'generic' value of *
		* 1013.25 hPa can be used (defined as SENSORS_PRESSURE_SEALEVELHPA *
		* in sensors.h), but this isn't ideal and will give variable *
		* results from one day to the next. *
		* *
		* You can usually find the current SLP value by looking at weather *
		* websites or from environmental information centers near any major *
		* airport. *
		* *
		* For example, for Paris, France you can check the current mean *
		* pressure and sea level at: http://bit.ly/16Au8ol */
		
		/* First we get the current temperature from the BMP085 */
		float temperature;
		bmp.getTemperature(&temperature);
		Serial.print("Temperature: ");
		Serial.print(temperature);
		Serial.println(" C");

		/* Then convert the atmospheric pressure, SLP and temp to altitude */
		/* Update this next line with the current SLP for better results */
		float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
		Serial.print("Altitude: ");
		Serial.print(bmp.pressureToAltitude(seaLevelPressure,
		event.pressure,
		temperature));
		Serial.println(" m");
		Serial.println("");
	}
	else
	{
		Serial.println("Sensor error");
	}
	
	
}

#endif

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

/**********************************************/
/* Setup the compass */
/**********************************************/
boolean setupHMC5883L() {
	boolean result = true;
	int error = 0;
	
	// Start the sensor
	if(!compass.begin() )
	{
		/* There was a problem detecting the BMP085 ... check your connections */
		Serial.print("Ooops, no HMC883L detected ... Check your wiring or I2C ADDR!");
		while(1);
	}
	
	// Display the sensor
	sensor_t sensor;
	compass.getSensor(&sensor);
	displaySensorDetails(sensor);
	
	// Set some defaults
	error = compass.SetScale(1.3f); // Set the scale of the compass.
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
	
	/* Get a new sensor event */
	sensors_event_t event;
	compass.getEvent(&event);
	
	/* Display the results (barometric pressure is measure in hPa) */
	if (event.type == SENSOR_TYPE_MAGNETIC_FIELD)
	{
		// Calculate heading when the magnetometer is level, then correct for signs of axis.
		float heading = atan2(event.orientation.y, event.orientation.x);
		
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

		Serial.print("   \tHeading:\t");
		Serial.print(heading);
		Serial.print(" Radians   \t");
		Serial.print(headingDegrees);
		Serial.println(" Degrees   \t");
	}
}

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
	
	#ifdef PRESSURE
	setupBMP085();
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
	
	#ifdef PRESSURE
	readBMP085();
	#endif

	// Wait for a short time
	delay(100);
}
