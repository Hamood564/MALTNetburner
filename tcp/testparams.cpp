/*
 * testparams.cpp
 *
 *  Created on: 1 Sep 2020
 *      Author: leszek.zarzycki
 */

#include <stdlib.h>
#include <string.h> //strcpy, strlen, strcmp
#include <utils.h> //AcsiiToIP, snShowIP
#include <stdio.h>
#include "MLT_globalTypes.h"
#include "testparams.h"

extern NV_Params_Struct NV_Params;

/* Set a test parameter field with the given value.
 * idx - test configuration index
 * The value (string) is converted to the type for the field.
 */
void setTestParam(int idx, TestParamField k, char *v) {
	switch (k) {
	case NOPARAM: break;
	case idString:
		strcpy(NV_Params.TestSetupNV[idx].idString, v);
		break;
	case testpressure: 			NV_Params.TestSetupNV[idx].testpressure=atoi(v); break;
	case initdelay: 			NV_Params.TestSetupNV[idx].initdelay = atoi(v); break;
	case ventoffdelay: 			NV_Params.TestSetupNV[idx].ventoffdelay = atoi(v); break;
	case filltime: 				NV_Params.TestSetupNV[idx].filltime = atoi(v); break;
	case stabilisationtime: 	NV_Params.TestSetupNV[idx].stabilisationtime = atoi(v); break;
	case isolationdelay: 		NV_Params.TestSetupNV[idx].isolationdelay = atoi(v); break;
	case measuretime: 			NV_Params.TestSetupNV[idx].measuretime = atoi(v); break;
	case evacuationtime: 		NV_Params.TestSetupNV[idx].evacuationtime = atoi(v); break;
	case offsetcomp:			NV_Params.TestSetupNV[idx].offsetcomp = atoi(v); break;
	case alarmleakrate: 		NV_Params.TestSetupNV[idx].alarmleakrate = atoi(v); break;
	case alarmdiffp: 			NV_Params.TestSetupNV[idx].alarmdiffp = atoi(v); break;
	case spare1: 				NV_Params.TestSetupNV[idx].spare1 = atoi(v); break;
	case fixtureOPstart: 		NV_Params.TestSetupNV[idx].fixtureOPstart = atoi(v); break;
	case fixturedelaytime: 		NV_Params.TestSetupNV[idx].fixturedelaytime = atoi(v); break;
	//case fixturecontrol: break;
	case checkinputs:			NV_Params.TestSetupNV[idx].checkinputs = atoi(v); break;
	case enablecheckinputs: 	NV_Params.TestSetupNV[idx].enablecheckinputs = (atoi(v)!=0); break;
	case fixtureOPend: 			NV_Params.TestSetupNV[idx].fixtureOPend = atoi(v); break;
	case nexttestonpass: 		NV_Params.TestSetupNV[idx].nexttestonpass = atoi(v); break;
	case nexttestonfailoralarm: NV_Params.TestSetupNV[idx].nexttestonfailoralarm = atoi(v); break;
	case inhibitresults: 		NV_Params.TestSetupNV[idx].inhibitresults = (atoi(v)!=0); break;
	case spare2: 				NV_Params.TestSetupNV[idx].spare2 = atoi(v); break;
	case spare3: 				NV_Params.TestSetupNV[idx].spare3 = atoi(v); break;
	case spareflag1: 			NV_Params.TestSetupNV[idx].spareflag1 = (atoi(v)!=0); break;
	case spareint10: 			NV_Params.TestSetupNV[idx].spareint10 = atoi(v); break;
	case spareint11: 			NV_Params.TestSetupNV[idx].spareint11 = atoi(v); break;
	case spareint12: 			NV_Params.TestSetupNV[idx].spareint12 = atoi(v); break;
	case spareint13: 			NV_Params.TestSetupNV[idx].spareint13 = atoi(v); break;
	case spareflag10: 			NV_Params.TestSetupNV[idx].spareflag10 = (atoi(v)!=0); break;
	case spareflag11: 			NV_Params.TestSetupNV[idx].spareflag11 = (atoi(v)!=0); break;
	case spareflag12: 			NV_Params.TestSetupNV[idx].spareflag12 = (atoi(v)!=0); break;
	case spareflag13: 			NV_Params.TestSetupNV[idx].spareflag13 = (atoi(v)!=0); break;
	//fixture control
	case enabled_1: 			NV_Params.TestSetupNV[idx].fixturecontrol.enabled[0] = atoi(v)!=0; break;
	case fixtime_1: 			NV_Params.TestSetupNV[idx].fixturecontrol.fixtime[0] = atoi(v); break;
	case feedback_1: 			NV_Params.TestSetupNV[idx].fixturecontrol.feedback[0] = atoi(v)!=0; break;

	case enabled_2: 			NV_Params.TestSetupNV[idx].fixturecontrol.enabled[1] = atoi(v)!=0; break;
	case fixtime_2: 			NV_Params.TestSetupNV[idx].fixturecontrol.fixtime[1] = atoi(v); break;
	case feedback_2: 			NV_Params.TestSetupNV[idx].fixturecontrol.feedback[1] = atoi(v)!=0; break;

	case enabled_3: 			NV_Params.TestSetupNV[idx].fixturecontrol.enabled[2] = atoi(v)!=0; break;
	case fixtime_3:				NV_Params.TestSetupNV[idx].fixturecontrol.fixtime[2] = atoi(v); break;
	case feedback_3: 			NV_Params.TestSetupNV[idx].fixturecontrol.feedback[2] = atoi(v)!=0; break;

	case passmark: 				NV_Params.TestSetupNV[idx].fixturecontrol.passmark = atoi(v); break;
	case passmarktime: 			NV_Params.TestSetupNV[idx].fixturecontrol.passmarktime = atoi(v); break;
	case failack: 				NV_Params.TestSetupNV[idx].fixturecontrol.failack = atoi(v)!=0; break;
	case fillToPres: 			NV_Params.TestSetupNV[idx].fixturecontrol.fillToPres = atoi(v)!=0; break;
	case fillToPresTimer: 		NV_Params.TestSetupNV[idx].fixturecontrol.fillToPresTimer = atoi(v); break;
	case spareint20: 			NV_Params.TestSetupNV[idx].fixturecontrol.spareint20 = atoi(v); break;
	case spareint21: 			NV_Params.TestSetupNV[idx].fixturecontrol.spareint20 = atoi(v); break;
	case spareint22: 			NV_Params.TestSetupNV[idx].fixturecontrol.spareint20 = atoi(v); break;
	case spareint23: 			NV_Params.TestSetupNV[idx].fixturecontrol.spareint20 = atoi(v); break;
	case spareflag20: 			NV_Params.TestSetupNV[idx].fixturecontrol.spareflag20 = (atoi(v)!=0); break;
	case spareflag21: 			NV_Params.TestSetupNV[idx].fixturecontrol.spareflag21 = (atoi(v)!=0); break;
	case spareflag22: 			NV_Params.TestSetupNV[idx].fixturecontrol.spareflag22 = (atoi(v)!=0); break;
	case spareflag23: 			NV_Params.TestSetupNV[idx].fixturecontrol.spareflag23 = (atoi(v)!=0); break;
	case sparestring20: 		strcpy(NV_Params.TestSetupNV[idx].fixturecontrol.sparestring20, v); break;
	default: ; //unknown parameter
	}
}

/* tk is a composite field number: test index*256 + field number
 * e.g. Set passmark field for test config2 has tk = 2*256 + 42 = 554
 */
void setTestParam(int tk, char *v) {
	setTestParam(tk/256, (TestParamField) (tk%256), v);
}


char bufp[1024] = "\0";  // (LZ) make this a var parameter of function instead???

/* Return the value of the test parameter field for the test configuration with index testidx*/
char* getTestParam(int idx, TestParamField k) {
	bufp[0]= '\0';
	switch (k) {
	case NOPARAM: break;
	case idString:
		strcpy(bufp, NV_Params.TestSetupNV[idx].idString);
		break;
	case testpressure: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].testpressure); break;
	case initdelay: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].initdelay); break;
	case ventoffdelay: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].ventoffdelay); break;
	case filltime: 				sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].filltime); break;
	case stabilisationtime: 	sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].stabilisationtime); break;
	case isolationdelay: 		sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].isolationdelay); break;
	case measuretime: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].measuretime); break;
	case evacuationtime: 		sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].evacuationtime); break;
	case offsetcomp:			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].offsetcomp); break;
	case alarmleakrate: 		sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].alarmleakrate); break;
	case alarmdiffp: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].alarmdiffp); break;
	case spare1: 				sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spare1); break;
	case fixtureOPstart: 		sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixtureOPstart); break;
	case fixturedelaytime: 		sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturedelaytime); break;
	//case fixturecontrol: break;
	case checkinputs:			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].checkinputs); break;
	case enablecheckinputs: 	sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].enablecheckinputs); break;
	case fixtureOPend: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixtureOPend); break;
	case nexttestonpass: 		sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].nexttestonpass); break;
	case nexttestonfailoralarm: sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].nexttestonfailoralarm); break;
	case inhibitresults: 		sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].inhibitresults); break;
	case spare2: 				sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spare2); break;
	case spare3: 				sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spare3); break;
	case spareflag1: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spareflag1); break;
	case spareint10: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spareint10); break;
	case spareint11: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spareint11); break;
	case spareint12: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spareint12); break;
	case spareint13: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spareint13); break;
	case spareflag10: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spareflag10); break;
	case spareflag11: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spareflag11); break;
	case spareflag12: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spareflag12); break;
	case spareflag13: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].spareflag13); break;
	//default: ; //unknown parameter

	//fixture control
	//case NOFIXTURE: 			break;
	case enabled_1: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.enabled[0]); break;
	case fixtime_1: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.fixtime[0]); break;
	case feedback_1: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.feedback[0]); break;

	case enabled_2: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.enabled[1]); break;
	case fixtime_2: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.fixtime[1]); break;
	case feedback_2: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.feedback[1]); break;

	case enabled_3: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.enabled[2]); break;
	case fixtime_3:				sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.fixtime[2]); break;
	case feedback_3: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.feedback[2]); break;

	case passmark: 				sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.passmark); break;
	case passmarktime: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.passmarktime); break;
	case failack: 				sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.failack); break;
	case fillToPres: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.fillToPres); break;
	case fillToPresTimer: 		sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.fillToPresTimer); break;
	case spareint20: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.spareint20); break;
	case spareint21: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.spareint20); break;
	case spareint22: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.spareint20); break;
	case spareint23: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.spareint20); break;
	case spareflag20: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.spareflag20); break;
	case spareflag21: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.spareflag21); break;
	case spareflag22: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.spareflag22); break;
	case spareflag23: 			sprintf(bufp, "%d", NV_Params.TestSetupNV[idx].fixturecontrol.spareflag23); break;
	case sparestring20: 		strcpy(bufp, NV_Params.TestSetupNV[idx].fixturecontrol.sparestring20); break;
	default: ; //unknown parameter
	}
	return bufp;
}

void getAllTestParams(int idx, char * buf) {
	buf[0] = '\0';
	sprintf(buf,"P%d{", idx);

	char temp[32];
	for (int i=1; i<=55; i++) {
		sprintf(temp, "%d:%s,", i, getTestParam(idx, (TestParamField) i) );
		strcat(buf, temp);
	}
	buf[strlen(buf)-1]='}';
	//iprintf("returning buf = --%s--\r\n", buf);
}
