/**
 * MLT_globalDefs.h
 * Declares variables and constants with Global scope.
 * Only include in main.cpp.
 * 		All other files should use "MLT_globalExterns.h" which should contain the
 * 		same declarations with the extern prefix.
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT1.1
 * @date 2015-01-13 
 *
 * lz: 22014-11-27
 * Removed module B references
 * Renamed module A references without 'A' suffix
 */

#ifndef MLT_GLOBALDEFS_H_
#define MLT_GLOBALDEFS_H_

#include "MLT_globalTypes.h"

const char * AppName = "MALT4.02_TCP_V1.1 20250321";

RunningValues dataIn; //, dataInB;  //values for leak test A and leak test B

TestResults LT_Results, LT_ResTemp; /*, LTB_Results, LTB_ResTemp*/ 	//future: LTA_Results[10], LTB_Results[10]
TestResults ResultsFIFO[RESULTSFIFOSIZE]; //Used by FIFO results comms mechanism

TestParams TestSetup =
    {"Config 0", 5000, 100, 200, 2000, 5000, 100, 2000, 1000, 0, 0, 0,
    		  0, 0, 7,
    		  {{false,false,false}, {0,0,0}, {false,false,false}, false,0,false}, //2016-06-23
    		  false, 0, 0, 0, true, 0, 0, false}; // Temporary legacy record


NV_Params_Struct NV_Params;

tm dateTime;  // Date and Time structure
              // "time.h" is included in "MLT_globalTypes.h" which satisfies this reference
              // to the system date time structure type "tm".

char                gLogBuf[120]; // General purpose buffer for use with logMessage
                                       // Functions making lots of logMessage calls often declare
                                       // their own buffer

volatile BYTE 	SolValves;
volatile BYTE	IO_OPs;
volatile BYTE 	DigIPs;
bool			ResetLocal;
bool			StartLocal;

char 		textInput[50]; //V2.8_20200305  BAE8 Barcode string Serial Input Task limits to 39 characters
bool		barCodeOk;  //Has Barcode been read

// Leak Test A - lz:all these identifiers had a suffix 'A'
int			testConfigA;  // 26/01/2015 test program to use during testing
volatile unsigned short int stepCount;
int			diffpZero =0;
int 		diffpOffset =0;
int			diffpCorrected =0;
int 		diffp =0;
BOOL 		diffpOverp =0;
int 		testp =0;
int 		testpstart =0;
float 		ResTPChange;

int         appliedConfig = 0; //Test Parameters selected from the pull down list on index page

bool 		webStart;
bool 		webReset;
//bool 		webStartAB;

/* lz(15.01.2015)- test B variables kept as some are used in calibration calculations */
// Leak Test B
volatile unsigned short int stepCountB;
int 		diffpZeroB =0;
int 		diffpOffsetB =0;
int			diffpCorrectedB =0;
int 		diffpB =0;
BOOL 		diffpOverpB =0;
int 		testpB =0;
int 		testpstartB =0;

/*
bool 		webStartB;
bool 		webResetB;
*/
// Data Logging Flags
BOOL 		logFull = FALSE;
BOOL 		logNow = FALSE;
BOOL 		logNowPls = FALSE;
int 		logCount = 0;

//Calibration start/stop flags
bool		calibrationBusy[4];

OS_FIFO		ResultsOSFIFO; //Used by FIFO results comms mechanism

#endif /* MLT_GLOBALSDEFS_H_ */
