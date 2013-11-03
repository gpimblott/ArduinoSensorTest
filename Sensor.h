/*
Header file for the I2C Sesnor Library.
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

#ifndef SENSOR_H_
#define SENSOR_H_

#include "Arduino.h"

/* Constants */
#define SENSORS_GRAVITY_EARTH (9.80665F) /**< Earth's gravity in m/s^2 */
#define SENSORS_GRAVITY_MOON (1.6F) /**< The moon's gravity in m/s^2 */
#define SENSORS_GRAVITY_SUN (275.0F) /**< The sun's gravity in m/s^2 */
#define SENSORS_GRAVITY_STANDARD (SENSORS_GRAVITY_EARTH)
#define SENSORS_MAGFIELD_EARTH_MAX (60.0F) /**< Maximum magnetic field on Earth's surface */
#define SENSORS_MAGFIELD_EARTH_MIN (30.0F) /**< Minimum magnetic field on Earth's surface */
#define SENSORS_PRESSURE_SEALEVELHPA (1013.25F) /**< Average sea level pressure is 1013.25 hPa */
#define SENSORS_DPS_TO_RADS (0.017453293F) /**< Degrees/s to rad/s multiplier */
#define SENSORS_GAUSS_TO_MICROTESLA (100) /**< Gauss to micro-Tesla multiplier */

/** Sensor types */
typedef enum
{
	SENSOR_TYPE_ACCELEROMETER = (1), /**< Gravity + linear acceleration */
	SENSOR_TYPE_MAGNETIC_FIELD = (2),
	SENSOR_TYPE_ORIENTATION = (3),
	SENSOR_TYPE_GYROSCOPE = (4),
	SENSOR_TYPE_LIGHT = (5),
	SENSOR_TYPE_PRESSURE = (6),
	SENSOR_TYPE_PROXIMITY = (8),
	SENSOR_TYPE_GRAVITY = (9),
	SENSOR_TYPE_LINEAR_ACCELERATION = (10), /**< Acceleration not including gravity */
	SENSOR_TYPE_ROTATION_VECTOR = (11),
	SENSOR_TYPE_RELATIVE_HUMIDITY = (12),
	SENSOR_TYPE_AMBIENT_TEMPERATURE = (13),
	SENSOR_TYPE_VOLTAGE = (15),
	SENSOR_TYPE_CURRENT = (16),
	SENSOR_TYPE_COLOR = (17)
} sensors_type_t;

/** struct sensors_vec_s is used to return a vector in a common format. */
typedef struct {
	union {
		float v[3];
		struct {
			float x;
			float y;
			float z;
		};
		/* Orientation sensors */
		struct {
			float azimuth; /**< Angle between the magnetic north direction and the Y axis, around the Z axis (0<=azimuth<360). 0=North, 90=East, 180=South, 270=West */
			float pitch; /**< Rotation around X axis (-180<=pitch<=180), with positive values when the z-axis moves toward the y-axis. */
			float roll; /**< Rotation around Y axis (-90<=roll<=90), with positive values when the x-axis moves towards the z-axis. */
		};
	};
	int8_t status;
	uint8_t reserved[3];
} sensors_vec_t;

/** struct sensors_color_s is used to return color data in a common format. */
typedef struct {
	union {
		float c[3];
		/* RGB color space */
		struct {
			float r; /**< Red component */
			float g; /**< Green component */
			float b; /**< Blue component */
		};
	};
	uint32_t rgba; /**< 24-bit RGBA value */
} sensors_color_t;

/* Sensor event (36 bytes) */
/** struct sensor_event_s is used to provide a single sensor event in a common format. */
typedef struct
{
	int32_t version; /**< must be sizeof(struct sensors_event_t) */
	int32_t sensor_id; /**< unique sensor identifier */
	int32_t type; /**< sensor type */
	int32_t reserved0; /**< reserved */
	int32_t timestamp; /**< time is in milliseconds */
	union
	{
		float data[4];
		sensors_vec_t acceleration; /**< acceleration values are in meter per second per second (m/s^2) */
		sensors_vec_t magnetic; /**< magnetic vector values are in micro-Tesla (uT) */
		sensors_vec_t orientation; /**< orientation values are in degrees */
		sensors_vec_t gyro; /**< gyroscope values are in rad/s */
		float temperature; /**< temperature is in degrees centigrade (Celsius) */
		float distance; /**< distance in centimeters */
		float light; /**< light in SI lux units */
		float pressure; /**< pressure in hectopascal (hPa) */
		float relative_humidity; /**< relative humidity in percent */
		float current; /**< current in milliamps (mA) */
		float voltage; /**< voltage in volts (V) */
		sensors_color_t color; /**< color in RGB component values */
	};
} sensors_event_t;

/* Sensor details (40 bytes) */
/** struct sensor_s is used to describe basic information about a specific sensor. */
typedef struct
{
	char name[12]; /**< sensor name */
	int32_t version; /**< version of the hardware + driver */
	int32_t sensor_id; /**< unique sensor identifier */
	int32_t type; /**< this sensor's type (ex. SENSOR_TYPE_LIGHT) */
	float max_value; /**< maximum value of this sensor's value in SI units */
	float min_value; /**< minimum value of this sensor's value in SI units */
	float resolution; /**< smallest difference between two values reported by this sensor */
	int32_t min_delay; /**< min delay in microseconds between events. zero = not a constant rate */
} sensor_t;

/************************************************************************/
/* Represents an individual sensor                                      */
/************************************************************************/
class Sensor {
	public:
	// Constructor(s)
	Sensor(uint8_t da, int32_t di)
	{
		deviceAddress = da;
		deviceId = di;
	};

	void writeI2C( byte data);
	byte readI2C();
	void updateI2C( int dataaddress, byte data);
	byte readWhoI2C();

	void writeCommand(byte reg, byte value);
	void read8(byte reg, uint8_t *value);
	void read16(byte reg, uint16_t *value);
	void readS16(byte reg, int16_t *value);
	

	// These must be defined by the subclass
	virtual void getEvent(sensors_event_t*);
	virtual void getSensor(sensor_t*);
	
	protected:
	uint8_t deviceAddress;
	int32_t deviceId;
};



byte readI2C(int address );
byte readWhoI2C(int deviceAddress);

int readShortI2C();
int readWordI2C();
int readWordAndWaitI2C(int deviceAddress);

#endif /* SENSORUTILS_H_ */