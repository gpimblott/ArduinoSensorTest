/*
HMC5883L.cpp - Class file for the HMC5883L Triple Axis Magnetometer Arduino Library.
Copyright (C) 2011 Love Electronics (loveelectronics.co.uk)

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
#include "HMC5883L.h"


/**************************************************************************/
/* Perform any setup for the sensor                                       */
/**************************************************************************/
bool HMC5883L::begin()
{
	// Enable I2C
	Wire.begin();

	/* Make sure we have the right device */
	uint8_t id;
	read8(IdenificationRegisterA, &id);

	Serial.print("ID:"); Serial.println(id);

	return true;
}


/**************************************************************************/
/* Return the details for the sensor                                      */
/**************************************************************************/
void HMC5883L::getSensor(sensor_t *sensor)
{
	/* Clear the sensor_t object */
	memset(sensor, 0, sizeof(sensor_t));

	/* Insert the sensor name in the fixed length char array */
	strncpy (sensor->name, "HMC5883L", sizeof(sensor->name) - 1);
	sensor->name[sizeof(sensor->name)- 1] = 0;
	sensor->version     = 1;
	sensor->sensor_id   = deviceId;
	sensor->type        = SENSOR_TYPE_MAGNETIC_FIELD;
	sensor->min_delay   = 0;
	sensor->max_value   = 0;
	sensor->min_value   = 359.9F;
	sensor->resolution  = 0.01F;
}

/**************************************************************************/
/* Get the next reading from the sensor                                   */
/**************************************************************************/
void HMC5883L::getEvent(sensors_event_t *event)
{
	float pressure_kPa;

	/* Clear the event */
	memset(event, 0, sizeof(sensors_event_t));

	event->version   = sizeof(sensors_event_t);
	event->sensor_id = deviceId;
	event->type      = SENSOR_TYPE_MAGNETIC_FIELD;
	event->timestamp = 0;
	
	
	// Retrived the scaled values from the compass (scaled to the configured scale).
	MagnetometerScaled scaled = ReadScaledAxis();
	
	event->orientation.x = scaled.XAxis;
	event->orientation.y = scaled.YAxis;
	event->orientation.z = scaled.ZAxis;
}

/************************************************************************/
/*  Read the raw data from the sensor                                   */
/************************************************************************/
MagnetometerRaw HMC5883L::ReadRawAxis()
{
	uint8_t* buffer = Read(DataRegisterBegin, 6);
	MagnetometerRaw raw = MagnetometerRaw();
	raw.XAxis = (buffer[0] << 8) | buffer[1];
	raw.ZAxis = (buffer[2] << 8) | buffer[3];
	raw.YAxis = (buffer[4] << 8) | buffer[5];
	return raw;
}

/************************************************************************/
/*  Get the scales values                                               */
/************************************************************************/
MagnetometerScaled HMC5883L::ReadScaledAxis()
{
	MagnetometerRaw raw = ReadRawAxis();
	MagnetometerScaled scaled = MagnetometerScaled();
	scaled.XAxis = raw.XAxis * m_Scale;
	scaled.ZAxis = raw.ZAxis * m_Scale;
	scaled.YAxis = raw.YAxis * m_Scale;
	return scaled;
}

/************************************************************************/
/* Set the scale to use                                                 */
/************************************************************************/
int HMC5883L::SetScale(float gauss)
{
	uint8_t regValue = 0x00;
	if(gauss == 0.88)
	{
		regValue = 0x00;
		m_Scale = 0.73;
	}
	else if(gauss == 1.3)
	{
		regValue = 0x01;
		m_Scale = 0.92;
	}
	else if(gauss == 1.9)
	{
		regValue = 0x02;
		m_Scale = 1.22;
	}
	else if(gauss == 2.5)
	{
		regValue = 0x03;
		m_Scale = 1.52;
	}
	else if(gauss == 4.0)
	{
		regValue = 0x04;
		m_Scale = 2.27;
	}
	else if(gauss == 4.7)
	{
		regValue = 0x05;
		m_Scale = 2.56;
	}
	else if(gauss == 5.6)
	{
		regValue = 0x06;
		m_Scale = 3.03;
	}
	else if(gauss == 8.1)
	{
		regValue = 0x07;
		m_Scale = 4.35;
	}
	else
	return ErrorCode_1_Num;
	
	// Setting is in the top 3 bits of the register.
	regValue = regValue << 5;
	writeCommand(ConfigurationRegisterB, regValue);
}

/************************************************************************/
/* Set the measurement mode to use                                      */
/************************************************************************/
int HMC5883L::SetMeasurementMode(uint8_t mode)
{
	writeCommand(ModeRegister, mode);
	return 0;
}

/************************************************************************/
/* Read a number of bytes from the sensor after sending the correct 
 * command  */
/************************************************************************/
uint8_t* HMC5883L::Read(int address, int length)
{
	writeI2C( address );
	
	Wire.beginTransmission(HMC5883L_Address);
	Wire.requestFrom(HMC5883L_Address, length);

	uint8_t buffer[length];
	if(Wire.available() == length)
	{
		for(uint8_t i = 0; i < length; i++)
		{
			buffer[i] = Wire.read();
		}
	}
	Wire.endTransmission();

	return buffer;
}

char* HMC5883L::GetErrorText(int errorCode)
{
	if(ErrorCode_1_Num == 1)
	return ErrorCode_1;
	
	return "Error not defined.";
}