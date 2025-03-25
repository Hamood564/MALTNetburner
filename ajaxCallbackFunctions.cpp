/**
 * ajaxCallbackFunctions.cpp
 * MALT web interface -
 * 		Call back functions used by web pages.
 * 		Note: Called back functions with parameters must be declared in htmlvar.h,
 * 		with format:	const char * function(int sock, int v, ...)
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT5.1
 * @date 2015-01-13
 */

#include <predef.h>
#include <stdio.h>
#include <startnet.h>
#include <init.h>
#include <assert.h>
#include <utils.h>

#include "datalog.h" //declaration of RingLog type
#include "configParams.h"
#include "CalibrationHtmlData.h"
#include "MLT_globalExterns.h"
#include "MLT_globalLiterals.h"

extern TestConfigParams configParams;
extern CalibrationHtmlData calibParams[4];
extern RingLog datalog[2];

/**==========================================================================
 * AJAX callback functions
 * @param sock file descriptor of the TCP socket for the HTTP stream
 * @param url Pointer to a string containing the URL requested
 */

/*=======================================================================================*/
/* Callbacks for Data section of datatable.html. */

const char * getHtmlTimestamp(int sock,int v) {
	char buf[20];
	RunningValues *val = datalog[v].getCurrent();
	sprintf(buf,"%d",val->testpCount);
	writestring( sock, buf );
	return "\0"; //must return something
}

const char * getHtmlTPCount(int sock,int v) {
	char buf[20];
	RunningValues *val = datalog[v].getCurrent();
	sprintf(buf,"%5d",val->testpCount);
	writestring( sock, buf );
	return "\0"; //must return something
}

const char * getHtmlTP(int sock,int v) {
	char buf[20];
	RunningValues *val = datalog[v].getCurrent();
	sprintf(buf,"%1.0f",(val->testp)/10.0); //lz dec2019--- was %8.0f
	writestring( sock, buf );
	return "\0"; //must return something
}

const char * getHtmlDPCount(int sock,int v) {
	char buf[20];
	RunningValues *val = datalog[v].getCurrent();
	sprintf(buf,"%5d",val->diffpCount);
	writestring( sock, buf );
	return "\0"; //must return something
}

const char * getHtmlDP(int sock,int v) {
	char buf[20];
	RunningValues *val = datalog[v].getCurrent();
	sprintf(buf,"%8.1f",( val->diffp)/10.0);
	writestring( sock, buf );
	return "\0"; //must return something
}

const char * getHtmlPassFail(int sock, int module, int circle) {
	char buf[20];
	switch (circle) {
		case 0:	//pass circle
			if (module == 1)
//				sprintf(buf,"\"circle %s\"", (PASSBITB?"pass":"neutral"));
				;
			else
				sprintf(buf,"\"circle %s\"", (PASSBIT?"pass":"neutral"));
			break;
		case 1:	//fail circle
			if (module == 1)
//				sprintf(buf,"\"circle %s\"", (FAILBITB?"fail":"neutral"));
				;
			else
				sprintf(buf,"\"circle %s\"", (FAILBIT?"fail":"neutral"));
			break;
		case 2:	//error circle
			if (module == 1)
//				sprintf(buf,"\"circle %s\"", (FAULTBITB?"neutral":"error"));
				;
			else
				sprintf(buf,"\"circle %s\"", (FAULTBIT?"error":"neutral"));
			break;
		default: sprintf(buf,"\"circle %s\"","neutral");	//should never happen
	}
	writestring( sock, buf );
	return "\0"; //must return something
}


const char * getHtmlDone(int sock, int module) {
	char buf[20];
	if (module == 1)
//		sprintf(buf,"\"circle %s\"", (DONEBITB?"done":"neutral"));
		;
	else
		sprintf(buf,"\"circle %s\"", (DONEBIT?"done":"neutral"));
	writestring( sock, buf );
	return "\0"; //must return something
}

const char * getHtmlError(int sock, int module) {
	char buf[20];
	if (module == 1)
//		sprintf(buf,"\"circle %s\"", (FAULTBITB?"neutral":"error"));
		;
	else
		sprintf(buf,"\"circle %s\"", (FAULTBIT?"error":"neutral"));
	writestring( sock, buf );
	return "\0"; //must return something
}
/*=======================================================================================*/
/* Callbacks for results section of resultTable.html. */

// Test sequence text states - index with stepCountA-B
// Copy also in resultsComms.cpp
static char* resultText[20] = {
		"No Result", "Pass", "Fail", "Gross Fail", "Fail - Test Pres Low",  //Result 0 to 4
		"No Result", "No Result", "No Result", "Pass - No Display", "Fail - No Display",  //Result 5 to 9
		"Diff Pres High", "Test Reset", "Fail - Test Pres High", "Reference Error", "No Result",  //Result 10 to 14
		"Dig IPs NOk", "Fixture NOk", "No Result", "No Result", "No Result"  //Result 15 to 19
};

const char * getHtmlResStatus(int sock,int module) {
	char buf[30];
//	if (module == 1)
//		siprintf(buf, "%s", resultText[LTB_Results.status]);
//	else
		siprintf(buf, "%s", resultText[LT_Results.status]);
	writestring( sock, buf );
	return "\0"; //must return something
}

const char * getHtmlResTP(int sock,int v) {
	char buf[20];
	sprintf(buf,"%1.0f",datalog[v].getResTP()/10.0); //lz dec2019 -- was %8.0f
	writestring( sock, buf );
	return "\0"; //must return something
}

const char * getHtmlResDP(int sock,int v) {
	char buf[20];
	sprintf(buf,"%8.1f",datalog[v].getResDP()/10.0);
	writestring( sock, buf );
	return "\0"; //must return something
}

const char * getHtmlResLR(int sock,int v) {
	char buf[20];
	sprintf(buf,"%8.2f",datalog[v].getResLR()/100.0);
	writestring( sock, buf );
	return "\0"; //must return something
}

const char * getHtmlResAZ(int sock,int v) {
	char buf[20];
	sprintf(buf,"%8.1f",datalog[v].getResAZ()/10.0);
	writestring( sock, buf );
	return "\0"; //must return something
}

const char * getHtmlResTimeStamp(int sock,int v) {
	char buf[20];
	sprintf(buf,"%d",datalog[v].getResTimeStamp());
	writestring( sock, buf );
	return "\0"; //must return something
}

/* =======================================================================================
 * Call back function to populate the Test Parameters Configuration Table
 * Note: Called back functions with parameters must be declared in htmlvar.h, with format
 * 		const char * function(int sock, int v, ...)
 */
const char * getHtmlParam(int fd, int v) {
	char buf[20];
	int n = -1;
	float f = -1.0;
	switch(v) {
		case 0: n=configParams.getTestpressure()/10; sprintf(buf,"%d", n); break;
		case 1: f=configParams.getVentoffdelay()/1000.0; sprintf(buf,"%8.3f", f); break;
		case 2: f=configParams.getFilltime()/1000.0; sprintf(buf,"%8.3f", f); break;
		case 3: f=configParams.getStabilisationtime()/1000.0; sprintf(buf,"%8.3f", f); break;
		case 4: f=configParams.getIsolationdelay()/1000.0; sprintf(buf,"%8.3f", f); break;
		case 5: f=configParams.getMeasuretime()/1000.0; sprintf(buf,"%8.3f", f); break;
		case 6: f=configParams.getEvacuationtime()/1000.0; sprintf(buf,"%8.3f", f); break;
		case 7: f=configParams.getOffsetcomp()/10.0; sprintf(buf,"%8.1f", f); break;
		case 8: f=configParams.getAlarmleakrate()/100.0; sprintf(buf,"%8.2f", f); break;
		case 9: f=configParams.getAlarmdiffp()/10.0; sprintf(buf,"%8.1f", f); break;
		case 10: n=configParams.getSpare1()/10; sprintf(buf,"%d", n); break;

		//additional test params 13.01.2015
		case 20: n=configParams.getFixtureOPstart(); sprintf(buf,"%X", n); break;
		case 21: f=configParams.getFixturedelaytime()/1000.0; sprintf(buf,"%8.3f", f); break;
		case 22: n=configParams.getCheckinputs(); sprintf(buf,"%X", n); break;
		case 23: sprintf(buf,"%s", configParams.isEnablecheckinputs()?"checked=\"checked\"":""); break;
		case 24: n=configParams.getFixtureOPend(); sprintf(buf,"%X", n); break;
		case 25: n=configParams.getNexttestonpass(); sprintf(buf,"%d", n); break;
		case 26: n=configParams.getNexttestonfailoralarm(); sprintf(buf,"%d", n); break;
		case 27: sprintf(buf,"%s", configParams.isInhibitresults()?"checked=\"checked\"":""); break;
		case 28: f=configParams.getSpare2()/1000.0; sprintf(buf,"%8.3f", f); break; //GM 3/02/1015 Inter Test Delay
		case 29: f=configParams.getSpare3()/1000.0; sprintf(buf,"%8.3f", f); break; //GM 27/02/2017 Isolate valve Pulse
		case 30: sprintf(buf,"%s", configParams.isSpareflag1()?"checked=\"checked\"":""); break;

		default : f =-1.0; sprintf(buf,"%8.3f", f);
	}

//	sprintf(buf,"%d", n);
	writestring( fd, buf );
	return "\0"; //must return something
}

//2016-06-22 fixture control
const char * getHtmlFixtureParam(int fd, int v) {
	char buf[20];
	float f = -1.0;
	switch(v) {
		case 0: sprintf(buf,"%s", configParams.getFixtureControl().enabled[0]?"checked=\"checked\"":""); break;
		case 1: f=configParams.getFixtureControl().fixtime[0]/1000.0; sprintf(buf,"%8.3f", f); break;
		case 2: sprintf(buf,"%s", configParams.getFixtureControl().feedback[0]?"checked=\"checked\"":""); break;

		case 10: sprintf(buf,"%s", configParams.getFixtureControl().enabled[1]?"checked=\"checked\"":""); break;
		case 11: f=configParams.getFixtureControl().fixtime[1]/1000.0; sprintf(buf,"%8.3f", f); break;
		case 12: sprintf(buf,"%s", configParams.getFixtureControl().feedback[1]?"checked=\"checked\"":""); break;

		case 20: sprintf(buf,"%s", configParams.getFixtureControl().enabled[2]?"checked=\"checked\"":""); break;
		case 21: f=configParams.getFixtureControl().fixtime[2]/1000.0; sprintf(buf,"%8.3f", f); break;
		case 22: sprintf(buf,"%s", configParams.getFixtureControl().feedback[2]?"checked=\"checked\"":""); break;

		case 30: sprintf(buf,"%s", configParams.getFixtureControl().passmark?"checked=\"checked\"":""); break;
		case 31: f=configParams.getFixtureControl().passmarktime/1000.0; sprintf(buf,"%8.3f", f); break;
		case 32: sprintf(buf,"%s", configParams.getFixtureControl().failack?"checked=\"checked\"":""); break;

		case 40: sprintf(buf,"%s", configParams.getFixtureControl().fillToPres?"checked=\"checked\"":""); break;
		case 41: f=configParams.getFixtureControl().fillToPresTimer/1000.0; sprintf(buf,"%8.3f", f); break;

		default : sprintf(buf,"%8.3f", f);
	}

	writestring( fd, buf );
	return "\0"; //must return something
}

//Configuration labels
const char * getHtmlConfigLabel(int fd, int v) {
	char buf[40];
	sprintf(buf,"%s", NV_Params.TestSetupNV[v].idString);
	writestring( fd, buf );
	return "\0"; //must return something
}

/* =======================================================================================
 * Call back function to populate the Calibration Table
 * Note: Called back functions with parameters must be declared in htmlvar.h, with format
 * 		const char * function(int sock, int v, ...)
 */
const char * getHtmlCalib(int fd, int v) {
	char buf[20];
	int n = -1;
	float f = -1.0;
	switch(v) {
		case 50: f=calibParams[0].getValueMax()/10.0; sprintf(buf,"%8.1f", f); break;
		case 60: f=calibParams[2].getValueMax()/10.0; sprintf(buf,"%8.1f", f); break;
		case 70: f=calibParams[1].getValueMax()/10.0; sprintf(buf,"%8.1f", f); break;
		case 80: f=calibParams[3].getValueMax()/10.0; sprintf(buf,"%8.1f", f); break;
		case 51: n=calibParams[0].getCountMax(); sprintf(buf,"%d", n); break;
		case 61: n=calibParams[2].getCountMax(); sprintf(buf,"%d", n); break;
		case 71: n=calibParams[1].getCountMax(); sprintf(buf,"%d", n); break;
		case 81: n=calibParams[3].getCountMax(); sprintf(buf,"%d", n); break;
		case 52: f=calibParams[0].getValueMin()/10.0; sprintf(buf,"%8.1f", f); break;
		case 62: f=calibParams[2].getValueMin()/10.0; sprintf(buf,"%8.1f", f); break;
		case 72: f=calibParams[1].getValueMin()/10.0; sprintf(buf,"%8.1f", f); break;
		case 82: f=calibParams[3].getValueMin()/10.0; sprintf(buf,"%8.1f", f); break;
		case 53: n=calibParams[0].getCountMin(); sprintf(buf,"%d", n); break;
		case 63: n=calibParams[2].getCountMin(); sprintf(buf,"%d", n); break;
		case 73: n=calibParams[1].getCountMin(); sprintf(buf,"%d", n); break;
		case 83: n=calibParams[3].getCountMin(); sprintf(buf,"%d", n); break;
		case 54: f=calibParams[0].getGradient(); sprintf(buf,"%8.3f", f); break;
		case 64: f=calibParams[2].getGradient(); sprintf(buf,"%8.3f", f); break;
		case 74: f=calibParams[1].getGradient(); sprintf(buf,"%8.3f", f); break;
		case 84: f=calibParams[3].getGradient(); sprintf(buf,"%8.3f", f); break;
		case 55: f=calibParams[0].getOffset(); sprintf(buf,"%8.3f", f); break;
		case 65: f=calibParams[2].getOffset(); sprintf(buf,"%8.3f", f); break;
		case 75: f=calibParams[1].getOffset(); sprintf(buf,"%8.3f", f); break;
		case 85: f=calibParams[3].getOffset(); sprintf(buf,"%8.3f", f); break;

		default : n =-1; sprintf(buf,"%8.3f", f);
	}

//	sprintf(buf,"%d", n);
	writestring( fd, buf );
	return "\0"; //must return something
}

/* =======================================================================================
 * Call back function to populate the Options Table
 * Note: Called back functions with parameters must be declared in htmlvar.h, with format
 * 		const char * function(int sock, int v, ...)
 */
const char * getHtmlOption(int fd, int v) {
	char buf[50];
	switch(v) {
		case 100: sprintf(buf,"%d", NV_Params.OptionsNV.logPort); break;
		case 101: // Dump an IP address in ASCII IP string format to char buf (see utils.h)
				  snShowIP( buf, 20, NV_Params.OptionsNV.logIP ); break;
		case 102: sprintf(buf,"%d", NV_Params.OptionsNV.logLevel); break;
		case 103: sprintf(buf,"%d", NV_Params.OptionsNV.testpTol); break;
		case 104: sprintf(buf,"%8.1f", NV_Params.OptionsNV.diffpAbort/10.0); break;
		case 105: sprintf(buf,"%8.1f", NV_Params.OptionsNV.diffpNegAlarm/10.0); break;
		case 106: sprintf(buf,"%c", NV_Params.OptionsNV.LT_ID); break;
//		case 107: sprintf(buf,"%c", NV_Params.OptionsNV.LTB_ID); break;
		case 107: sprintf(buf,"%s", NV_Params.OptionsNV.startenable?"checked=\"checked\"":""); break;
		case 110: sprintf(buf,"%d", NV_Params.OptionsNV.option1); break;
		case 111: sprintf(buf,"%8.3f", NV_Params.OptionsNV.option2); break;
		case 112: sprintf(buf,"%d", NV_Params.OptionsNV.option3); break;
		case 113: sprintf(buf,"%8.3f", NV_Params.OptionsNV.option4); break;
		case 114: sprintf(buf,"%d", NV_Params.OptionsNV.option5); break;
		case 115: sprintf(buf,"%8.3f", NV_Params.OptionsNV.option6); break;

		case 116: sprintf(buf,"%s", NV_Params.OptionsNV.password1); break;
		case 117: sprintf(buf,"%s", NV_Params.OptionsNV.password2); break;

		case 120: sprintf(buf,"%s", AppName); break;

		default : float f=-1.0; sprintf(buf,"%8.3f", f);
	}

	writestring( fd, buf );
	return "\0"; //must return something
}

/*=======================================================================================*/
/* Callbacks for status line defined in status.html. */

/**
* @brief getHTMLTicks
*  Writes the current TimeTick to the given socket/file descriptor
*/
void getHTMLTicks(int sock, PCSTR url) {
	char tick[40];
	siprintf(tick, "%lu", TimeTick);
	writestring(sock, tick);
}

/**
* @brief getHTMLStatus
*  Writes the current status to the given socket/file descriptor
*/
void getHTMLStatus(int sock, PCSTR url) {
	char status[30];
	siprintf(status, " %02d/%02d/%d  %02d:%02d:%02d", //lz was " %d/%02d/%d
			dateTime.tm_mday,
			dateTime.tm_mon+1,
			dateTime.tm_year+1900,
			dateTime.tm_hour,
			dateTime.tm_min,
			dateTime.tm_sec);
	writestring(sock, status);
}


/**
* @brief getHTMLTestStatus
*  Writes the current instrument test status to the given socket/file descriptor
*/
// Test sequence text states - index with stepCountA-B
static char* teststate[40] = {"Waiting  ","Read Prog","Load Prog","Fix OPs  ","Fix OPs  ", //Steps 0 to 4
		"Fix IPs  ","Spare    ","Fixture A ","Fixture B ","Fixture C ","Spare    ","Spare    ", //Steps 5 to 11
		"Vent Off ","Filling  ","Stabilise","Dwell    ","Measure  ","Pass     ","Fail     ", //Steps 12 to 18
		"Fault    ","Vent     ","Chain Tst","Fixture C ","Fixture B ","Fail Ack? ","Fixture A ", //Steps 19 to 25
		"Done     ","Spare    ","Spare    ","Spare    ","Spare    ","Spare    ", //Steps 26 to 31
		"Spare    ","DiffP Cal","DiffP Cal","TestP Cal","TestP Cal","End Cal  ", //Steps 32 to 37
		"Spare    ","Manual Ops  "}; //Steps 38 to 39

const char * getHtmlTestState(int sock, int module) {
	char buf[20];
//	if (module == 1)
//		siprintf(buf, "%s", teststate[stepCountB]);
//	else
		siprintf(buf, "Test:%d  %s", testConfigA, teststate[stepCount]);
	writestring( sock, buf );
	return "\0"; //must return something
}

/*=======================================================================================*/
/* Callbacks for calibration start/stop buttons in status.html. */

const char * getHtmlBusy(int sock, int column) {
	char buf[128];
	sprintf(buf, "<td><button type=\"submit\" name=\"calstate\" class=\"%s\" value=\"%d\">%s</td>",
						(calibrationBusy[column]?"red":"green"), (column+5)*10+4, (calibrationBusy[column]?"Stop":"Start"));
	writestring( sock, buf );
	return "\0"; //must return something
}

/*-----Current Configuration ----*/
extern int selectedConfig; //first defined in formcode.cpp
//extern int appliedConfig; //first defined in formcode.cpp

/*Following callback is used on html pages as a hidden element.
 * It is inspected 'onload' by javascript.
 */
void getHTMLSelectedConfig(int sock, PCSTR url) {
//	iprintf("Ajax selectedconfig=%d\r\n",selectedConfig);
	char buf[20];
	sprintf(buf,"%d",selectedConfig);
	writestring(sock,buf);
}

void getHTMLAppliedConfig(int sock, PCSTR url) {
	char buf[20];
	sprintf(buf,"%d",appliedConfig);
	writestring(sock,buf);
}

/*-----Authentication ----*/
extern int authenticated; //first defined in formcode.cpp

/*Following callback is used on html pages as a hidden element.
 * It is inspected 'onload' by javascript.
 * If not authenticated javascript redirects to password page*/
void getHTMLAuthentication(int sock, PCSTR url) {
	char buf[20];
	sprintf(buf,"%d",authenticated);
	writestring(sock,buf);
}

/*
 * Remove any previous authentication.
 * Called within a hidden field when index.html is loaded.
 */
void resetAuthentication(int sock, PCSTR url) {
	authenticated=0; //reset authentication
	char buf[20];
	sprintf(buf,"%d",authenticated);
	writestring(sock,buf);

	 /* Additionally, reset selectedConfiguration to appliedConfiguration
	  * 	so that it is selected on re-entry to config.html
	  */
//	 selectedConfig=appliedConfig;
}

// stepCount <> 0 disables Test selection in index.html
void getHTMLStepCount(int sock, PCSTR url) {
	char buf[20];
	sprintf(buf,"%d",stepCount);
	writestring(sock,buf);
}


