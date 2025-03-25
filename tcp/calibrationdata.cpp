/*
 * calibrationdata.cpp
 *
 *  Created on: 10 Sep 2020
 *      Author: leszek.zarzycki
 */


#include <stdlib.h>
#include <string.h> //strcpy, strlen, strcmp
#include <utils.h> //AcsiiToIP, snShowIP
#include <stdio.h>
#include "MLT_globalTypes.h"
#include "calibrationdata.h"

extern NV_Params_Struct NV_Params;

/* Set a calibration field with the given value.
 * channel - AD channel 0..3
 * The value (string) is converted to the type for the field.
 */
void setCalibField(int channel, CalibrationField k, char *v) {
	iprintf("Calibration[%d].%d=%s\r\n",channel, k, v);
	switch (k) {
	case NONE:	 	break;	//0
	case valueMax:	NV_Params.CalDataNV[channel].valueMax = atoi(v); break;	//1 int
	case countMax:	NV_Params.CalDataNV[channel].countMax = atoi(v); break;	//2 int
	case valueMin:	NV_Params.CalDataNV[channel].valueMin = atoi(v); break;	//3 int
	case countMin:	NV_Params.CalDataNV[channel].countMin = atoi(v); break;	//4 int
	case gradient:	NV_Params.CalDataNV[channel].gradient = atof(v); break;	//5 float
	case offset:	NV_Params.CalDataNV[channel].offset = atof(v); break;	//6 float
	default: ;
	}
}

/* tk is a composite field number: channel*256 + field number
 * e.g. Set offset field for channel 2, tk = 2*256 + 6 = 518
 */
void setCalibration(int tk, char *v) {
	setCalibField(tk/256, (CalibrationField) (tk%256), v);
}

char bufc[256] = "\0";

/* Return the value of the calibration field for the given channel*/
char* getCalibrationValue(int channel, CalibrationField k) {
	bufc[0] = '\0';

	switch (k) {
	case NONE:	 	break;	//0
	case valueMax:	sprintf(bufc, "%d", NV_Params.CalDataNV[channel].valueMax); break;	//1 int
	case countMax:	sprintf(bufc, "%d", NV_Params.CalDataNV[channel].countMax); break;	//2 int
	case valueMin:	sprintf(bufc, "%d", NV_Params.CalDataNV[channel].valueMin); break;	//3 int
	case countMin:	sprintf(bufc, "%d", NV_Params.CalDataNV[channel].countMin); break;	//4 int
	case gradient:	sprintf(bufc, "%f", NV_Params.CalDataNV[channel].gradient); break;	//5 float
	case offset:	sprintf(bufc, "%f", NV_Params.CalDataNV[channel].offset); break;	//6 float
	default: ; //unknown parameter
	}

	return bufc;
}

/* Return calibration all (field:value) pairs for the given channel*/
void getAllCalibrationValues(int channel, char * buf) {
	buf[0] = '\0';
	sprintf(buf,"C%d{", channel);

	char temp[32];
	for (int i=1; i<=6; i++) {
		sprintf(temp, "%d:%s,", i, getCalibrationValue(channel, (CalibrationField) i) );
		strcat(buf, temp);
	}
	buf[strlen(buf)-1]='}';
	//iprintf("returning buf = --%s--\r\n", buf);
}



