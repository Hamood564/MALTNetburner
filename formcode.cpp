/**
 * formcode.cpp
 * Processes user input from MALT web pages
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT1.1
 * @date 2015-01-13
 */

#include "predef.h"
#include <ctype.h>
#include <stdio.h>
#include <htmlfiles.h>
#include <http.h>
#include <stdlib.h>
#include <system.h> //- SaveParams & GetParams
#include <string.h> //strcpy, strlen, str.compare
#include <constants.h>
#include "configParams.h"
#include "CalibrationHtmlData.h"
#include <time.h>
#include <utils.h>
#include <math.h>
//#include <mcf5441x_rtc.h>
#include "mcf5441x_rtc.h"  //Local 54415 RTC support

#include "MLT_globalExterns.h"
#include "test_seq.h"  // only used to test printTimeStruct
#include "importexport/userParameters.h" //import and export functions
#include "message_log.h"

#define SIZEOF_DATA 256
#define SIZEOF_FIELD 32

#define HOMEPAGE 0
#define CONFIGPAGE 1
#define CALIBPAGE 2
#define OPTIONPAGE 3
#define FIXCONTROLPAGE 4

char data[SIZEOF_DATA];
char field[SIZEOF_FIELD];
int authenticated; //authentication to access config pages. 0=not authenticated.
int selectedConfig=0;
//int appliedConfig=0; // moved to globals

/* Configuration and calibration variables - main declarations
 *  ...these variables are used in the ajax callback.
 * They are used to gather data from the webpages, before
 * validation (...to do...) and transfer to NV_Params and vice-versa.
 */
TestConfigParams configParams;
CalibrationHtmlData calibParams[4]; //moduleA diffp =0, moduleB diffp =1, moduleA testp = 2, moduleB testp = 3

#include "CalibrationHtmlData.h"

void printNV_Params();

//=====Helper functions for GetParams and SaveParams=========

void getConfigParamsFromSavedData() {

	// Read the stored data
	NV_Params_Struct *pItem = (NV_Params_Struct*) GetUserParameters();
	iprintf("Retrieved Test Configuration Parameters: size=%lu bytes\r\n", sizeof(*pItem));

	//copy to NV_Params
	NV_Params = *pItem;

	//copy test params to actual test setup
//	TestSetup = NV_Params.TestSetupNV[0];  //TestSetup is the test parameters used in the actual test sequence
	TestSetup = NV_Params.TestSetupNV[selectedConfig];  //TestSetup is the test parameters used in the actual test sequence

	//copy retrieved data to test configuration web interface variables
//	configParams.setTestParams(NV_Params.TestSetupNV[0]);  // Only working with 1 set of test parameters at the present
	configParams.setTestParams(NV_Params.TestSetupNV[selectedConfig]);

	//copy retrieved data to calibration web interface variables
	calibParams[0].setCalib(NV_Params.CalDataNV[0]);	//module A Differential Pressure
	calibParams[1].setCalib(NV_Params.CalDataNV[1]);	//module B Differential Pressure
	calibParams[2].setCalib(NV_Params.CalDataNV[2]);	//module A Test Pressure
	calibParams[3].setCalib(NV_Params.CalDataNV[3]);	//module B Test Pressure
}

/* Extract values form the fields on the form and write to a
 * TestConfigParams data structure, ready to save.
 */

//helper function
int extractIntField(PCSTR fieldName, PCSTR pData ) {
	int result = -1;
	if (ExtractPostData(fieldName, pData, field, SIZEOF_FIELD) != -1) {
		result = atoi(field);
	}
//	iprintf("Extracted %s= %d\r\n", fieldName, result);
	return result;
}

int extractIntFromHex(PCSTR fieldName, PCSTR pData ) {
	int result = -1;
	if (ExtractPostData(fieldName, pData, field, SIZEOF_FIELD) != -1) {
		result = (int) strtol(field, NULL, 16);
//		iprintf("converted %s to int %d", field, result);
	}
//	iprintf("Extracted %s= %d\r\n", fieldName, result);
	return result;
}
int extractBoolField(PCSTR fieldName, PCSTR pData ) {
	int result = false;
	if (ExtractPostData(fieldName, pData, field, SIZEOF_FIELD) != -1) {
		result = true;
	}
//	iprintf("Extracted %s= %d\r\n", fieldName, result);
	return result;
}

float extractFloatField(PCSTR fieldName, PCSTR pData ) {
	float result = -1.0;
	if (ExtractPostData(fieldName, pData, field, SIZEOF_FIELD) != -1) {
		result = (float) atof(field);
	}
//	iprintf("Extracted %s= %d\r\n", fieldName, result);
	return result;
}

/* pData --> double*multiplier --. nearest int */
int extractFloatAsInt(PCSTR fieldName, PCSTR pData, int multiplier ) {
	float result = -1;
	if (ExtractPostData(fieldName, pData, field, SIZEOF_FIELD) != -1) {
		double temp = round(atof(field)*multiplier);
		result = int(temp);
	}
//	iprintf("Extracted %s= %d\r\n", fieldName, result);
	return result;
}

float extractCharField(PCSTR fieldName, PCSTR pData ) {
	float result = '\0';
	if (int b = ExtractPostData(fieldName, pData, field, SIZEOF_FIELD) != -1) {
		if (b>0) result = field[0];
	}
//	iprintf("Extracted %s= %d\r\n", fieldName, result);
	return result;
}

//helper function
IPADDR extractIPField(PCSTR fieldName, PCSTR pData ) {
	IPADDR result; // = -1;
	//result.print();
	//iprintf("\r\n IPADDR result is null: %s \r\n", (result.IsNull()?"true":"false"));
	if (ExtractPostData(fieldName, pData, field, SIZEOF_FIELD) != -1) {
		result = AsciiToIp(field);
	}
//	result.print();
//	iprintf("Extracted %s= %d\r\n", fieldName, result);
	return result;
}

//note 'index' is the configuration 1..4. Unused in this version.
void applyConfigParams(char *pData) { //, int index) { - redundant

	//extract the data from each field, and assign to corresponding field of configParams
	configParams.setTestpressure(extractIntField("testpressure", pData)*10);
	configParams.setVentoffdelay(extractFloatAsInt("ventoffdelay", pData, 1000.0));
	configParams.setFilltime(extractFloatAsInt("filltime", pData, 1000.0));
	configParams.setStabilisationtime(extractFloatAsInt("stabilisationtime", pData, 1000.0));
	configParams.setIsolationdelay(extractFloatAsInt("isolationdelay", pData, 1000.0));
	configParams.setMeasuretime(extractFloatAsInt("measuretime", pData, 1000.0));
	configParams.setEvacuationtime(extractFloatAsInt("evacuationtime", pData, 1000.0));
	configParams.setOffsetcomp(extractFloatAsInt("offsetcomp", pData, 10.0));
	configParams.setAlarmleakrate(extractFloatAsInt("alarmleakrate", pData,100.0));
	configParams.setAlarmdiffp(extractFloatAsInt("alarmdiffp", pData, 10.0));
	configParams.setSpare1(extractIntField("spare1", pData)*10);

	configParams.setFixtureOPstart(extractIntFromHex("fixtureoutputstart", pData));
	configParams.setFixturedelaytime(extractFloatAsInt("fixturedelaytime", pData, 1000.0));
	configParams.setCheckinputs(extractIntFromHex("checkinputs", pData));
	configParams.setEnablecheckinputs(extractBoolField("enablecheckinputs", pData));
	configParams.setFixtureOPend(extractIntFromHex("fixtureOPend", pData));
	configParams.setNexttestonpass(extractIntField("nexttestonpass", pData));
	configParams.setNexttestonfailoralarm(extractIntField("nexttestonfailoralarm", pData));
	configParams.setInhibitresults(extractBoolField("inhibitresults", pData));
	configParams.setSpare2(extractFloatAsInt("spare2", pData, 1000.0));  //GM 3/02/2015 Inter Test Delay
	configParams.setSpare3(extractFloatAsInt("spare3", pData, 1000.0));  //GM 27/02/2017 Isolate valve Pulse
	configParams.setSpareflag1(extractBoolField("spareflag1", pData));

//	TestSetup = configParams.getTestParams();

//	NV_Params.TestSetupNV[0] = configParams.getTestParams();
//	printNV_Params();

//	TestSetup = NV_Params.TestSetupNV[0];
}

//2016-06-23 fixture control
//Note: called when returning to main Config page from FixtureControl page.
//Not saved to user data area until Config is saved.
void applyConfigFixtureParams(char *pData) {
	FixtureControl fc; //temp structure container
   	fc.enabled[0] = extractBoolField("enablecontrol1", pData);
   	fc.fixtime[0] = extractFloatAsInt("fixturetime1", pData, 1000.0);
   	fc.feedback[0] = extractBoolField("enablefeedback1", pData);

   	fc.enabled[1] = extractBoolField("enablecontrol2", pData);
   	fc.fixtime[1] = extractFloatAsInt("fixturetime2", pData, 1000.0);
   	fc.feedback[1] = extractBoolField("enablefeedback2", pData);

   	fc.enabled[2] = extractBoolField("enablecontrol3", pData);
   	fc.fixtime[2] = extractFloatAsInt("fixturetime3", pData, 1000.0);
   	fc.feedback[2] = extractBoolField("enablefeedback3", pData);

   	fc.passmark = extractBoolField("enablepassmark", pData);
    fc.passmarktime = extractFloatAsInt("passmarktime", pData, 1000.0);
    fc.failack = extractBoolField("enablefailack", pData);

   	fc.fillToPres = extractBoolField("enablefillToPres", pData);
    fc.fillToPresTimer = extractFloatAsInt("fillToPrestime", pData, 1000.0);

    //copy to current (html) config variable
    configParams.setFixtureControl(fc);
}

//note 'index' is the configuration 1..4. Unused in this version. - LZ:2016-06-23+ removed
void applyCalibParams(char *pData) {

	//extract the data from each field, and assign to corresponding field of configParams
	calibParams[0].setValueMax(extractFloatAsInt("diffvaluemaxA", pData, 10.0));
	calibParams[2].setValueMax(extractFloatAsInt("testvaluemaxA", pData, 10.0));
	calibParams[1].setValueMax(extractFloatAsInt("diffvaluemaxB", pData, 10.0));
	calibParams[3].setValueMax(extractFloatAsInt("testvaluemaxB", pData, 10.0));
	calibParams[0].setCountMax(extractIntField("diffcountmaxA", pData));
	calibParams[2].setCountMax(extractIntField("testcountmaxA", pData));
	calibParams[1].setCountMax(extractIntField("diffcountmaxB", pData));
	calibParams[3].setCountMax(extractIntField("testcountmaxB", pData));
	calibParams[0].setValueMin(extractFloatAsInt("diffvalueminA", pData, 10.0));
	calibParams[2].setValueMin(extractFloatAsInt("testvalueminA", pData, 10.0));
	calibParams[1].setValueMin(extractFloatAsInt("diffvalueminB", pData, 10.0));
	calibParams[3].setValueMin(extractFloatAsInt("testvalueminB", pData, 10.0));
	calibParams[0].setCountMin(extractIntField("diffcountminA", pData));
	calibParams[2].setCountMin(extractIntField("testcountminA", pData));
	calibParams[1].setCountMin(extractIntField("diffcountminB", pData));
	calibParams[3].setCountMin(extractIntField("testcountminB", pData));
	calibParams[0].setGradient(extractFloatField("diffgradientA", pData));
	calibParams[2].setGradient(extractFloatField("testgradientA", pData));
	calibParams[1].setGradient(extractFloatField("diffgradientB", pData));
	calibParams[3].setGradient(extractFloatField("testgradientB", pData));
	calibParams[0].setOffset(extractFloatField("diffoffsetA", pData));
	calibParams[2].setOffset(extractFloatField("testoffsetA", pData));
	calibParams[1].setOffset(extractFloatField("diffoffsetB", pData));
	calibParams[3].setOffset(extractFloatField("testoffsetB", pData));

	//note: fixture control parameters are applied in a separate function when returning from fixtureControl.html page

	//test set up with these calibrations
	/* ...to do... */

}

void applyOptions(char *pData) {
	iprintf("Applying options\r\n");
	NV_Params.OptionsNV.logPort= extractIntField("logPort", pData);
	NV_Params.OptionsNV.logIP= extractIPField("logIP", pData);
	NV_Params.OptionsNV.logLevel= extractIntField("loglevel", pData);
	NV_Params.OptionsNV.testpTol= extractIntField("testpTol", pData);
	NV_Params.OptionsNV.diffpAbort= extractFloatAsInt("diffpAbort", pData, 10);
	NV_Params.OptionsNV.diffpNegAlarm= extractFloatAsInt("diffpNegAlarm", pData, 10);
	NV_Params.OptionsNV.LT_ID= extractCharField("LTA_ID", pData);
//	NV_Params.OptionsNV.LTB_ID= extractCharField("LTB_ID", pData);
	NV_Params.OptionsNV.startenable = extractBoolField("startenable", pData);
	NV_Params.OptionsNV.option1= extractIntField("option1", pData);
	NV_Params.OptionsNV.option2= extractFloatField("option2", pData);
	NV_Params.OptionsNV.option3= extractIntField("option3", pData);
	NV_Params.OptionsNV.option4= extractFloatField("option4", pData);
	NV_Params.OptionsNV.option5= extractIntField("option5", pData);
	NV_Params.OptionsNV.option6= extractFloatField("option6", pData);
	ExtractPostData("password1", pData, NV_Params.OptionsNV.password1, sizeof(NV_Params.OptionsNV.password1));
	ExtractPostData("password2", pData, NV_Params.OptionsNV.password2, sizeof(NV_Params.OptionsNV.password2));
}

	//===========Save values in Params area of on-chip flash memory========
void saveNV_Params() {
	iprintf("Saving SelectedConfig=%d\r\n", selectedConfig);
//	NV_Params.TestSetupNV[0] = configParams.getTestParams();
	NV_Params.TestSetupNV[selectedConfig] = configParams.getTestParams();
	NV_Params.CalDataNV[0] = calibParams[0].getCalib(); //module A Differential Pressure
	NV_Params.CalDataNV[1] = calibParams[1].getCalib(); //module B Differential Pressure
	NV_Params.CalDataNV[2] = calibParams[2].getCalib(); //module A Test Pressure
	NV_Params.CalDataNV[3] = calibParams[3].getCalib(); //module B Test Pressure

	//sanity check
//	printNV_Params();
//	printf("Saving TestParams,size=%lu bytes\r\n", sizeof(NV_Params));
//	printf("Saving TestParams,size=%lu bytes\r\n", sizeof(NV_Params_Struct)); // should be the same

	//Do sanity check (above) before committing to this code.
	//Ensure you are not writing more than 128K (MOD54415)
	int b = 0;
	if ((b = SaveUserParameters(&NV_Params, sizeof(NV_Params_Struct))) != 0) {
		iprintf("Parameters saved in User Parameter area. %d bytes\r\n", b);
	} else {
		iprintf("Programming failed. %d bytes written \r\n", b);
	}
}
/*-------------------------------------------------------------------*/
void setDateTime(char *pData) {
	dateTime.tm_wday = extractIntField("dayOfWeek", pData);
	dateTime.tm_mday = extractIntField("dayOfMonth", pData);
	dateTime.tm_mon = extractIntField("month", pData);
	dateTime.tm_year = extractIntField("year", pData)-1900;
	dateTime.tm_hour = extractIntField("hh", pData);
	dateTime.tm_min = extractIntField("mm", pData);
	dateTime.tm_sec = extractIntField("ss", pData);
}

bool TestPassword( const char *pPass ) {
    return strcmp(pPass, NV_Params.OptionsNV.password1)==0
    		|| strcmp(pPass, NV_Params.OptionsNV.password2)==0
    		|| strcmp(pPass,"TQCAutomation&TestSolutions")==0;
}

/*-------------------------------------------------------------------
 * When you write a MyDoPost() function and register it, it will
 * be called when a web browser submits a Form POST. This is how
 * the data entered in the web page form gets to the device.
 *-----------------------------------------------------------------*/
int MyDoPost(int sock, char *url, char *pData, char *rxBuffer) {
	int value;
	extern ADArray   ADAve;

	iprintf("MyDoPost has url %s and data \"%s\"\r\n", url, pData);

//---------------replace iprintf above with this code-----------
	char buf[256]; //for UDP/TCP message log

//	sprintf(buf, "MyDoPost has url %s and data \"%s\"\r\n", url, pData);
//	iprintf("%s", buf);
//	logMessage(LOGPRIOLOW, buf);
//---------------replace iprintf above with this code-----------

	if ((ExtractPostData("startA", pData, data, SIZEOF_DATA) != -1)
		 ||(ExtractPostData("resetA", pData, data, SIZEOF_DATA) != -1))
	{	//Process button on index.html page.
//			iprintf("Extracted name= %s\r\n", data);
			value = atoi(data);
			switch (value) {
				case 1: webStart = true; break;
				case 2: webReset = true; break;
			}
	}
	else if (ExtractPostData("applyTest", pData, data, SIZEOF_DATA) != -1) {
		iprintf("Extracted name= %s\r\n", data);
		value = atoi(data);
		appliedConfig=value;
		TestSetup = NV_Params.TestSetupNV[value];
		//optional extra... can remove
		selectedConfig=appliedConfig; //so that it is selected on entry to config.html
		configParams.setTestParams(NV_Params.TestSetupNV[value]); //and keep web data in step
	}
	else if(ExtractPostData("pwdbtn", pData, data, SIZEOF_DATA) != -1) {
		//check password from password.html
//		iprintf("PasswordOkBtn - Extracted name= %s\r\n", data);
		if (atoi(data)!=1) { //cancel requested - back to index.htm
			url = "index.htm";
		}
		else {
			char password[SIZEOF_FIELD] ="";

			(ExtractPostData("pwd", pData, password, SIZEOF_FIELD));
			authenticated = TestPassword(password) ? 1 : 0;
			if (authenticated) {
				url = "config.html";
			}
		}
	}
	else { //Authentication required for all the posts below.
		if (!authenticated) {
			//send to password page
			url = "password.html";
		}
		//ok to process
		else if (ExtractPostData("saveParams", pData, data, SIZEOF_DATA) != -1) {
			//data is now the query string of the url with key-value pairs.
//			iprintf("saveParams - Extracted name= %s\r\n", data);

			int page = atoi(data);
			if (page==CONFIGPAGE) { //config page
				selectedConfig= extractIntField("config", pData ); //redundant?????
				iprintf("Applying SelectedConfig=%d", selectedConfig);
				applyConfigParams(pData); //, selectedConfig);
			}
			else
			if (page==OPTIONPAGE) {
				applyOptions(pData);
			}
			saveNV_Params();

		} else if (ExtractPostData("getParams", pData, data, SIZEOF_DATA) != -1) {
//			iprintf("getParams - Extracted name= %s\r\n", data);
			getConfigParamsFromSavedData();

		} else if (ExtractPostData("revert", pData, data, SIZEOF_DATA) != -1) {
//			iprintf("revert - Extracted name= %s\r\n", data);
			int page = atoi(data);
			//TO DO: set TestConfigData according to the selected config
//			if (page==CONFIGPAGE) { //config page
////				configParams.setTestParams(NV_Params.TestSetupNV[0]);
////				TestSetup = NV_Params.TestSetupNV[0];
//				configParams.setTestParams(NV_Params.TestSetupNV[selectedConfig]);
//				TestSetup = NV_Params.TestSetupNV[selectedConfig];
//			}
//			else
			if (page==CALIBPAGE) { //calib page
				//reset calib data to values in NV_Params
				calibParams[0].setCalib(NV_Params.CalDataNV[0]); //module A Differential Pressure
				calibParams[1].setCalib(NV_Params.CalDataNV[1]); //module B Differential Pressure
				calibParams[2].setCalib(NV_Params.CalDataNV[2]); //module A Test Pressure
				calibParams[3].setCalib(NV_Params.CalDataNV[3]); //module B Test Pressure
				//test set up with these calibrations
				/* ...to do... */
			}

		}
		//=============Import/export settings=====LZ Dec 2019=====================
		else if (ExtractPostData("inout", pData, data, SIZEOF_DATA) != -1) {
			int inout = atoi(data); //0=export, 1=import

			//import data from memory card.
			char filename[128];
			if (ExtractPostData("filename", pData, filename, SIZEOF_DATA) > 0 ) {
				sprintf(buf, "inout=%d filename:%s\r\n", inout, filename);
				logMessage(LOGPRIOLOW, buf);

				if (inout==0)
					setExportReq(filename);
				else {
					setImportReq(filename);
					//OSTimeDly(TICKS_PER_SECOND);
					//i.e. give time for import to complete before page is returned
					//otherwise may need to refresh browser to see changes
				}
			}

		} else if (ExtractPostData("apply", pData, data, SIZEOF_DATA) != -1) {
			iprintf("apply - Extracted name= %s\r\n", data);
			int page = atoi(data);
			if (page==CONFIGPAGE) { //config page
				selectedConfig= extractIntField("config", pData );  //2016-06-28LZ: changed from appliedConfig to selectedConfig
				applyConfigParams(pData); //, appliedConfig);
				url = "fixtureControl.html";
//				saveNV_Params();
			}
			else
			if (page==CALIBPAGE) { //calib page
				applyCalibParams(pData);
			}
			else if(page==FIXCONTROLPAGE) { //2016-06-23
				applyConfigFixtureParams(pData);
//				configParams.printConfigParameters();
				//url = "config.html"; //lz dec2019
			}
		}

//		else if (ExtractPostData("renamedconfig", pData, data, SIZEOF_DATA) != -1 && strlen(data)>0) {
//				iprintf("renamedconfig - Extracted name= %s\r\n", data);
//				configParams.setIdString(data);
//				strcpy(NV_Params.TestSetupNV[selectedConfig].idString, data);
//		}
		else if (ExtractPostData("config", pData, data, SIZEOF_DATA) != -1) {

			if (ExtractPostData("renamedconfig", pData, data, SIZEOF_DATA) != -1 && strlen(data)>0) {
							iprintf("renamedconfig - Extracted name= %s\r\n", data);
							configParams.setIdString(data);
							strcpy(NV_Params.TestSetupNV[selectedConfig].idString, data);
							/*lz-2015-01-20: should this be automatically followed by a save? i.e...*/
							applyConfigParams(pData); //, selectedConfig);
							saveNV_Params();
			}
			else {
	//			iprintf("config - Extracted name= %s\r\n", data);
				//set TestConfigData according to the selection config1..config4
				selectedConfig= extractIntField("config", pData );
				iprintf("temp Config= %d\r\n", selectedConfig);
				configParams.setTestParams(NV_Params.TestSetupNV[selectedConfig]);
				configParams.printConfigParameters();
			}

		} else if ((ExtractPostData("calmax", pData, data, SIZEOF_DATA) != -1)
				 ||(ExtractPostData("calmin", pData, data, SIZEOF_DATA) != -1)
				 ||(ExtractPostData("calstate", pData, data, SIZEOF_DATA) != -1)) {

//			iprintf("Extracted name= %s\r\n", data);
			value = atoi(data);
			switch (value) {

				//calmax buttons
				case 50: calibParams[0].setCountMax(ADAve.chan0); break;
				case 60: calibParams[2].setCountMax(ADAve.chan2); break;
				case 70: calibParams[1].setCountMax(ADAve.chan1); break;
				case 80:
//					calibParams[3].setCountMax(ADAve.chan3);
//Crude Man Ops - Toggle FixB OP
					if ( (stepCount == RESETSTEP) || (stepCount == MANUALOPSSTEP) ) {
						stepCount = MANUALOPSSTEP;
						if (IO_OPs & 0x20) {
							IO_OPs &= 0x00df;
//							calibrationBusy[3] = false; //This button doesnt highlight
						}
						else {
							IO_OPs |= 0x0020;
//							calibrationBusy[3] = true; //This button doesnt highlight
						}
					}
					break;

//calmin buttons
				case 52: calibParams[0].setCountMin(ADAve.chan0); break;
				case 62: calibParams[2].setCountMin(ADAve.chan2); break;
				case 72: calibParams[1].setCountMin(ADAve.chan1); break;
				case 82:
//					calibParams[3].setCountMin(ADAve.chan3);
//Crude Man Ops - Toggle FixB OP
					if ( (stepCount == RESETSTEP) || (stepCount == MANUALOPSSTEP) ) {
						stepCount = MANUALOPSSTEP;
						if (IO_OPs & 0x40) {
							IO_OPs &= 0x00bf;
							calibrationBusy[3] = false;
						}
						else {
							IO_OPs |= 0x0040;
							calibrationBusy[3] = true;
						}
					}
					break;

//calstate buttons (start/stop)
				case 54:
					if (stepCount == RESETSTEP) {
						stepCount = DIFFPCAL1;
						calibrationBusy[0] = true; //toggle busy flag for start/stop button
					}
					else if (stepCount == DIFFPCAL2) {
						stepCount = ENDCALSTEP; //26/01/2015 Changed from EVACSTEP in 4 places
						calibrationBusy[0] = false; //toggle busy flag for start/stop button
					}
					break;
				case 64:
					if (stepCount == RESETSTEP) {
						stepCount = TESTPCAL1;
						calibrationBusy[1] = true;
					}
					else if (stepCount == TESTPCAL2) {
						stepCount = ENDCALSTEP;
						calibrationBusy[1] = false;
					}
					break;
				case 74:
//Crude Man Ops - Toggle Marker OP
					if ( (stepCount == RESETSTEP) || (stepCount == MANUALOPSSTEP) ) {
						stepCount = MANUALOPSSTEP;
						if (IO_OPs & 0x10) {
							IO_OPs &= 0x00ef;
							calibrationBusy[2] = false; //remove button highlight
						}
						else {
							IO_OPs |= 0x0010;
							calibrationBusy[2] = true; //highlight button
						}
					}
//					if (stepCountB == RESETSTEP) {
//						stepCountB = DIFFPCAL1;
//						calibrationBusy[2] = true;
//					}
//					else if (stepCountB == DIFFPCAL2) {
//						stepCountB = ENDCALSTEP;
//						calibrationBusy[2] = false;
//					}
					break;
				case 84:
//Crude Man Ops - Toggle FixA OP
					if ( (stepCount == RESETSTEP) || (stepCount == MANUALOPSSTEP) ) {
						stepCount = MANUALOPSSTEP;
						if (IO_OPs & 0x80) {
							IO_OPs &= 0x007f;
							calibrationBusy[3] = false; //remove button highlight
						}
						else {
							IO_OPs |= 0x0080;
							calibrationBusy[3] = true; //highlight button
						}
					}
//					if (stepCountB == RESETSTEP) {
//						stepCountB = TESTPCAL1;
//						calibrationBusy[3] = true;
//					}
//					else if (stepCountB == TESTPCAL2) {
//						stepCountB = ENDCALSTEP;
//						calibrationBusy[3] = false;
//					}
					break;

				default : ;
			}
		}

		else if(ExtractPostData("setRTC", pData, data, SIZEOF_DATA) != -1) {
//			iprintf("setRTC - Extracted name= %s\r\n", data);

			setDateTime(pData); //set dateTime global variable
			MCF541X_RTCSetTime(dateTime); //set RTC time
			MCF541X_RTCSetSystemFromRTCTime(); //set system time from RTC time

//			printTimeStruct(dateTime); iprintf("\r\n");
		}
		else {
			iprintf("Error in extracting data from form \r\n");

		}
	}

	// We have to respond to the post with a new HTML page...
	// In this case we will redirect so the browser will go to that URL for the response...
	RedirectResponse(sock, url); //stay on same page unless changed in code above

	return 0;
}

/*-------------------------------------------------------------------
 * Function to register the MyDoPost() function as the POST handler
 * ----------------------------------------------------------------*/
void RegisterPost() {
	SetNewPostHandler(MyDoPost);
}


//Only used to separately init the Options
void initNV_ParamsOptions() {
	NV_Params.OptionsNV.logPort = 9284;
	NV_Params.OptionsNV.logIP = AsciiToIp6("192.168.115.200");
	NV_Params.OptionsNV.logLevel = 1; //Only High Priority messages sent
	NV_Params.OptionsNV.testpTol = 20; // +- 20% tolerance
	NV_Params.OptionsNV.diffpAbort = 7500; // 750.0 Pa max allowed
	NV_Params.OptionsNV.diffpNegAlarm = -200; // -20.0 Pa Reference Alarm
	NV_Params.OptionsNV.LT_ID = 'A';
	NV_Params.OptionsNV.startenable = false;
	NV_Params.OptionsNV.option1= 0;
	NV_Params.OptionsNV.option2= 0.0;
	NV_Params.OptionsNV.option3= 0;
	NV_Params.OptionsNV.option4= 0.0;
	NV_Params.OptionsNV.option5= 0;
	NV_Params.OptionsNV.option6= 0.0;

	strcpy(NV_Params.OptionsNV.password1,"highpass");
	strcpy(NV_Params.OptionsNV.password2,"lowpass");
}

//Initialise the NV Saved Parameters - called if an unprogrammed device is detected
void initNV_Params() {
	char buf[40];

	for(int i=0; i<MAXCONFIGS; i++) {
//		NV_Params.TestSetupNV[i] = LTA_Params[i];
//		NV_Params.TestSetupNV[i] = LTA_Params[0];
		sprintf(buf,"%d: config%d",i, i);
//		configParams.setIdString(buf);
//		NV_Params.TestSetupNV[i] = configParams.getTestParams();

		strcpy(NV_Params.TestSetupNV[i].idString, buf);
		NV_Params.TestSetupNV[i].testpressure=5000;
		NV_Params.TestSetupNV[i].initdelay=100;
		NV_Params.TestSetupNV[i].ventoffdelay=200;
		NV_Params.TestSetupNV[i].filltime=2000;
		NV_Params.TestSetupNV[i].stabilisationtime=5000;
		NV_Params.TestSetupNV[i].isolationdelay=400;
		NV_Params.TestSetupNV[i].measuretime=2000;
		NV_Params.TestSetupNV[i].evacuationtime=1000;
		NV_Params.TestSetupNV[i].offsetcomp=0;
		NV_Params.TestSetupNV[i].alarmleakrate=5000,
		NV_Params.TestSetupNV[i].alarmdiffp=1000,
		NV_Params.TestSetupNV[i].spare1=0;
		NV_Params.TestSetupNV[i].fixtureOPstart=0; //0..FF 0d.p
		NV_Params.TestSetupNV[i].fixturedelaytime=0; //display to 3d.p
		NV_Params.TestSetupNV[i].checkinputs=0;
		NV_Params.TestSetupNV[i].enablecheckinputs=false;
		NV_Params.TestSetupNV[i].fixtureOPend=0; //0..FF 0d.p
		NV_Params.TestSetupNV[i].nexttestonpass=0;	//0..F
		NV_Params.TestSetupNV[i].nexttestonfailoralarm=0;	//0..F
		NV_Params.TestSetupNV[i].inhibitresults=false;
		NV_Params.TestSetupNV[i].spare2=0;
		NV_Params.TestSetupNV[i].spare3=0;
		NV_Params.TestSetupNV[i].spareflag1=false;

		//lz Oct 2020
		NV_Params.TestSetupNV[i].spareint10 = 0; //Spare for future use
		NV_Params.TestSetupNV[i].spareint11 = 0; //Spare for future use
		NV_Params.TestSetupNV[i].spareint12 = 0; //Spare for future use
		NV_Params.TestSetupNV[i].spareint13 = 0; //Spare for future use
		NV_Params.TestSetupNV[i].spareflag10 = false; //Spare for future use
		NV_Params.TestSetupNV[i].spareflag11 = false; //Spare for future use
		NV_Params.TestSetupNV[i].spareflag12 = false; //Spare for future use
		NV_Params.TestSetupNV[i].spareflag13 = false; //Spare for future use
		//--- end

	    //2016-06-23 fixture control - 20190425 which is now Advanced Test Configuration Options
	    for(int j=0; j<3; j++) {
	    	NV_Params.TestSetupNV[i].fixturecontrol.enabled[j] = false;
	    	NV_Params.TestSetupNV[i].fixturecontrol.fixtime[j] = 0;
	    	NV_Params.TestSetupNV[i].fixturecontrol.feedback[j] = false;
	    }
	    NV_Params.TestSetupNV[i].fixturecontrol.passmark = false;
	    NV_Params.TestSetupNV[i].fixturecontrol.passmarktime = 0;
	    NV_Params.TestSetupNV[i].fixturecontrol.failack = false;

	    NV_Params.TestSetupNV[i].fixturecontrol.fillToPres = false;
	    NV_Params.TestSetupNV[i].fixturecontrol.fillToPresTimer = 0;

		//lz Oct 2020
	    NV_Params.TestSetupNV[i].fixturecontrol.spareint20 = 0;
	    NV_Params.TestSetupNV[i].fixturecontrol.spareint21 = 0;
	    NV_Params.TestSetupNV[i].fixturecontrol.spareint22 = 0;
	    NV_Params.TestSetupNV[i].fixturecontrol.spareint23 = 0;
	    NV_Params.TestSetupNV[i].fixturecontrol.spareflag20 = false;
	    NV_Params.TestSetupNV[i].fixturecontrol.spareflag21 = false;
	    NV_Params.TestSetupNV[i].fixturecontrol.spareflag22 = false;
	    NV_Params.TestSetupNV[i].fixturecontrol.spareflag23 = false;
	    strcpy(NV_Params.TestSetupNV[i].fixturecontrol.sparestring20, "-");
	    //--- end
	}

	for(int i=0; i<4; i++) {
			NV_Params.CalDataNV[i].valueMax = 30000;
			NV_Params.CalDataNV[i].countMax = 14000;
			NV_Params.CalDataNV[i].valueMin = 0;
			NV_Params.CalDataNV[i].countMin = 6500;
			NV_Params.CalDataNV[i].gradient = 1.0;
			NV_Params.CalDataNV[i].offset = 0.0;
	}

	initNV_ParamsOptions();

	//copy initialised data into the test parameter structure used by testParams web pages
//	configParams.setTestParams(NV_Params.TestSetupNV[0]);  // Only working with 1 set of test parameters at the present
	selectedConfig=0;
	configParams.setTestParams(NV_Params.TestSetupNV[selectedConfig]);
	calibParams[0].setCalib(NV_Params.CalDataNV[0]);	//module A Differential Pressure
	calibParams[1].setCalib(NV_Params.CalDataNV[1]);	//module B Differential Pressure
	calibParams[2].setCalib(NV_Params.CalDataNV[2]);	//module A Test Pressure
	calibParams[3].setCalib(NV_Params.CalDataNV[3]);	//module B Test Pressure

//	printNV_Params();
}


//print utility
void printNV_Params() {
	for(int i=0; i<MAXCONFIGS; i++) {
		iprintf("NV_Params.TestSetupNV[%d]:[%s, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
										   "%d, %d, %d, %c, %d, %d, %d, %c, %d, %d, %c]\r\n",
			i,
			NV_Params.TestSetupNV[i].idString,
			NV_Params.TestSetupNV[i].testpressure,
			NV_Params.TestSetupNV[i].initdelay,
			NV_Params.TestSetupNV[i].ventoffdelay,
			NV_Params.TestSetupNV[i].filltime,
			NV_Params.TestSetupNV[i].stabilisationtime,
			NV_Params.TestSetupNV[i].isolationdelay,
			NV_Params.TestSetupNV[i].measuretime,
			NV_Params.TestSetupNV[i].evacuationtime,
			NV_Params.TestSetupNV[i].offsetcomp,
			NV_Params.TestSetupNV[i].alarmleakrate,
			NV_Params.TestSetupNV[i].alarmdiffp,
			NV_Params.TestSetupNV[i].spare1,
			NV_Params.TestSetupNV[i].fixtureOPstart, //0..FF 0d.p
			NV_Params.TestSetupNV[i].fixturedelaytime, //display to 3d.p
			NV_Params.TestSetupNV[i].checkinputs,
			NV_Params.TestSetupNV[i].enablecheckinputs?'T':'F',
			NV_Params.TestSetupNV[i].fixtureOPend, //0..FF 0d.p
			NV_Params.TestSetupNV[i].nexttestonpass,	//0..F
			NV_Params.TestSetupNV[i].nexttestonfailoralarm,	//0..F
			NV_Params.TestSetupNV[i].inhibitresults?'T':'F',
			NV_Params.TestSetupNV[i].spare2,
			NV_Params.TestSetupNV[i].spare3,
			NV_Params.TestSetupNV[i].spareflag1?'T':'F'
			);

		 //2016-06-23 fixture control
		iprintf("\tFixture Control[%d]: [", i);
	    for(int j=0; j<3; j++) {
	    	iprintf("(%c, %d, %c)",
	    		NV_Params.TestSetupNV[i].fixturecontrol.enabled[j]?'T':'F',
	    		NV_Params.TestSetupNV[i].fixturecontrol.fixtime[j],
	    		NV_Params.TestSetupNV[i].fixturecontrol.feedback[j]?'T':'F'
	   	    );
	    }
	    iprintf(", %c, %d, %c, %c, %d]\r\n",
	    		NV_Params.TestSetupNV[i].fixturecontrol.passmark?'T':'F',
	    		NV_Params.TestSetupNV[i].fixturecontrol.passmarktime,
	    		NV_Params.TestSetupNV[i].fixturecontrol.failack?'T':'F',

	    	    NV_Params.TestSetupNV[i].fixturecontrol.fillToPres?'T':'F',
	    	    NV_Params.TestSetupNV[i].fixturecontrol.fillToPresTimer
	    );
	}

	for(int i=0; i<4; i++) {
		printf("NV_Params.CalDataNV[%d]:[%d, %d, %d, %d, %.3f, %.3f]\r\n",
			i,
			NV_Params.CalDataNV[i].valueMax,
			NV_Params.CalDataNV[i].countMax,
			NV_Params.CalDataNV[i].valueMin,
			NV_Params.CalDataNV[i].countMin,
			NV_Params.CalDataNV[i].gradient,
			NV_Params.CalDataNV[i].offset
			);
	}

	printf("NV_Params.OptionsNV:[%d, %d, %d, %d, %d, %c, %c, %d, %.3f, %d, %.3f, %d, %.3f, %s, %s]\r\n",
		NV_Params.OptionsNV.logPort,
		NV_Params.OptionsNV.logLevel,
		NV_Params.OptionsNV.testpTol,
		NV_Params.OptionsNV.diffpAbort,
		NV_Params.OptionsNV.diffpNegAlarm,
		NV_Params.OptionsNV.LT_ID,
		NV_Params.OptionsNV.startenable?'T':'F',
		NV_Params.OptionsNV.option1,
		NV_Params.OptionsNV.option2,

		NV_Params.OptionsNV.option3,
		NV_Params.OptionsNV.option4,
		NV_Params.OptionsNV.option5,
		NV_Params.OptionsNV.option6,

		NV_Params.OptionsNV.password1,
		NV_Params.OptionsNV.password2
		);

	printf("NV_Params.OptionsNV.logIP:[");
	ShowIP(NV_Params.OptionsNV.logIP);
	printf("]\r\n");

}



