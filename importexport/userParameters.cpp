/*
 * userParameters.cpp
 *
 *  Created on: 13 May 2016
 *      Author: leszek.zarzycki
 */
#include <stdio.h>
#include <utils.h> //ShowIP()
#include <system.h> // Save/GetUserParameters
#include <syslog.h> //SysLogAddress

#include "MLT_globalExterns.h"

//#include "src/data/testParameters.h"
//#include "src/data/options.h"
//#include "src/maltunit/calibrationData.h"
#include "userParameters.h"
#include "ftp/FileSystemUtils.h"
#include "message_log.h"
//#include "web/statusline.h"
#include "configParams.h"


extern TestConfigParams configParams;
extern int selectedConfig; //first defined in formcode.cpp
//extern Options options;


// This key can be updated if you modify your structure. For example,
// you may need to do something special if you load a new version
// of s/w in a device that has the old structure values.
//MALT3_V1.0: 4D 33 31 30 <--M310
//#define VERIFY_KEY (0x4D333135)  // NV Settings key code

//about verify key...
//	C:\nburn\examples\StandardStack\SaveUserParameters
//	C:\nburn\examples\StandardStack\Web\FlashForm
/*
struct NV_Params_Struct {
	DWORD 			VerifyKey;
	TestParams		testSetup[MAXCONFIGS];
	CalibrationData	calibData[MAXCHANNELS];
	Options			options;
};
*/

bool exportreq = false;
bool importreq = false;
char path[128];
//static char * root[5] = {"ALL", "ALLTESTS", "TESTCONF", "OPTIONS", "CALIBRTN"};


//==============save / load settings===========================================
/* MALT3 code
void initDefaultSettings() {
	for (int i=0; i<MAXCONFIGS; i++) { initDefaultTestParams(i); }
	for (int i=0; i<MAXCHANNELS; i++) { setDefaultCalibrationData(i);}
	initDefaultOptions();
}
*/
/*print the Settings to standard io (ie using printf) */
/* MALT3 code
  void printSettings() {
	char buf[256];

	for (int i=0; i<MAXCONFIGS; i++) {
		printTestParams(buf, i);
		iprintf("config[%d]:%s\r\n", i, buf);
	}

	for (int i=0; i<MAXCHANNELS; i++) {
		printCalibrationData(buf, i);
		iprintf("calib[%d]:%s\r\n", i, buf);
	}
	printOptions(buf);
	iprintf("options:%s\r\n", buf);
}

void saveSettings(){
	NV_Params.VerifyKey = VERIFY_KEY;
	memcpy(&NV_Params.testSetup, &configs, sizeof(configs));
	memcpy(&NV_Params.calibData, &calib, sizeof(calib));
	memcpy(&NV_Params.options, &options, sizeof(Options));

	//sanity check
	//iprintf("Saving TestParams,size=%lu bytes\r\n", sizeof(NV_Params_Struct));
	//Do sanity check (above) before committing to this code.
	//Ensure you are not writing more than 128K (MOD54415)
	int b = 0;
	char buf[80];
	if ((b = SaveUserParameters(&NV_Params, sizeof(NV_Params_Struct))) != 0) {
//		iprintf("All Settings saved to MALT memory. %d bytes\r\n", b);
		sprintf(buf, "All Settings saved to MALT memory. %d bytes", b);
	} else {
//		iprintf("Save Settings failed. %d bytes written \r\n", b);
		sprintf(buf, "Save Settings failed. %d bytes written", b);
	}
	logMessage(LOGPRIOMED, buf);
	setWebMessage(buf);
}

//set system log preferences from Options settings - used in loadSettings()
void initLogPreferences() {
	SysLogAddress = getCommsIp(); // Set IP for UDP sys log debug data
	setSystemLogDestination(getLogDestination());
	setSystemLogPriority(getLogLevel()); //LOGPRIOMED); //LOW, MED, HIGH
}
*/
/* Retrieves the stored settings from MALT on-chip flash memory;
 * Restores default settings if (internal) verification key is not valid. This may
 * only happen on the first system boot up after a software update.*/
/* MALT3 code
void loadSettings(){
	NV_Params_Struct *pItem = (NV_Params_Struct*) GetUserParameters();
	iprintf("Retrieved Test Configuration Parameters: size=%lu bytes\r\n", sizeof(*pItem));

	if (pItem->VerifyKey != VERIFY_KEY) {
		iprintf("Invalid Verification Key - Initialising with default settings.\r\n");
		initDefaultSettings();	//Default initialisation
		initLogPreferences();
		saveSettings(); 		// to ensure data structure is saved
	}
	else {//copy to externs
		memcpy(&configs, &(pItem->testSetup), sizeof(configs));
		memcpy(&options, &(pItem->options), sizeof(Options));
		memcpy(&calib, &(pItem->calibData), sizeof(calib));
		initLogPreferences();
	}
	//printSettings();
}
*/
//==============Import / Export SD card===========================================
void setPath(char * filename) {
//	ioreq = req;
	sprintf(path, "PARAMS/%s", filename);
	iprintf("path=%s\r\n", path);
	logMessage(LOGPRIOLOW, path);
}

void setExportReq(char * filename) {
	exportreq=true;
	importreq=false; //cannot be both
	setPath(filename);
}

void setImportReq(char * filename) {
	importreq=true;
	exportreq=false; //cannot be both
	setPath(filename);
}

void exportToMemoryCard() {
	char buf[256];	//was 80
	exportreq=false;

	F_FILE* file = f_open(path, "w");
	if (!file) {
		sprintf(buf, "Failed to export: %s", path);
	}
	else {
		int n = f_write(&NV_Params, sizeof(NV_Params_Struct), 1, file);
		sprintf(buf, "Export user parameters. Wrote %d items of size: %lu",
				n, sizeof(NV_Params_Struct));
		f_close(file);
	}
	logMessage(LOGPRIOMED, buf);
	iprintf("%s",buf);
}

void importFromMemoryCard() {
	char buf[256];	//was 80
	importreq=false;

	F_FILE* file = f_open(path, "r");
	if (!file) {
		sprintf(buf, "Failed to import: %s", path);
		logMessage(LOGPRIOMED, buf);
	}
	else {
		int n = f_read(&NV_Params, sizeof(NV_Params_Struct), 1, file);
		sprintf(buf, "Import user parameters. Read %lu bytes", sizeof(NV_Params_Struct)*n);
		f_close(file);

		//copy test params to actual test setup
	//	TestSetup = NV_Params.TestSetupNV[0];  //TestSetup is the test parameters used in the actual test sequence
		TestSetup = NV_Params.TestSetupNV[selectedConfig];  //TestSetup is the test parameters used in the actual test sequence

		//copy retrieved data to test configuration web interface variables
	//	configParams.setTestParams(NV_Params.TestSetupNV[0]);  // Only working with 1 set of test parameters at the present
		configParams.setTestParams(NV_Params.TestSetupNV[selectedConfig]);

	}
	logMessage(LOGPRIOMED, buf);
	iprintf("%s",buf);
	//OSTimeDly(TICKS_PER_SECOND); //give time for io to complete before page is returned
}

void serviceMemoryCardReq(){
	if (exportreq) exportToMemoryCard();
	if (importreq) importFromMemoryCard();
}
