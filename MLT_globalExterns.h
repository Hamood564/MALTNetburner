/**
 * MLT_globalTypes.h
 * Declares external references to global variables and constants.
 * "MLT_globalDefs.h" should be included in main.cpp which is the actual declaration of the global symbols.
 * All other files needing to access these global symbols should include "MLT_globalExterns.h"
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT1.1
 * @date 2015-01-13
 */

#ifndef MLT_GLOBALEXTERNS_H_
#define MLT_GLOBALEXTERNS_H_

#include <basictypes.h>
#include "MLT_globalTypes.h"

extern char * AppName;

extern RunningValues dataIn; // dataInB;  //values for leak test A and leak test B

extern TestResults LT_Results, LT_ResTemp; /* LTB_Results, LTB_ResTemp;*/ 	//future: LTA_Results[10], LTB_Results[10]
extern TestResults ResultsFIFO[RESULTSFIFOSIZE]; //Used by FIFO results comms mechanism

//extern TestParams LTA_Params[4], LTB_Params[4] ; /*V5.6 not used*/

extern TestParams TestSetup; // Temporary legacy data record

extern NV_Params_Struct NV_Params;

extern tm dateTime;  // Date and Time structure

extern char             gLogBuf[120]; // General purpose buffer for use with logMessage
                                       // Functions making lots of logMessage calls often declare
                                       // their own buffer

extern volatile BYTE 	SolValves;
extern volatile BYTE 	IO_OPs;
extern volatile BYTE	DigIPs;
extern bool		ResetLocal;
extern bool		StartLocal;

extern char 	textInput[50]; //V2.8_20200305  BAE8 Barcode string Serial Input Task limits to 39 characters
extern bool		barCodeOk;

// Leak Test A
extern int		testConfigA;  // 26/01/2015 test program to use during testing
extern volatile unsigned short int stepCount;
extern int 		diffpZero;
extern int 		diffpOffset;
extern int		diffpCorrected;
extern int 		diffp;
extern BOOL 	diffpOverp;
extern int 		testp;
extern int 		testpstart;
extern float 	ResTPChange;

extern int      appliedConfig; //Test Parameters selected from the pull down list on index page

extern bool 	webStart;
extern bool 	webReset;
//extern bool 	webStartAB;


// Leak Test B
extern volatile unsigned short int stepCountB;
extern int 		diffpZeroB;
extern int 		diffpOffsetB;
extern int		diffpCorrectedB;
extern int 		diffpB;
extern BOOL 	diffpOverpB;
extern int 		testpB;
extern int 		testpstartB;
/*
extern bool 	webStartB;
extern bool 	webResetB;
*/

// Data Logging Flags
extern BOOL 	logFull;
extern BOOL 	logNow;
extern BOOL 	logNowPls;
extern int 		logCount;

//Calibration start/stop flags
extern bool		calibrationBusy[4]; //was 4

extern OS_FIFO	ResultsOSFIFO; //Used by FIFO results comms mechanism


#endif /* MLT_GLOBALEXTERNS_H_ */

