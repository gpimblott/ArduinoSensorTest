/*
L3G4200D.cpp - L3G4200D Gyroscope Arduino Library.
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
#include "L3G4200D.h"
#include <Wire.h>
#include <math.h>

// Defines ////////////////////////////////////////////////////////////////

// The Arduino two-wire interface uses a 7-bit number for the address, 
// and sets the last bit correctly based on reads and writes
#define GYR_ADDRESS (0xD2 >> 1)

// Public Methods //////////////////////////////////////////////////////////////

// Turns on the L3G4200D's gyro and places it in normal mode.
bool L3G4200D::setup(Range_t rng)
{
	address = GYR_ADDRESS;
	range = rng;
	
	Wire.begin();
  
	/* Make sure we have the correct chip ID since this checks
     for correct address and that the IC is properly connected */
	byte id = readReg(L3G4200D_WHO_AM_I);
	if ( id != L3G4200D_ID )
	{
		Serial.print("Wrong ID");
		return false;
	}
  
	
	/* Set CTRL_REG1 (0x20)
	 ====================================================================
	 BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     7-6  DR1/0     Output data rate                                   00
     5-4  BW1/0     Bandwidth selection                                00
     3  PD        0 = Power-down mode, 1 = normal/sleep mode          0
     2  ZEN       Z-axis enable (0 = disabled, 1 = enabled)           1
     1  YEN       Y-axis enable (0 = disabled, 1 = enabled)           1
     0  XEN       X-axis enable (0 = disabled, 1 = enabled)           1 */

	/* Switch to normal mode and enable all three channels */
	writeReg(L3G4200D_CTRL_REG1, 0x0F);
	
	/* ------------------------------------------------------------------ */

	/* Set CTRL_REG2 (0x21)
    ====================================================================
    BIT  Symbol    Description                                   Default
    ---  ------    --------------------------------------------- -------
    5-4  HPM1/0    High-pass filter mode selection                    00
    3-0  HPCF3..0  High-pass filter cutoff frequency selection      0000 */

	/* Nothing to do ... keep default values */
	/* ------------------------------------------------------------------ */

	/* Set CTRL_REG3 (0x22)
    ====================================================================
	BIT  Symbol    Description                                   Default
	---  ------    --------------------------------------------- -------
     7  I1_Int1   Interrupt enable on INT1 (0=disable,1=enable)       0
     6  I1_Boot   Boot status on INT1 (0=disable,1=enable)            0
     5  H-Lactive Interrupt active config on INT1 (0=high,1=low)      0
     4  PP_OD     Push-Pull/Open-Drain (0=PP, 1=OD)                   0
     3  I2_DRDY   Data ready on DRDY/INT2 (0=disable,1=enable)        0
     2  I2_WTM    FIFO wtrmrk int on DRDY/INT2 (0=dsbl,1=enbl)        0
     1  I2_ORun   FIFO overrun int on DRDY/INT2 (0=dsbl,1=enbl)       0
     0  I2_Empty  FIFI empty int on DRDY/INT2 (0=dsbl,1=enbl)         0 */

	/* Nothing to do ... keep default values */
	/* ------------------------------------------------------------------ */

	/* Set CTRL_REG4 (0x23)
	====================================================================
	BIT  Symbol    Description                                   Default
    ---  ------    --------------------------------------------- -------
    7  BDU       Block Data Update (0=continuous, 1=LSB/MSB)         0
    6  BLE       Big/Little-Endian (0=Data LSB, 1=Data MSB)          0
    5-4  FS1/0     Full scale selection                               00
                                  00 = 250 dps
                                  01 = 500 dps
                                  10 = 2000 dps
                                  11 = 2000 dps
    0  SIM       SPI Mode (0=4-wire, 1=3-wire)                       0 */

	/* Adjust resolution if requested */
	switch(range)
	{	
    case RANGE_250DPS:
		writeReg(L3G4200D_CTRL_REG4, 0x00);
		break;
    case RANGE_500DPS:
		writeReg(L3G4200D_CTRL_REG4, 0x10);
		break;
    case RANGE_2000DPS:
		writeReg(L3G4200D_CTRL_REG4, 0x20);
		break;
  }
  /* ------------------------------------------------------------------ */

  /* Set CTRL_REG5 (0x24)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
     7  BOOT      Reboot memory content (0=normal, 1=reboot)          0
     6  FIFO_EN   FIFO enable (0=FIFO disable, 1=enable)              0
     4  HPen      High-pass filter enable (0=disable,1=enable)        0
   3-2  INT1_SEL  INT1 Selection config                              00
   1-0  OUT_SEL   Out selection config                               00 */

  /* Nothing to do ... keep default values */
  /* ------------------------------------------------------------------ */

	return true;
}



// Reads the 3 gyro channels and stores them in vector g
void L3G4200D::read()
{
	Wire.beginTransmission(GYR_ADDRESS);
	// assert the MSB of the address to get the gyro 
	// to do slave-transmit subaddress updating.
	Wire.write(L3G4200D_OUT_X_L | (1 << 7)); 
	Wire.endTransmission();
	Wire.requestFrom(GYR_ADDRESS, 6);

  // Wait for data to be available
	while (Wire.available() < 6);
	
	uint8_t xla = Wire.read();
	uint8_t xha = Wire.read();
	uint8_t yla = Wire.read();
	uint8_t yha = Wire.read();
	uint8_t zla = Wire.read();
	uint8_t zha = Wire.read();

	g.x = (xha << 8) | xla;
	g.y = (yha << 8) | yla;
	g.z = (zha << 8) | zla;
	
	// Compensate values depending on the resolution
	switch(range)
	{
    case RANGE_250DPS:
      g.x *= L3G4200D_SENSITIVITY_250DPS;
      g.y *= L3G4200D_SENSITIVITY_250DPS;
      g.z *= L3G4200D_SENSITIVITY_250DPS;
      break;
    case RANGE_500DPS:
      g.x *= L3G4200D_SENSITIVITY_500DPS;
      g.y *= L3G4200D_SENSITIVITY_500DPS;
      g.z *= L3G4200D_SENSITIVITY_500DPS;
      break;
    case RANGE_2000DPS:
      g.x *= L3G4200D_SENSITIVITY_2000DPS;
      g.y *= L3G4200D_SENSITIVITY_2000DPS;
      g.z *= L3G4200D_SENSITIVITY_2000DPS;
      break;
	}	
}

void L3G4200D::vector_cross(const vector *a,const vector *b, vector *out)
{
  out->x = a->y*b->z - a->z*b->y;
  out->y = a->z*b->x - a->x*b->z;
  out->z = a->x*b->y - a->y*b->x;
}

float L3G4200D::vector_dot(const vector *a,const vector *b)
{
  return a->x*b->x+a->y*b->y+a->z*b->z;
}

void L3G4200D::vector_normalize(vector *a)
{
  float mag = sqrt(vector_dot(a,a));
  a->x /= mag;
  a->y /= mag;
  a->z /= mag;
}

/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

// Writes a gyro register
void L3G4200D::writeReg(byte reg, byte value)
{
	Wire.beginTransmission(GYR_ADDRESS);
	Wire.write(reg);
	Wire.write(value);
	Wire.endTransmission();
}

// Reads a gyro register
byte L3G4200D::readReg(byte reg)
{
	byte value;
	
	Wire.beginTransmission(GYR_ADDRESS);
	Wire.write(reg);
	Wire.endTransmission();
	Wire.requestFrom(GYR_ADDRESS, 1);
	value = Wire.read();
	Wire.endTransmission();
	
	return value;
}