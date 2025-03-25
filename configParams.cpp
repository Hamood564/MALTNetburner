/**
 * configParams.cpp
 * MALT Web interface to TestParams variables NV_Params.TestSetupNV[]
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT51.1
 * @date 2015-01-13
 */

#include <string.h>
#include "configParams.h"

TestConfigParams::TestConfigParams() {
	this->resetAllParams();
}

TestConfigParams::~TestConfigParams() {
	// TODO Auto-generated destructor stub
}


TestParams TestConfigParams::getTestParams() const {
	return params;
}

void TestConfigParams::setTestParams(TestParams p) {
	params = p;
}

void TestConfigParams::resetAllParams() {
	strcpy(params.idString,"NO_ID");
	params.testpressure=5000;
	params.initdelay=100;
    params.ventoffdelay=200;
    params.filltime=2000;
    params.stabilisationtime=5000;
    params.isolationdelay=100;
    params.measuretime=2000;
    params.evacuationtime=1000;
    params.offsetcomp=0;
	params.alarmleakrate=0,
	params.alarmdiffp=1000,
    params.spare1=0;
    params.fixtureOPstart=0; //0..FF 0d.p
    params.fixturedelaytime=0; //display to 3d.p
    params.checkinputs=0;
    params.enablecheckinputs=false;
    params.fixtureOPend=0; //0..FF 0d.p
    params.nexttestonpass=0;	//0..F
    params.nexttestonfailoralarm=0;	//0..F
    params.inhibitresults=true;
    params.spare2=0;
    params.spare3=0;
    params.spareflag1=false;

    //2016-06-23 fixture control
    for(int i=0; i<3; i++) {
    	params.fixturecontrol.enabled[i] = false;
    	params.fixturecontrol.fixtime[i] = 0;
    	params.fixturecontrol.feedback[i] = false;
    }
    params.fixturecontrol.passmark = false;
    params.fixturecontrol.passmarktime = 0;
    params.fixturecontrol.failack = false;
}



void TestConfigParams::printConfigParameters() const {
	iprintf("Test Configuration Parameters:[%s, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
										   "%d, %d, %d, %c, %d, %d, %d, %c, %d, %d, %c]\r\n",

			params.idString,
			params.testpressure,
			params.initdelay,
			params.ventoffdelay,
			params.filltime,
			params.stabilisationtime,
			params.isolationdelay,
			params.measuretime,
			params.evacuationtime,
			params.offsetcomp,
			params.alarmleakrate,
			params.alarmdiffp,
		    params.spare1,
		    params.fixtureOPstart, //0..FF 0d.p
		    params.fixturedelaytime, //display to 3d.p
		    params.checkinputs,
		    params.enablecheckinputs?'T':'F',
		    params.fixtureOPend, //0..FF 0d.p
		    params.nexttestonpass,	//0..F
		    params.nexttestonfailoralarm,	//0..F
		    params.inhibitresults?'T':'F',
		    params.spare2,
		    params.spare3,
		    params.spareflag1?'T':'F'
			);

	 //2016-06-23 fixture control
	iprintf("\tFixture Control: [");
    for(int i=0; i<3; i++) {
    	iprintf("(%c, %d, %c), ",
   	    	params.fixturecontrol.enabled[i]?'T':'F',
   	    	params.fixturecontrol.fixtime[i],
   	    	params.fixturecontrol.feedback[i]?'T':'F'
   	    );
    }
    iprintf("%c, %d, %c]\r\n",
    	    params.fixturecontrol.passmark?'T':'F',
    	    params.fixturecontrol.passmarktime,
    	    params.fixturecontrol.failack?'T':'F'
    );
}


// to re-instate if config struct includes an ID field.
const char* TestConfigParams::getIdString() const {
	return params.idString;
}

void TestConfigParams::setIdString(char* idString) {
	strcpy(params.idString, idString);
}

int TestConfigParams::getAlarmdiffp() const {
	return params.alarmdiffp;
}

void TestConfigParams::setAlarmdiffp(int alarmdiffp) {
	params.alarmdiffp = alarmdiffp;
}

int TestConfigParams::getAlarmleakrate() const {
	return params.alarmleakrate;
}

void TestConfigParams::setAlarmleakrate(int alarmleakrate) {
	params.alarmleakrate = alarmleakrate;
}

int TestConfigParams::getEvacuationtime() const {
	return params.evacuationtime;
}

void TestConfigParams::setEvacuationtime(int evacuationtime) {
	params.evacuationtime = evacuationtime;
}

int TestConfigParams::getFilltime() const {
	return params.filltime;
}

void TestConfigParams::setFilltime(int filltime) {
	params.filltime = filltime;
}

int TestConfigParams::getIsolationdelay() const {
	return params.isolationdelay;
}

void TestConfigParams::setIsolationdelay(int isolationdelay) {
	params.isolationdelay = isolationdelay;
}

int TestConfigParams::getInitdelay() const {
	return params.initdelay;
}

void TestConfigParams::setInitdelay(int initdelay) {
	params.initdelay = initdelay;
}

int TestConfigParams::getMeasuretime() const {
	return params.measuretime;
}

void TestConfigParams::setMeasuretime(int measuretime) {
	params.measuretime = measuretime;
}

int TestConfigParams::getOffsetcomp() const {
	return params.offsetcomp;
}

void TestConfigParams::setOffsetcomp(int offsetcomp) {
	params.offsetcomp = offsetcomp;
}

int TestConfigParams::getStabilisationtime() const {
	return params.stabilisationtime;
}

void TestConfigParams::setStabilisationtime(int stabilisationtime) {
	params.stabilisationtime = stabilisationtime;
}

int TestConfigParams::getTestpressure() const {
	return params.testpressure;
}

void TestConfigParams::setTestpressure(int testpressure) {
	params.testpressure = testpressure;
}

int TestConfigParams::getVentoffdelay() const {
	return params.ventoffdelay;
}

void TestConfigParams::setVentoffdelay(int ventoffdelay) {
	params.ventoffdelay = ventoffdelay;
}

int TestConfigParams::getSpare1() const {
	return params.spare1;
}

void TestConfigParams::setSpare1(int spare1A) {
	params.spare1 = spare1A;
}

int TestConfigParams::getCheckinputs() const {
	return params.checkinputs;
}

void TestConfigParams::setCheckinputs(int checkinputs) {
	params.checkinputs = checkinputs;
}

bool TestConfigParams::isEnablecheckinputs() const {
	return params.enablecheckinputs;
}

void TestConfigParams::setEnablecheckinputs(bool enablecheckinputs) {
	params.enablecheckinputs = enablecheckinputs;
}

int TestConfigParams::getFixturedelaytime() const {
	return params.fixturedelaytime;
}

void TestConfigParams::setFixturedelaytime(int fixturedelaytime) {
	params.fixturedelaytime = fixturedelaytime;
}

int TestConfigParams::getFixtureOPend() const {
	return params.fixtureOPend;
}

void TestConfigParams::setFixtureOPend(int fixtureOPend) {
	params.fixtureOPend = fixtureOPend;
}

int TestConfigParams::getFixtureOPstart() const {
	return params.fixtureOPstart;
}

void TestConfigParams::setFixtureOPstart(int fixtureOPstart) {
	params.fixtureOPstart = fixtureOPstart;
}

bool TestConfigParams::isInhibitresults() const {
	return params.inhibitresults;
}

void TestConfigParams::setInhibitresults(bool inhibitresults) {
	params.inhibitresults = inhibitresults;
}

int TestConfigParams::getNexttestonfailoralarm() const {
	return params.nexttestonfailoralarm;
}

void TestConfigParams::setNexttestonfailoralarm(int nexttestonfailoralarm) {
	params.nexttestonfailoralarm = nexttestonfailoralarm;
}

int TestConfigParams::getNexttestonpass() const {
	return params.nexttestonpass;
}

void TestConfigParams::setNexttestonpass(int nexttestonpass) {
	params.nexttestonpass = nexttestonpass;
}

int TestConfigParams::getSpare2() const {
	return params.spare2;
}

void TestConfigParams::setSpare2(int spare2) {
	params.spare2 = spare2;
}

int TestConfigParams::getSpare3() const {
	return params.spare3;
}

void TestConfigParams::setSpare3(int spare3) {
	params.spare3 = spare3;
}

bool TestConfigParams::isSpareflag1() const {
	return params.spareflag1;
}

void TestConfigParams::setSpareflag1(bool spareflag1) {
	params.spareflag1 = spareflag1;
}

FixtureControl TestConfigParams::getFixtureControl() {
	return params.fixturecontrol;
}

void TestConfigParams::setFixtureControl(FixtureControl fixctrl){
	params.fixturecontrol = fixctrl;
}


