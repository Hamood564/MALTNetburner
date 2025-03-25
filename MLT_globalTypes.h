/**
 * MLT_globalTypes.h
 * Variable and Record Structure type definitions.
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT1.1
 * @date 2015-01-13
 *  
 * lz: 22014-11-27
 * Removed module B references
 */

#ifndef MLT_GLOBALTYPES_H_
#define MLT_GLOBALTYPES_H_

#include <nettypes.h>
#include <time.h>
#include <ucos.h>

#include "MLT_globalDefines.h"

/* System Date Time structure used from the C Library time.h
 * Structure repeated here just for ease of finding the documentation!
 struct tm
 * {
 *    int tm_sec;     // Second (0-59)
 *    int tm_min;     // Minute (0-59)
 *    int tm_hour;    // Hour (0-23)
 *    int tm_mday;    // Day of the month (1-31)
 *    int tm_mon;     // Month of the year [January(0)-December(11)]
 *    				  // NOTE! Add 1 to get normal 1 to 12 month number
 *    int tm_year;    // Years since 1900 (ie 2014 = 114)
 *    				  // NOTE! Add 1900 to get 4 digit year
 *    				  // Mod by 100 (remainder after / 100) to get 2 digit year
 *    int tm_wday;    // Day of the week [Sunday(0)-Saturday(6)]
 *    int tm_yday;    // Days since January 1st (0-365)
 *    				  // NOTE! Add 1 to get the Julian or Ordinal date
 *    int tm_isdst;   // Daylight Saving Time flag (in effect if greater than
 *                    // zero, not in effect if equal to zero, information not
 *                    // available if less than zero)
 * };
*/

// Structure for A/D Channels
struct ADArray {
  int   chan0;
  int   chan1;
  int   chan2;
  int   chan3;
  };

struct RunningValues {
	int		diffp; 			//differential pressure Pa (display to 1dp)
	int 	diffpCount; 	//A to D counts
	int 	testp;			//test pressure millibar (display to 0dp)
	int 	testpCount;		//A to D counts
};

//TestResults structure used to echo results to web browser, also for TCP and UDP output task
//Can contain results data or just a text string as defined by the msgID field
struct TestResults {
	void  * pUsedByFifo;//Used by FIFO mechanism. Don't modify this value, and keep it first
	bool	free;		// free = TRUE if structure is free to be used.  Used by FIFO post / pend
	int		msgID;		//indicate contents type of Result. 0 = Results data, 1 = message only
	char	LT_ID;		//Leak Test ID character (useful when there are multiple leak test instruments)
	int 	params; 	//test parameter program number 1..n
	int 	status;		//result of the test: 1=pass, 2=fail, etc, >10 = error
	int 	testp;		//test pressure at end of test
	int 	diffp;		//diff pressure at end of test
	int 	leakRate;	//calculated leak rate mm3/s (display to 2dp)
	int 	diffpAZ;	//auto zero differential pressure Pa (display to 1dp)
	char 	msg[80];	//text string buffer for use when the Result type is a message only
	tm 		dateTime;	//timestamp of results
};

struct FixtureControl {
	bool	enabled[3];
	int		fixtime[3];
	bool	feedback[3];
	bool	passmark;
	int		passmarktime;
	bool	failack;
	bool	fillToPres;
	int		fillToPresTimer;
    int		spareint20; //Spare for future use
    int		spareint21; //Spare for future use
    int		spareint22; //Spare for future use
    int		spareint23; //Spare for future use
    bool	spareflag20; //Spare for future use
    bool	spareflag21; //Spare for future use
    bool	spareflag22; //Spare for future use
    bool	spareflag23; //Spare for future use
	char 	sparestring20[30]; //Spare for future use
    //Spare parameters added so Parameter expansion will not cause problems
	//when existing systems are updated (hopefully).
};

struct TestParams {
//	String	idString;
	char	idString[40];
	int		testpressure;
	int		initdelay;
    int   	ventoffdelay;
    int		filltime;
    int		stabilisationtime;
    int		isolationdelay;
    int		measuretime;
    int		evacuationtime;
    int		offsetcomp;
    int		alarmleakrate;
    int		alarmdiffp;
    int 	spare1;
    int		fixtureOPstart; //0..FF 0d.p
    int		fixturedelaytime; //display to 3d.p
    FixtureControl fixturecontrol;
    int		checkinputs;
    bool	enablecheckinputs;
    int		fixtureOPend; //0..FF 0d.p
    int		nexttestonpass;	//0..F
    int		nexttestonfailoralarm;	//0..F
    bool	inhibitresults;
    int 	spare2; //Inter Test Delay display to 3d.p
    int 	spare3; //Isolate Pulse time at end of Fill if Test Pressure has changed
    bool 	spareflag1; //Use TestP for Leak Detection
    int		spareint10; //Spare for future use
    int		spareint11; //Spare for future use
    int		spareint12; //Spare for future use
    int		spareint13; //Spare for future use
    bool	spareflag10; //Spare for future use
    bool	spareflag11; //Spare for future use
    bool	spareflag12; //Spare for future use
    bool	spareflag13; //Spare for future use
    //Spare parameters added so Parameter expansion will not cause problems
    //when existing systems are updated (hopefully).
};

struct CalibrationData {
	int		valueMax;
	int 	countMax;
	int 	valueMin;
	int 	countMin;
	float	gradient;
	float	offset;
};

struct Options {
	int		logPort;			//Remote Port for TCP and UDP comms - Local port for UDP listener
	IPADDR	logIP;				//Default IP Address to log to (overridden by UDP Comms)
	int		logLevel;			//Logging message level to send
	int		testpTol;			//Percentage +/- tolerance for Test Pressure
	int		diffpAbort;			//Abort test at this diff pressure
	int		diffpNegAlarm;		//Alarm at this negative diffp
	char	LT_ID;				//Identifier for Leak Test A
//	char	LTB_ID;				//Identifier for Leak Test B
	bool	startenable;			//Fixture is ready at start of test sequence
	int 	option1;
	float	option2;
	int 	option3;
	float	option4;
	int 	option5;
	float	option6;
	char 	password1[25];		//Authentication to access configuration web pages
	char	password2[25];		//Authentication to access configuration web pages
	//add more as neccessary
};

#define MAXCONFIGS 16
struct NV_Params_Struct {
	TestParams       TestSetupNV[MAXCONFIGS];
	CalibrationData  CalDataNV[4];
	Options			 OptionsNV;
};

#endif /* MLT_GLOBALTYPES_H_ */
