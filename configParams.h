/**
 * configParams.h
 * MALT Web interface to TestParams variables NV_Params.TestSetupNV[]
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT1.1
 * @date 2015-01-13
 */

#ifndef CONFIGPARAMS_H_
#define CONFIGPARAMS_H_

#include <stdio.h>

#include "MLT_globalExterns.h"

/*
 * class for configuration parameters for a leak test.
 * Essentially a struct with standard modifiers and accessors,
 * and convenience functions: setAllParams(source), and printConfigParams().
 */

class TestConfigParams {
private:

	TestParams params;

public:
	TestConfigParams();
	virtual ~TestConfigParams();

	TestParams getTestParams() const;

	/* Convenience method to copy all the parameters from p to this instance. */
	void setTestParams(TestParams p);

	void resetAllParams();

	void printConfigParameters() const;


	//========setters and getters for each field of TestParams=================

	//to re-instate if config struct includes an ID field. - done on 2014.11.18
	const char* getIdString() const;
	void setIdString(char* idString);

	int getAlarmdiffp() const;
	void setAlarmdiffp(int alarmdiffp);

	int getAlarmleakrate() const;
	void setAlarmleakrate(int alarmleakrate);

	int getEvacuationtime() const;
	void setEvacuationtime(int evacuationtime);

	int getFilltime() const;
	void setFilltime(int filltime);

	int getIsolationdelay() const;
	void setIsolationdelay(int isolationdelay);

	int getInitdelay() const;
	void setInitdelay(int initdelay);

	int getMeasuretime() const;
	void setMeasuretime(int measuretime);

	int getOffsetcomp() const;
	void setOffsetcomp(int offsetcomp);

	int getStabilisationtime() const;
	void setStabilisationtime(int stabilisationtime);

	int getTestpressure() const;
	void setTestpressure(int testpressure);

	int getVentoffdelay() const;
	void setVentoffdelay(int ventoffdelay);
	
	int getSpare1() const;
	void setSpare1(int spare1A);

	int getCheckinputs() const;
	void setCheckinputs(int checkinputs);

	bool isEnablecheckinputs() const;
	void setEnablecheckinputs(bool enablecheckinputs);

	int getFixturedelaytime() const;
	void setFixturedelaytime(int fixturedelaytime);

	int getFixtureOPend() const;
	void setFixtureOPend(int fixtureOPend);

	int getFixtureOPstart() const;
	void setFixtureOPstart(int fixtureOPstart);

	bool isInhibitresults() const;
	void setInhibitresults(bool inhibitresults);

	int getNexttestonfailoralarm() const;
	void setNexttestonfailoralarm(int nexttestonfailoralarm);

	int getNexttestonpass() const;
	void setNexttestonpass(int nexttestonpass);

	int getSpare2() const;
	void setSpare2(int spare2);

	int getSpare3() const;
	void setSpare3(int spare3);

	bool isSpareflag1() const;
	void setSpareflag1(bool spareflag1);

	//2016-06-23
	FixtureControl getFixtureControl();
	void setFixtureControl(FixtureControl fixctrl);
}
;

#endif /* CONFIGPARAMS_H_ */
