/*
L3G4200D.h - Header file for the L3G4200D Gyroscope Arduino Library.
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
#ifndef L3G4200D_h
#define L3G4200D_h

#include "Arduino.h" // for byte data type

// register addresses

#define L3G4200D_WHO_AM_I      0x0F
//#define L3G4200D_ID           (0b11010100)
#define L3G4200D_ID           (0b11010011)

#define L3G4200D_SENSITIVITY_250DPS  (0.00875F)      // Roughly 22/256 for fixed point match
#define L3G4200D_SENSITIVITY_500DPS  (0.0175F)       // Roughly 45/256
#define L3G4200D_SENSITIVITY_2000DPS (0.070F)        // Roughly 18/256
#define L3G4200D_DPS_TO_RADS         (0.017453293F)  // degress/s to rad/s multiplier


#define L3G4200D_CTRL_REG1     0x20
#define L3G4200D_CTRL_REG2     0x21
#define L3G4200D_CTRL_REG3     0x22
#define L3G4200D_CTRL_REG4     0x23
#define L3G4200D_CTRL_REG5     0x24
#define L3G4200D_REFERENCE     0x25
#define L3G4200D_OUT_TEMP      0x26
#define L3G4200D_STATUS_REG    0x27

#define L3G4200D_OUT_X_L       0x28
#define L3G4200D_OUT_X_H       0x29
#define L3G4200D_OUT_Y_L       0x2A
#define L3G4200D_OUT_Y_H       0x2B
#define L3G4200D_OUT_Z_L       0x2C
#define L3G4200D_OUT_Z_H       0x2D

#define L3G4200D_FIFO_CTRL_REG 0x2E
#define L3G4200D_FIFO_SRC_REG  0x2F

#define L3G4200D_INT1_CFG      0x30
#define L3G4200D_INT1_SRC      0x31
#define L3G4200D_INT1_THS_XH   0x32
#define L3G4200D_INT1_THS_XL   0x33
#define L3G4200D_INT1_THS_YH   0x34
#define L3G4200D_INT1_THS_YL   0x35
#define L3G4200D_INT1_THS_ZH   0x36
#define L3G4200D_INT1_THS_ZL   0x37
#define L3G4200D_INT1_DURATION 0x38

class L3G4200D
{
	
		
	public:
	    typedef enum
		{
			RANGE_250DPS,
			RANGE_500DPS,
			RANGE_2000DPS
		} Range_t;
	
		typedef struct vector
		{
			float x, y, z;
		} vector;
		
		vector g; // gyro angular velocity readings

		
		bool setup(Range_t rng);
		void writeReg(byte reg, byte value);
		byte readReg(byte reg);
		
		void read(void);
		
		// vector functions
		static void vector_cross(const vector *a, const vector *b, vector *out);
		static float vector_dot(const vector *a,const vector *b);
		static void vector_normalize(vector *a);
		
	private:
		byte address;
		Range_t range;
};

#endif



