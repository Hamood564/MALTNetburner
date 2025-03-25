/**
 * test_seq_defs.h
 * Variable and constant declarations used in "test_seq.cpp".
 * Separated to here and included in "test_seq.cpp" simply for clarity.
 * @author gavin.murray
 * @version Single_MALT1.1
 * @date 2015-01-13
 */

#ifndef TEST_SEQ_DEFS_H_
#define TEST_SEQ_DEFS_H_

#include "MLT_globalTypes.h"

// Test States are now defined in MLT_globalDefines so they can be used in other modules.
// It is included by MLT_globalTypes so they are effectively global in scope.
//#define RESETSTEP           0
//#define INITSTEP            1
//#define VENTOFFSTEP         2
//#define FILLSTEP            3
//#define STABILISESTEP       4
//#define ISOLATEDWELLSTEP    5
//#define MEASURESTEP         6
//#define PASSSTEP            7
//#define FAILSTEP            8
//#define FAULTSTEP           9
//#define EVACSTEP            10
//#define COMPLETESTEP        11
//#define DIFFPCAL1           12
//#define DIFFPCAL2           13
//#define TESTPCAL1           14
//#define TESTPCAL2           15
//

#define FILLON         (SolValves |= 0x0001)
#define FILLOFF        (SolValves &= 0x00fe)
#define VENTON         (SolValves |= 0x0002)
#define VENTOFF        (SolValves &= 0x00fd)
#define ISOLATEON      (SolValves |= 0x0004)
#define ISOLATEOFF     (SolValves &= 0x00fb)
//#define REFLEAKON      (SolValves |= 0x0008)
//#define REFLEAKOFF     (SolValves &= 0x00f7)
#define AUTOCONNON     (SolValves |= 0x0008)
#define AUTOCONNOFF    (SolValves &= 0x00f7)

/*
#define FILLONB        (SolValves |= 0x0010)
#define FILLOFFB       (SolValves &= 0x00ef)
#define VENTONB        (SolValves |= 0x0020)
#define VENTOFFB       (SolValves &= 0x00df)
#define ISOLATEONB     (SolValves |= 0x0040)
#define ISOLATEOFFB    (SolValves &= 0x00bf)
//#define REFLEAKONB     (SolValves |= 0x0080)
//#define REFLEAKOFFB    (SolValves &= 0x007f)
#define AUTOCONNONB    (SolValves |= 0x0080)
#define AUTOCONNOFFB   (SolValves &= 0x007f)
*/

#define PASSON        (IO_OPs |= 0x0001)
#define PASSOFF       (IO_OPs &= 0x00fe)
#define FAILON        (IO_OPs |= 0x0002)
#define FAILOFF       (IO_OPs &= 0x00fd)
#define DONEON        (IO_OPs |= 0x0004)
#define DONEOFF       (IO_OPs &= 0x00fb)
#define FAULTON       (IO_OPs |= 0x0008)
#define FAULTOFF      (IO_OPs &= 0x00f7)
#define FIX1ON        (IO_OPs |= 0x0080)
#define FIX1OFF       (IO_OPs &= 0x007f)
#define FIXAON        (IO_OPs |= 0x0080)
#define FIXAOFF       (IO_OPs &= 0x007f)
#define FIXBON        (IO_OPs |= 0x0040)
#define FIXBOFF       (IO_OPs &= 0x00bf)
#define FIXCON        (IO_OPs |= 0x0020)
#define FIXCOFF       (IO_OPs &= 0x00df)
#define MARKERON      (IO_OPs |= 0x0010)
#define MARKEROFF     (IO_OPs &= 0x00ef)

/*
#define PASSONB        (IO_OPs |= 0x0010)
#define PASSOFFB       (IO_OPs &= 0x00ef)
#define FAILONB        (IO_OPs |= 0x0020)
#define FAILOFFB       (IO_OPs &= 0x00df)
#define DONEONB        (IO_OPs |= 0x0040)
#define DONEOFFB       (IO_OPs &= 0x00bf)
#define FAULTOFFB      (IO_OPs |= 0x0080)
#define FAULTONB       (IO_OPs &= 0x007f)
*/

#define RESETLT		((DigIPs & 0x01) || ResetLocal)
#define STARTLT	    ((DigIPs & 0x02) || StartLocal)
#define READPARAMSA	(((DigIPs & 0x1C) >> 2) & 0x07)  //DEV_V2.2 change from 2 bits to 3 bits
#define READCHKIPS	(DigIPs & 0xF0)
#define STARTENSW	(DigIPs & 0x20)  //Start Enable Drawer Switch
#define FIXAFBIP	(DigIPs & 0x80)  //FixA feedback input
#define FIXBFBIP	(DigIPs & 0x40)  //FixB feedback input
#define FIXCFBIP	(DigIPs & 0x20)  //FixC feedback input - Can be used as Start Enable


//#define RESETLTB		(DigIPs & 0x10)
//#define STARTLTB		(DigIPs & 0x20)

#define IPDebounce			20  //Switch input debounce in ms BYTE Count so MAX 127
#define AV_SAMPLES          40   // number of a/d readings to average

//DSPI1 Bit Definitions
// MCR Register masks
#define MCR_MASTER_INIT     0x80070C01
#define MCR_HALT_BIT        0x00000001
#define MCR_DIS_TXF         0x00002000
#define MCR_DIS_RXF         0x00001000
#define MCR_CLR_FIFOS       0x00000C00


// CTAR Register masks
#define CTAR_CLOCK_POLARITY 0x04000000
#define CTAR_CLOCK_PHASE    0x02000000
#define CTAR_FRAME_16BIT    0x78000000
#define CTAR_FRAME_4BIT     0x18000000

// SR Register masks
#define SR_EOQF_MASK        0x10000000
#define SR_CLR_FLAGS        0x9A0A0000

// PUSHR (command) Register masks
#define PUSHR_CONT_BIT      0x80000000
#define PUSHR_EOQ_BIT       0x08000000


//Array of structures for individual A/D readings
ADArray   ADSamples[AV_SAMPLES];

//Structure of averaged A/D counts for each channel
volatile ADArray   ADAve;


#define LOG_SIZE    100000        // number of a/d readings to save 100s at 1ms sample rate
long 				logPitCount[LOG_SIZE];
int					logTestPA[LOG_SIZE];
int					logTestPB[LOG_SIZE];
int 				logDiffPPaA[LOG_SIZE];
int 				logDiffPPaB[LOG_SIZE];
//short unsigned int  logDiffPA[LOG_SIZE];
//short unsigned int  logDiffPAvA[LOG_SIZE];

int          TestPressure;
int          TestPressurePrev =0; //Save test pressure of previous test
int          InitTimer;
int          VentOffTimer;
int			 PresOverFillTimer;
bool		 FillToPresEn;
int          FillTimer;
int          StabTimer;
int          IsolatePlsTimer;
int          IsolateTimer;
int          MeasureTimer;
int          MeasureTimerInc;
int          EvacTimer;
int          OffComp;
int          AlarmLeakRate;
int          AlarmDiffP;
int          AlarmLeakRateTime;
int          DosingGLPLimit;
int			 FixOPStart;
int			 FixOPTimer;
int			 CheckIP;
bool		 EnCheckIP;
int			 FixOPEnd;
bool		 FixAEn;
bool		 FixBEn;
bool		 FixCEn;
int			 FixATime;
int			 FixBTime;
int			 FixCTime;
bool		 FixAFBack;
bool		 FixBFBack;
bool		 FixCFBack;
bool		 MarkerEn;
int			 MarkerTime;
bool		 FailAckEn;
int			 NextTestPass;
int			 NextTestFail;
bool		 DisResultsLamps;

int			 ChainTimer;
int          CalTimer;
bool		 evacPls;
int			 resIndex;
int          nextConfig; //Used for chained tests to store next test program

bool		 passFlag, failFlag, faultFlag;
// AGEN1 special - Flash Done lamp to signal ready to test after a delay at end of test
int          interTestTimer;
bool         flashDoneFlag;
bool         flashDoneStatus;
int          flashDoneOnTimer;
int          flashDoneOffTimer;

//Fixture Control
int FixATimer; //used for FixtureA step timer delays - used at both start and end of seq
int FixBTimer; //used for FixtureB step timer delays
int FixCTimer; //used for FixtureC step timer delays
//Enabled to Start flag
bool		 StartEnabled;
//Vacuum or Over Pressure Test
bool         vacTest;
//Use Test Pressure or Differential Pressure Transducer
bool		 useTestP;
/*
int          TestPressureB;
int          InitTimerB;
int          VentOffTimerB;
int          FillTimerB;
int          StabTimerB;
int          IsolateTimerB;
int          MeasureTimerB;
int          MeasureTimerIncB;
int          EvacTimerB;
int          OffCompB;
int          AlarmLeakRateB;
int          AlarmDiffPB;
int          AlarmLeakRateTimeB;
int          CalTimerB;
bool		 evacPlsB;
int			 resIndexB;
*/

//BYTE ledmem = 0;
int testp1 =0;

int			 udpMessageIndex; //FIFO array index for none result messages

// Semaphore used for Programmable Interrupt Timer
// PIT increments the semaphore
// testSeqTask pends on this semaphore and executes the Leak Test scan on each interrupt
OS_SEM PitSem1;



#endif /* TEST_SEQ_DEFS_H_ */
