/*
 * SensorUtils.h
 *
 * Created: 01/11/2013 21:53:57
 *  Author: gordon
 */ 


#ifndef SENSORUTILS_H_
#define SENSORUTILS_H_

// I2C functions
#include "Arduino.h"
#include <Wire.h>


void writeI2C(int id , int address, int data);
void writeI2C(int address, int data);
byte readI2C(int address );
byte readWhoI2C(int deviceAddress);

#endif /* SENSORUTILS_H_ */