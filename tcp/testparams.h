/*
 * testparams.h
 *
 *  Created on: 31 Aug 2020
 *      Author: leszek.zarzycki
 */

#ifndef TESTPARAMS_H_
#define TESTPARAMS_H_

enum TestParamField {
	NOPARAM,				//0
	idString,				//1 string[40]
	testpressure,			//2 int
	initdelay,				//3 int
    ventoffdelay,			//4 int
    filltime,				//5 int
    stabilisationtime,		//6 int
    isolationdelay,			//7 int
    measuretime,			//8 int
    evacuationtime,			//9 int
    offsetcomp,				//10 int
    alarmleakrate,			//11 int
    alarmdiffp,				//12 int
    spare1,					//13 int
    fixtureOPstart,			//14 int //0..FF 0d.p
    fixturedelaytime,		//15 int //display to 3d.p
    //fixturecontrol,		//FixtureControl
    checkinputs,			//16 int
    enablecheckinputs,		//17 bool
    fixtureOPend,			//18 int //0..FF 0d.p
    nexttestonpass,			//19 int	//0..F
    nexttestonfailoralarm,	//20 int	//0..F
    inhibitresults,			//21 bool
    spare2,					//22 int //Inter Test Delay display to 3d.p
    spare3,					//23 int //Isolate Pulse time at end of Fill if Test Pressure has changed
    spareflag1,				//24 bool //Use TestP for Leak Detection
    spareint10,				//25 int //Spare for future use
    spareint11,				//26 int //Spare for future use
    spareint12,				//27 int //Spare for future use
    spareint13,				//28 int //Spare for future use
    spareflag10,			//29 bool //Spare for future use
    spareflag11,			//30 bool //Spare for future use
    spareflag12,			//31 bool //Spare for future use
    spareflag13, 			//32 bool //Spare for future use
    //when existing systems are updated (hopefully).

    //FixtureControl
	enabled_1,				//33 bool
	fixtime_1,				//34 int
	feedback_1,				//35 bool
	enabled_2,				//36 bool
	fixtime_2,				//37 int
	feedback_2,				//38 bool
	enabled_3,				//39 bool
	fixtime_3,				//40 int
	feedback_3,				//41 bool
	passmark,				//42 bool
	passmarktime,			//43 int
	failack,				//44 bool
	fillToPres,				//45 bool
	fillToPresTimer,		//46 int
    spareint20,				//47 int
    spareint21,				//48 int
    spareint22,				//49 int
    spareint23,				//50 int
    spareflag20,			//51 bool
    spareflag21,			//52 bool
    spareflag22,			//53 bool
    spareflag23,			//54 bool
	sparestring20,			//55 string[20]
};


/* Set a test parameter field with the given value.
 * testidx - test configuration index
 * The value (string) is converted to the type for the field.
 */
void setTestParam(int testidx, TestParamField k, char *v);

/* tk is a composite field number: test index*256 + field number
 * e.g. Set passmark field for test config2 has tk = 2*256 + 42 = 554
 */
void setTestParam(int tk, char *v);

/* Return the value of the test parameter field for the test configuration with index testidx*/
char* getTestParam(int testidx, TestParamField k);

void getAllTestParams(int idx, char * buf);

#endif /* TESTPARAMS_H_ */
