
#include <Wire.h>
#include "L3G4200D.h"
#include "HMC5883L.h"

// Store our compass as a variable.
HMC5883L compass;
L3G4200D gyro;

int error = 0;


void setup() {
	Serial.begin(9600);
	
	if( !gyro.setup( gyro.RANGE_250DPS) ) {
		Serial.println("Failed to setup connection to gyro");
		while(1);
	}
	
	compass = HMC5883L(); // Construct a new HMC5883 compass.
	
	Serial.println("Setting scale to +/- 1.3 Ga");
	error = compass.SetScale(1.3); // Set the scale of the compass.
	
	Serial.println("Setting measurement mode to continous.");
	error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
	
	if(error != 0) // If there is an error, print it out.
	Serial.println(compass.GetErrorText(error));
}

void loop() {
	gyro.read();

	Serial.print("G ");
	Serial.print("X: ");
	Serial.print((int)gyro.g.x);
	Serial.print(" Y: ");
	Serial.print((int)gyro.g.y);
	Serial.print(" Z: ");
	Serial.println((int)gyro.g.z);

	
	
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
	if(heading < 0)
	heading += 2*PI;
	
	// Check for wrap due to addition of declination.
	if(heading > 2*PI)
	heading -= 2*PI;
	
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
	
	delay(100);
}

