/*
 * calibrationdata.h
 *
 *  Created on: 10 Sep 2020
 *      Author: leszek.zarzycki
 */

#ifndef CALIBRATIONDATA_H_
#define CALIBRATIONDATA_H_

#define DIFFPCHANNEL 	0
#define CHANNEL1 		1
#define TESTPCHANNEL 	2
#define CHANNEL3 		3

enum CalibrationField {
	NONE,		//0
	valueMax,	//1 int
	countMax,	//2 int
	valueMin,	//3 int
	countMin,	//4 int
	gradient,	//5 float
	offset		//6 float
};

/* Set a calibration field with the given value.
 * channel - AD channel 0..3
 * The value (string) is converted to the type for the field.
 */
void setCalibField(int channel, CalibrationField k, char *v);

/* tk is a composite field number: channel*1024 + field number
 * e.g. Set passmark field for test config2 has tk = 2*1024 + 400 = 2448
 */
void setCalibration(int tk, char *v);

/* Return the value of the calibration field for the given channel*/
char* getCalibrationValue(int channel, CalibrationField k);

/* Return calibration all (field:value) pairs for the given channel*/
void getAllCalibrationValues(int channel, char * buf);

#endif /* CALIBRATIONDATA_H_ */
