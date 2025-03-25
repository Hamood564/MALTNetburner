/**
 * test_seq.cpp
 * Implementation of MALT engine.
 * @author gavin.murray
 * @version Single_MALT1.1
 * @date 2015-01-13
 */

// System includes
#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <sim.h>
//#include <..\MOD5441X\include\sim.h>
#include <pins.h>
#include <basictypes.h>
//#include <system.h>
#include <ucos.h>
#include <iosys.h>
#include <serial.h>
#include <cfinter.h>
#include <stdlib.h>
#include <string.h>
#include <pitr_sem.h>
//#include <mcf5441x_rtc.h>
//#include <ip.h>
#include <tcp.h>
#include <udp.h>


// MALT Project includes
#include "test_seq.h"
//#include "MLT_globalTypes.h"
#include "MLT_globalExterns.h"
#include "test_seq_defs.h"
#include "message_log.h"
#include "resultsComms.h"
#include "graph/graphData.h" //LZ nov 2019


/*-------------------------------------------------------------------
 Sends a message to the specified host.
 ------------------------------------------------------------------*/
void sendLog(int count)
{
    char buf[40];
    int    DestPort = 1284;
    int n;
	IPADDR DestIp;
	DestIp = AsciiToIp( "192.168.115.209" );
//	DestIp = AsciiToIp( "255.255.255.255" );  //UDP Broadcast

//    printf("Start Serial Log Output\r\n");
//	        for ( int i = 0; i < count; i++ )
//	        {
//	        	iprintf("%u\r\n", logDiffPA[i]);
//          	siprintf( buf, "%d, %d, %d, %d, %d\r\n", logPitCount[i], logTestPA[i], logTestPB[i], logDiffPPaA[i], logDiffPPaB[i]);
//	        }
//	        iprintf("%d\r\n", TimeTick);
//	        logFull = FALSE;
//    printf("End Serial Log Output\r\n");

	sprintf(buf, "Start TCP Log Output\r\n");
	logMessage(LOGPRIOLOW, buf);
	int fd = connect( DestIp, 0, DestPort, TICKS_PER_SECOND * 3);
    if (fd < 0)
    {
    	sprintf(buf, "Error: Connection to port %d failed\r\n", DestPort);
    	logMessage(LOGPRIOLOW, buf);
    }
    else
    {
        for ( int i = 0; i < count; i++ )
        {
        	siprintf( buf, "%ld, %d, %d, %d, %d\r\n", logPitCount[i], logTestPA[i], logTestPB[i], logDiffPPaA[i], logDiffPPaB[i]);
        	n = write( fd, buf, strlen(buf) );
        }
        logFull = FALSE;
        close( fd );
    }
    logFull = FALSE;
    sprintf(buf, "End TCP Log Output\r\n");
    logMessage(LOGPRIOLOW, buf);

//    sprintf(buf, "Start UDP Log Output\r\n");
//    logMessage(LOGPRIOLOW, buf);
//    // Create a UDP socket for sending/receiving (local port same as remote port)
//    // int CreateTxUdpSocket(IPADDR send_to_addr, WORD remote_port, WORD local_port).
//    int UdpFd = CreateTxUdpSocket( DestIp, DestPort, DestPort );
//    if ( UdpFd <= 0 )
//    {
//       sprintf(buf, "Error Creating UDP Socket: %d\r\n", UdpFd);
//       logMessage(LOGPRIOLOW, buf);
//    }
//    else
//    {
//        for ( int i = 0; i < count; i++ )
//        {
//            siprintf( buf, "%d, %d, %u, %u, %d\r\n", logPitCount[i], logTestPA[i], logTestPB[i], logDiffPPaA[i], logDiffPPaB[i]);
//            n = sendto( UdpFd, (BYTE *)buf, strlen(buf), DestIp, DestPort );
//        }
//        logFull = FALSE;
//        close( UdpFd );
//    }
//    logFull = FALSE;
//    sprintf(buf, "End UDP Log Output\r\n");
//    logMessage(LOGPRIOLOW, buf);

} // End of sendLog



/*-------------------------------------------------------------------
 * On the MOD-DEV-70, the LEDs are on J2 connector pins:
 * 15, 16, 31, 23, 37, 19, 20, 24 (in that order)
 * -----------------------------------------------------------------*/
void writeOutputs( BYTE SolMask, BYTE IO_Mask )
{
   static BOOL bOPGpioInit = FALSE;
   const BYTE SolPinNumber[8] = { 23, 19, 15, 16, 20, 24, 33, 34 }; //On board Sol Valve OPs
   const BYTE IOPinNumber[8]  = { 48, 45, 44, 43, 41, 37, 36, 31 }; //Off board IO OPs
   BYTE BitMask = 0x01;

   if ( ! bOPGpioInit ) // Needs Initialisation - first call
   {
      for ( int i = 0; i < 8; i++ )
      {
         J2[SolPinNumber[i]].function( PIN_GPIO );
         J2[IOPinNumber[i]].function( PIN_GPIO );
      }
      bOPGpioInit = TRUE;
   }

   for ( int i = 0; i < 8; i++ )
   {
//Output to on-board Outputs (usually leak test module sol valves)
      if ( (SolMask & BitMask) == 0 )
         J2[SolPinNumber[i]] = 1;  // Sol Valve opto-isolators tied to 3.3V, so 1 = off
      else
         J2[SolPinNumber[i]] = 0;
//Output to off-board Outputs
      if ( (IO_Mask & BitMask) == 0 )
         J2[IOPinNumber[i]] = 1;  // IO opto-isolators tied to 3.3V, so 1 = off
      else
         J2[IOPinNumber[i]] = 0;

      BitMask <<= 1;
   }
} // End of writeOutputs


/*-------------------------------------------------------------------
 * On the MOD-DEV-70, the switches are on J2 connector pins:
 * 8, 6, 7, 10, 9, 11, 12, 13 (in that order). These signals are
 * Analog to Digital, not GPIO, so we read the analog value and
 * determine the switch postion from it.
 * ------------------------------------------------------------------*/
BYTE readInputs()
{
   static BOOL bIPGpioInit = FALSE;
   static BYTE RDbncOnCnt = 0;
   static BYTE RDbncOffCnt = 0;
   static BYTE SDbncOnCnt = 0;
   static BYTE SDbncOffCnt = 0;
   const BYTE PinIns[8] = { 32, 26, 31, 13, 10, 9, 7, 6 }; //Dig IPs
   BYTE BitMask = 0;

   if ( ! bIPGpioInit )  // Needs Initialisation - first call
   {
      for ( int i = 0; i < 2; i++ )
         J2[PinIns[i]].function( PIN_GPIO );
      for ( int i = 2; i < 8; i++ )
         J1[PinIns[i]].function( PIN_GPIO );
	  J2[39].function(PINJ2_39_GPIO); //I2C 0 – Serial Data Used for Local Reset I/P or Debug O/P
	  J2[42].function(PINJ2_42_GPIO); //I2C 0 – Serial Clock Used for Local Start I/P or Debug O/P
      bIPGpioInit = TRUE;
   }

   for ( int BitPos = 0x01, i = 0; BitPos < 256; BitPos <<= 1, i++ )
   {
       if (i < 2)
       {
           if ( ! J2[PinIns[i]]) BitMask |= (BYTE)(0xFF & BitPos);
       }
       else
       {
    	   if ( ! J1[PinIns[i]]) BitMask |= (BYTE)(0xFF & BitPos);
       }
   }
//Debounce Local Reset Input
   if (!J2[39]) {
	   RDbncOnCnt++;
	   if (RDbncOnCnt >= IPDebounce) {
		   RDbncOnCnt = IPDebounce;
		   RDbncOffCnt = 0;
		   ResetLocal = true;
	   }
   }
   else {
	   RDbncOffCnt++;
	   if (RDbncOffCnt >= IPDebounce) {
		   RDbncOffCnt = IPDebounce;
		   RDbncOnCnt = 0;
		   ResetLocal = false;
	   }
   }
//Debounce Local Start Input
   if (!J2[42]) {
	   SDbncOnCnt++;
	   if (SDbncOnCnt >= IPDebounce) {
		   SDbncOnCnt = IPDebounce;
		   SDbncOffCnt = 0;
		   StartLocal = true;
	   }
   }
   else {
	   SDbncOffCnt++;
	   if (SDbncOffCnt >= IPDebounce) {
		   SDbncOffCnt = IPDebounce;
		   SDbncOnCnt = 0;
		   StartLocal = false;
	   }
   }
   return BitMask;
} // End of readInputs



/*-------------------------------------------------------------------
 * Print the time to stdout, which is the debug serial port by
 * default.
 * ----------------------------------------------------------------*/
void printTimeStruct(struct tm &bt )
{
	const char *dow_str[] =
	{
	  "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT", "\0"
	};

   iprintf( "%d/%d/%d day: %d (%s) %02d:%02d:%02d",
            bt.tm_mday,
            bt.tm_mon+1,
            bt.tm_year+1900,
            bt.tm_yday,
            dow_str[bt.tm_wday],
            bt.tm_hour,
            bt.tm_min,
            bt.tm_sec );
} // End of printTimeStruct




/*
 * Test Sequence Software Initialisation
 * Call once on power on
 */
void initTestSoftware()
{
// initialise leak test sequence - *** DO THIS IN test_seq
//    SolValves = 0;
//    IO_OPs = 0x88;
//Both Alarm outputs on (no Alarm)
    FAULTOFF;
//    FAULTOFFB;
    stepCount = RESETSTEP;
    stepCountB = RESETSTEP;
    vacTest = false; //Default to over pressure test
    useTestP = false; //Default to using Differential Pressure Transducer

	//Flush Averaging buffer to A/D zero on start
	    for (int i = 0; i < AV_SAMPLES; i++)
	    {
	      ADSamples[i].chan0=33210;
	      ADSamples[i].chan1=33210;
	      ADSamples[i].chan2=33210;
	      ADSamples[i].chan3=33210;
	    }
 	   flashDoneOnTimer = 700;  //Init to sensible values
 	   flashDoneOffTimer = 300;
 	   flashDoneFlag = false;
 	strcpy(textInput, "None"); //V2.8_20200305  BAE8 - Init Serial Data Input
 	barCodeOk = false;
} // End of initTestSoftware


/*
 * Test Sequence Hardware Initialisation
 * Call once on power on
 */
void initHardware()
{
	// Initialise the DSPI
	// Set DSPI1 pins for SPI function
	    J2[25].function(PINJ2_25_DSPI1_SCK);
	    J2[27].function(PINJ2_27_DSPI1_SIN);
	    J2[28].function(PINJ2_28_DSPI1_SOUT);
	    J2[30].function(PINJ2_30_DSPI1_PCS0);
	    J2[35].function(PINJ2_35_DSPI1_PCS1);
	    J2[40].function(PINJ2_40_DSPI1_PCS2);

	    J2[39].function(PINJ2_39_GPIO); //I2C 0 – Serial Data Used for Local Reset I/P or Debug O/P
	    J2[42].function(PINJ2_42_GPIO); //I2C 0 – Serial Clock Used for Local Start I/P or Debug O/P

//Init the D/A for E to P control
	    sim1.ccm.dactsr = 0x0000; // DAC trigger select register in CCM - should be the default
	    sim2.adc.cal = 0x0003; // Source of the ADCA3 pin as DAC0 output and ADCA7 pin as DAC1 output.
	    sim2.dac[0].data = 0;  // Init DAC0 data value to 0
	    sim2.dac[1].data = 0;  // Init DAC1 data value to 0

	    sim1.ccm.misccr2 &= ~(0x000C);   // Disable ADC3 on J2.9 and ADC7 on J2.13
	    sim1.ccm.misccr2 |= 0x0060;     // Enable DAC0 output J2.9 and DAC1 output on J2.13

	    sim2.dac[1].cr = 0x1140;  //Low Speed, Normal Mode and Power On
	    sim2.dac[1].data = 0;
	    sim2.dac[0].cr = 0x1140;  //Low Speed, Normal Mode and Power On
	    sim2.dac[0].data = 0;

	// SDHC slewrate is bits [1:0], others unused
	// Default slew rate for DSPI1 is Lowest
	// Bits 1:0 set 00=Lowest, 01=Low, 10=High, 11=Highest
	// NB use Lowest (default) for DSPI clock of <4MHz, Low for clock of > 4MHz and High for clock of > 20MHz
	// So to set bits 1:0 to 01=Low
	    sim1.gpio.srcr_sdhc     &= ~0x03;  // Mask bits 1:0 to zero
	    sim1.gpio.srcr_sdhc     |= 0x01;  // Set bit 0

	// SPI Master using CS0, CS1, CS2 Inactive state Hi
	// Clear TX and RX FIFO and Halt
	    sim2.dspi1.mcr = MCR_MASTER_INIT;
	// 16 bit, 0=Clk inactive low, 0=Pol Cap Leading edge
	// PCSSCK CS to 1st Clk 1.02us
	// PASC   Last Clk to CS 1.02us
	// PDT    CS off to next CS on 4.096us
	// Baud Rate 3.9MHz
	    sim2.dspi1.ctar[0] = 0x78006684;
	// CLear flags
	    sim2.dspi1.sr = SR_CLR_FLAGS;
	// Push commands into the Tx FIFO to read the A/D
	    sim2.dspi1.pushr = 0x04018400;  //CH0 - Gnd, Single Unipolar
	    sim2.dspi1.pushr = 0x0001C400;  //Read CH0 Select CH1 next - Gnd, Single Unipolar
	    sim2.dspi1.pushr = 0x00019400;  //Read CH1 Select CH2 next - Gnd, Single Unipolar
	    sim2.dspi1.pushr = 0x0001D400;  //Read CH2 Select CH3 next - Gnd, Single Unipolar
	    sim2.dspi1.pushr = 0x08018400;  //Read CH3 Select CH0 next - Gnd, Single Unipolar (End of list)

	//    sims.dspi1.mcr |= MCR_HALT_BIT;
	//    sim2.dspi1.sr |= SR_EOQF_MASK;  //Clear End of Queue flag  - could clear all status flags?

	// Start the SPI processing
	    sim2.dspi1.mcr &= ~MCR_HALT_BIT;
} // End of initTestHardware



void readAnalogue()
{
	  int avsum0, avsum1, avsum2, avsum3, j, divisor;
	  static unsigned short int avcount;

	// Assume all A/D transfers are complete
	  sim2.dspi1.mcr |= MCR_HALT_BIT;  // Make sure Halt is set
	// Now read A/D data
	// Save in an array for averaging
	  avcount++;
	  if (avcount >= AV_SAMPLES) avcount = 0;
	  ADSamples[avcount].chan0 = sim2.dspi1.popr;  //Discard first A/D channel - sample hold has been waiting too long
	  ADSamples[avcount].chan0 = sim2.dspi1.popr;  // LTA Diff Pressure actually Chan 0
	  ADSamples[avcount].chan1 = sim2.dspi1.popr;  // LTB Diff Pressure actually Chan 2
	  ADSamples[avcount].chan3 = sim2.dspi1.popr;  // LTB Test Pressure actually Chan 4
	  ADSamples[avcount].chan2 = sim2.dspi1.popr;  // LTA Test Pressure actually Chan 5

	// Now start the DSPI again to read the next A/D samples
	// SPI Master using CS0, CS1, CS2 Inactive state Hi
	// Clear TX and RX FIFO and Halt
	   sim2.dspi1.mcr = MCR_MASTER_INIT;
	// 16 bit, 0=Clk inactive low, 0=Pol Cap Leading edge
	// PCSSCK CS to 1st Clk 1.02us
	// PASC   Last Clk to CS 1.02us
	// PDT    CS off to next CS on 4.096us
	// Baud Rate 3.9MHz
	   sim2.dspi1.ctar[0] = 0x78006684;
	// CLear flags
	   sim2.dspi1.sr = SR_CLR_FLAGS;
	// Push commands into the Tx FIFO to read the A/D
	   sim2.dspi1.pushr = 0x04018400;  //Select CH0 - Gnd next, Single Unipolar
	   sim2.dspi1.pushr = 0x00019400;  //Read CH0 Select CH2 - Gnd next, Single Unipolar
	//   sim2.dspi1.pushr = 0x04010000;  //Select CH0 - CH1 next, Differential Bipolar
	//   sim2.dspi1.pushr = 0x00011000;  //Read CH0 Select CH2 - CH3 next, Differential Bipolar
	   sim2.dspi1.pushr = 0x0001A400;  //Read CH2 Select CH4 - Gnd next, Single Unipolar
	   sim2.dspi1.pushr = 0x0001E400;  //Read CH4 Select CH5 - Gnd next, Single Unipolar
	   sim2.dspi1.pushr = 0x08018400;  //Read CH5 Select CH0 - Gnd next, Single Unipolar

	//   sims.dspi1.mcr |= MCR_HALT_BIT;  //its already in Halt
	//   sim2.dspi1.sr |= SR_EOQF_MASK;  //Clear End of Queue flag  - have already cleared all status flags
	    sim2.dspi1.mcr &= ~MCR_HALT_BIT;  // Start the SPI

	// Debug prints only
	//    iprintf("Chan0 Read %X\r\n", A_D[0]);
	//    iprintf("Chan1 Read %X\r\n", A_D[1]);
	//    iprintf("Chan2 Read %X\r\n", A_D[2]);
	//    iprintf("Chan3 Read %X\r\n", A_D[3]);
	//    iprintf("TX FIFO 0  %X\r\n", sim2.dspi1.txfr[0]);
	//    iprintf("TX FIFO 1  %X\r\n", sim2.dspi1.txfr[1]);
	//    iprintf("TX FIFO 2  %X\r\n", sim2.dspi1.txfr[2]);
	//    iprintf("TX FIFO 3  %X\r\n", sim2.dspi1.txfr[3]);
	//    iprintf("TX FIFO 4  %X\r\n", sim2.dspi1.txfr[4]);
	//    iprintf("TX FIFO 5  %X\r\n", sim2.dspi1.txfr[5]);

	// Calculate averages
	  avsum0=0; avsum1=0; avsum2=0; avsum3=0;
//	  int chan0jMax = 0; int chan0jMin = 0; int chan0Max = 0; int chan0Min = 65535;
	  for (j=0; j < AV_SAMPLES; j++)
	  {
	//    if (ADSamples[j].chan0 > chan0Max) {chan0jMax = j; chan0Max = ADSamples[j].chan0;}
	//    if (ADSamples[j].chan0 < chan0Min) {chan0jMin = j; chan0Min = ADSamples[j].chan0;}
		avsum0 += ADSamples[j].chan0;
	    avsum1 += ADSamples[j].chan1;
	    avsum2 += ADSamples[j].chan2;
	    avsum3 += ADSamples[j].chan3;
	  }
	//  avsum0 = avsum0 - ADSamples[chan0jMax].chan0;
	//  avsum0 = avsum0 - ADSamples[chan0jMin].chan0;
	  ADAve.chan0 = avsum0/AV_SAMPLES;
	  ADAve.chan1 = avsum1/AV_SAMPLES;
	  ADAve.chan2 = avsum2/AV_SAMPLES;
	  ADAve.chan3 = avsum3/AV_SAMPLES;

	//  Now average AV_SAMPLES averages again - Filtering experiment
	//  avcount2++;
	//  if (avcount2 >= 200) avcount2 = 0;
	//  ADSamples2[avcount2].chan0 = ADAve.chan0;
	//  avsum0=0;
	//  for (j=0;j<200;j++)
	//    {
	//  	avsum0 += ADSamples2[j].chan0;
	//    }
	//  ADAve.chan0 = avsum0/(200);
// Calibration
/* 20140703 Back to original SCX +-25mb Diff TRansducer but with the VREF driven by an external buffer amp*/
//	     diffpA = ((ADAve.chan0 - 33900 ) * 6300 ) / ( 43100 - 33900 );  //With Vref Ref
//	     testpA = ((ADAve.chan2 - 13478 ) * 29300 ) / ( 28702 - 13478 );  //Sunix
// module A calibration
// Vac Mod ( negate if vacTest)
	     divisor = NV_Params.CalDataNV[0].countMax - NV_Params.CalDataNV[0].countMin;
	     if (divisor == 0) divisor = 1;
	     diffp = ((ADAve.chan0 - NV_Params.CalDataNV[0].countMin ) * (NV_Params.CalDataNV[0].valueMax - NV_Params.CalDataNV[0].valueMin)) / divisor;  //With Vref Ref
//Remove negate of diffp.  Now handled in the diffpCorrected calc.
//	     if (vacTest) diffp = -diffp; //negate if a vacuum test - leak will be increasing pressure so keep diffp positive for a leak
	     divisor = NV_Params.CalDataNV[2].countMax - NV_Params.CalDataNV[2].countMin;
	     if (divisor == 0) divisor = 1;
	     testp = ((ADAve.chan2 - NV_Params.CalDataNV[2].countMin ) * (NV_Params.CalDataNV[2].valueMax - NV_Params.CalDataNV[2].valueMin)) / divisor;  //Sunix
// If Options set to use TestP transducer to measure Leak then use TestP as DiffP reading
	     if (useTestP) diffp = testp;
	     testp=5000; //LZ testing purposes
// module B calibration
	     divisor = NV_Params.CalDataNV[1].countMax - NV_Params.CalDataNV[1].countMin;
	     if (divisor == 0) divisor = 1;
	     diffpB = ((ADAve.chan1 - NV_Params.CalDataNV[1].countMin ) * (NV_Params.CalDataNV[1].valueMax - NV_Params.CalDataNV[1].valueMin)) / divisor;  //With Vref Ref
	     divisor = NV_Params.CalDataNV[3].countMax - NV_Params.CalDataNV[3].countMin;
	     if (divisor == 0) divisor = 1;
	     testpB = ((ADAve.chan3 - NV_Params.CalDataNV[3].countMin ) * (NV_Params.CalDataNV[3].valueMax - NV_Params.CalDataNV[3].valueMin)) / divisor;  //Sunix

// Data Logging for diagnostic use only
// Use TCP capture program on a connected PC to save to csv file
	     if (logNow)
	     {
           if (! logNowPls) logCount = 0;  // Detect logNow rising edge and log to buffer start
	 	   logPitCount[logCount] = logCount; //1ms count
 	 	   logTestPA[logCount] = testp /10; // Test Pressure A
// 	 	   logTestPB[logCount] = testpB /10; // Test Pressure B
//	 	   logDiffPA[logCount] = ADSamples[avcount].chan0; // Raw DiffP A/D counts
//	 	   logDiffPAvA[logCount] = ADAve.chan0; //Averaged DiffP A/D counts
	 	   logDiffPPaA[logCount] = diffp; //DiffP B in Pa (1/10th of Pa)
//	 	   logDiffPPaB[logCount] = diffpB; //DiffP B in Pa (1/10th of Pa)
	   	   logCount++;
	   	   if (logCount >= LOG_SIZE) logCount = 0; // Circular buffer
	     }
	     logNowPls = logNow; // Rising Edge detection
	     diffp=200; //LZ testing purposes
//  Set Reset differential Over Pressure flag - but Only if using the DiffP Transducer
	    if ((abs(diffp) > NV_Params.OptionsNV.diffpAbort) && (!useTestP))
	    { diffpOverp = true;
	    } else
	    { diffpOverp = false;
	    }
/*
	    if (abs(diffpB) > NV_Params.OptionsNV.diffpAbort)
	    { diffpOverpB = true;
	    } else
	    { diffpOverpB = false;
	    }
*/
} // End of readAnalogue


//Calculate Corrected Pressure Change caused by a Leak
int calcCorrectedDiffp()
{
	int correctedValue;

	if ((!vacTest && !useTestP) || (vacTest && useTestP))
		correctedValue = (diffp - diffpZero - diffpOffset);
	else
		correctedValue = (diffpZero - diffp - diffpOffset);

	return 60; //correctedValue; //60 LZ testing purposes
}



void leakTestSeq()
{
	  unsigned int DisplayValue;
	  char buf[80];
	  int tempIO; //used for temp bit shifts
	  int tempEtoP; //used for D/A E to P value

	  DisplayValue = stepCount * 1000;

	  switch ( stepCount )
	  {
	     case RESETSTEP:
//         SolValves = 0;
	       FILLOFF;
	       VENTOFF;
	       ISOLATEOFF;
	       AUTOCONNOFF;
//BGB1 - Open Fixture
//	       FIX1OFF;
	       if ( ! flashDoneFlag) {
	    	   DONEON;
	       }
	       if (flashDoneFlag && flashDoneStatus && (flashDoneOnTimer <= 0)) {
	    	   flashDoneStatus = false;
	    	   flashDoneOnTimer = 700;
	    	   flashDoneOffTimer = 300;
	    	   DONEOFF;
	       }
	       if (flashDoneFlag && (! flashDoneStatus) && (flashDoneOffTimer <= 0)) {
	    	   flashDoneStatus = true;
	    	   flashDoneOnTimer = 700;
	    	   flashDoneOffTimer = 300;
	    	   DONEON;
	       }
	       if (flashDoneFlag) {
		       flashDoneOnTimer--;
		       flashDoneOffTimer--;
	       }
	       testConfigA = appliedConfig; //lz nov 2019 0; //Make sure a valid test parameter config number is set
	       CalTimer = 500; //Initialise ready for use during Calibration
	       diffpZero = 0;
	       diffpOffset = 0;
	       evacPls = false; //Used to detect first scan on evac step
	       nextConfig = 0; //Init to a valid config
//Debug only - to start a test sequence after a 3 sec delay
//         OSTimeDly(70);
//	       stepCountA = INITSTEP;
//BGB1 - Check If Enabled to Start and Web Start or DigIP Start signal
//V2.8_20200305  BAE8 - If Option 5 is > 0 then a minimum number of characters is required as a Part ID before Test Can Start
	       StartEnabled = ( ((!NV_Params.OptionsNV.startenable) ||  (NV_Params.OptionsNV.startenable && STARTENSW))
	    		         && ((NV_Params.OptionsNV.option5 <= 0) || ((NV_Params.OptionsNV.option5 > 0) && barCodeOk)) );
	       if ((STARTLT && StartEnabled) || (webStart && StartEnabled)) // || webStartAB)
	       {
		       sprintf(buf, " Start Leak Test A\r\n");
		       logMessage(LOGPRIOMED, buf);
	    	   stepCount = GETTESTPARAMSSTEP; // Start input
	    	   webStart = false; // reset flags so they don't auto repeat
//	    	   webStartAB = false; // don't reset here otherwise LTB won't start! Itll be reset at the end of test
	    	   webReset = false; // reset Reset flag incase it's been clicked while waiting to start
		       MeasureTimerInc = 0;
		       LT_ResTemp.free = false;
		       LT_ResTemp.status = 0; //No result yet
		       LT_ResTemp.msgID = 0; //Structure contains results data
		       LT_ResTemp.LT_ID = NV_Params.OptionsNV.LT_ID; //Set LT A identifier character
		       LT_ResTemp.params = 1; //Test Parameter config (changed later when known)
		       LT_ResTemp.testp = 0;
		       LT_ResTemp.diffp = 0;
		       LT_ResTemp.diffpAZ = 0;
		       LT_ResTemp.leakRate = 0;
		       LT_ResTemp.dateTime = dateTime; //Set Date and Time at start of test
		       strcpy(LT_ResTemp.msg, textInput);
		       PASSOFF;
		       FAILOFF;
		       FAULTOFF;
		       DONEOFF;
// HRC1 20160218 Use IO Dig O/P 4,5,6,7 as Fixture Pass Indicators so switch ALL Off at Start of Test
//		       IO_OPs &= 0x000f;  // BGB1 Remove
		       passFlag = false; //Needed for chained tests
		       failFlag = false;
		       faultFlag = false;
//// Send start message to listening PC via UDP
//			   udpMessageIndex = findFreeResultsFifoStruct();
//			   if (udpMessageIndex >= 0)
//			   {
//				   ResultsFIFO[udpMessageIndex].msgID = 10; // msgID 10 = Start of Test
//				   ResultsFIFO[udpMessageIndex].LT_ID = NV_Params.OptionsNV.LT_ID; //Set LT A identifier character
//				   ResultsFIFO[udpMessageIndex].params = 1; //Test Parameter set (only 1 available at the present)
//				   ResultsFIFO[udpMessageIndex].status = 0; //ms Tick count 0
//				   ResultsFIFO[udpMessageIndex].testp = TestPressure; // Expected Test pressure - use for graph scaling
//				   ResultsFIFO[udpMessageIndex].diffp = AlarmDiffP; //DiffP fail limit - use for graph scaling
//				   ResultsFIFO[udpMessageIndex].free = false;  //Make sure this element is marked as in use
//// Post the message in the Fifo.  This is then handled in the resultsCommsTask
//				   OSFifoPost( &ResultsOSFIFO, ( OS_FIFO_EL * ) &ResultsFIFO[udpMessageIndex] );
//			   }
			   gPitCount[1] = 0; //Zero timer 1ms count, used as log data time base
		       //startGraphData(); // ---LZ Sep 2020 - moved to VENTOFFSTEP
	       }
//BGB1 - Make sure webStart isn't latched if the Drawer Switch is disabling at start
	       if ( webStart ) {webStart = false;} // reset flag so it doesn't latch the web start and go when drawer closed
	       break;

	     case GETTESTPARAMSSTEP:
	       tempIO = READPARAMSA;  //Read digital inputs - see test_seq_defs.h
//If DigIPs are zero select the test parameters from the index page pull down list
//If DigIPs are non-zero use them to select the test parameters
	       if (tempIO == 0)
	       {
	    	   testConfigA = appliedConfig; //select the test parameters from the index page list
	       }
	       else
	       {
	    	   testConfigA = tempIO; //select the test parameters from the index page list
	       }
	       stepCount = LOADPARAMSSTEP;
//           sprintf(buf, " Init A Done\r\n");
//          logMessage(LOGPRIOMED, buf);
	       break;

	     case LOADPARAMSSTEP:
//If out of range default to test zero
		       if ((testConfigA < 0) || (testConfigA > 15))
		       {
		    	   testConfigA = 0;  //Default to Test Parameters 0 if out of range
		       }

//	    	   testConfigA = 0;  //Default to Test Parameters 0 if out of range
		       PASSOFF; //Switch OPs off here in case they are not inhibited during chained tests
		       FAILOFF;
		       FAULTOFF;
		       DONEOFF;
		       LT_ResTemp.params = testConfigA; //Test Parameter config
		       LT_ResTemp.free = false;
		       LT_ResTemp.status = 0; //No result yet
		       LT_ResTemp.msgID = 0; //Structure contains results data
		       LT_ResTemp.LT_ID = NV_Params.OptionsNV.LT_ID; //Set LT A identifier character
		       LT_ResTemp.testp = 0;
		       LT_ResTemp.diffp = 0;
		       LT_ResTemp.diffpAZ = 0;
		       LT_ResTemp.leakRate = 0;
		       LT_ResTemp.dateTime = dateTime; //Set Date and Time at start of test - updated at end of test
//Save local copy of test parameters for use during the test cycle so changes only take effect at the start of the next test
		       TestPressure = NV_Params.TestSetupNV[testConfigA].testpressure;
		       if (TestPressure < 0) vacTest = true; else vacTest = false; // Vac or Over Pressure Test
		       useTestP = NV_Params.TestSetupNV[testConfigA].spareflag1; //TestP (if true) or DiffP (if false) Transducer
		       InitTimer = 100;
		       VentOffTimer = NV_Params.TestSetupNV[testConfigA].ventoffdelay;
		       PresOverFillTimer = NV_Params.TestSetupNV[testConfigA].fixturecontrol.fillToPresTimer;;
		       FillToPresEn = NV_Params.TestSetupNV[testConfigA].fixturecontrol.fillToPres;
		       FillTimer = NV_Params.TestSetupNV[testConfigA].filltime;
		       StabTimer = NV_Params.TestSetupNV[testConfigA].stabilisationtime;
		       IsolateTimer = NV_Params.TestSetupNV[testConfigA].isolationdelay;
		       MeasureTimer = NV_Params.TestSetupNV[testConfigA].measuretime;
		       EvacTimer = NV_Params.TestSetupNV[testConfigA].evacuationtime;
		       OffComp = NV_Params.TestSetupNV[testConfigA].offsetcomp;
		       AlarmLeakRate = NV_Params.TestSetupNV[testConfigA].alarmleakrate;
		       AlarmDiffP = NV_Params.TestSetupNV[testConfigA].alarmdiffp;
		       AlarmLeakRateTime = NV_Params.TestSetupNV[testConfigA].measuretime;
		       DosingGLPLimit = NV_Params.TestSetupNV[testConfigA].spare1; //Dosing Gross Leak Pressure Limit
		       FixOPStart = NV_Params.TestSetupNV[testConfigA].fixtureOPstart;
		       FixOPTimer = NV_Params.TestSetupNV[testConfigA].fixturedelaytime;
		       CheckIP = NV_Params.TestSetupNV[testConfigA].checkinputs & 0xF0; // 0 to 0xF0 ie MS nibble only
		       EnCheckIP = NV_Params.TestSetupNV[testConfigA].enablecheckinputs;
		       FixOPEnd = NV_Params.TestSetupNV[testConfigA].fixtureOPend;
		       FixAEn = NV_Params.TestSetupNV[testConfigA].fixturecontrol.enabled[0];
		       FixBEn = NV_Params.TestSetupNV[testConfigA].fixturecontrol.enabled[1];
		       FixCEn = NV_Params.TestSetupNV[testConfigA].fixturecontrol.enabled[2];
		       FixATime = NV_Params.TestSetupNV[testConfigA].fixturecontrol.fixtime[0];
		       FixBTime = NV_Params.TestSetupNV[testConfigA].fixturecontrol.fixtime[1];
		       FixCTime = NV_Params.TestSetupNV[testConfigA].fixturecontrol.fixtime[2];
		       FixAFBack = NV_Params.TestSetupNV[testConfigA].fixturecontrol.feedback[0];
		       FixBFBack = NV_Params.TestSetupNV[testConfigA].fixturecontrol.feedback[1];
		       FixCFBack = NV_Params.TestSetupNV[testConfigA].fixturecontrol.feedback[2];
		       MarkerEn = NV_Params.TestSetupNV[testConfigA].fixturecontrol.passmark;
		       MarkerTime = NV_Params.TestSetupNV[testConfigA].fixturecontrol.passmarktime;
		       FailAckEn = NV_Params.TestSetupNV[testConfigA].fixturecontrol.failack;
		       NextTestPass = NV_Params.TestSetupNV[testConfigA].nexttestonpass;
		       NextTestFail = NV_Params.TestSetupNV[testConfigA].nexttestonfailoralarm;
		       DisResultsLamps = NV_Params.TestSetupNV[testConfigA].inhibitresults;
		       CalTimer = 500; //Initialise ready for use during Calibration
		       interTestTimer = NV_Params.TestSetupNV[testConfigA].spare2;
		       //Time to pulse isolate valve at end of fill if test pressure has changed
		       IsolatePlsTimer = NV_Params.TestSetupNV[testConfigA].spare3;
		       if (IsolatePlsTimer > 500) {IsolatePlsTimer = 250;} //Do not allow more than 500ms default to 250ms
		       diffpZero = 0;
		       diffpOffset = 0;
		       evacPls = false; //Used to detect first scan on evac step
		       MeasureTimerInc = 0;

//D/A Output to the E to P
		       if (TestPressure > 0) {
		    	   tempEtoP = (int)(TestPressure * 4095 / 250 / 10);
		    	   if (tempEtoP > 4095) tempEtoP = 4095;
		       } else tempEtoP = 0;
		       sim2.dac[1].data = tempEtoP;  //E to P uses DAC1 on J2 p13
//		       sim2.dac[0].data = tempEtoP;

		       stepCount = SETFIXOPSTARTSTEP;
//	          sprintf(buf, " Init A Done\r\n");
//	          logMessage(LOGPRIOMED, buf);
	       break;

	     case SETFIXOPSTARTSTEP:
// HRC1 20160218 Use IO Dig O/P 4,5,6,7 as Fixture Pass Indicators so do NOT change here
//	       tempIO = ((FixOPStart & 0x0F) << 4) & 0xF0; // Mask only the LS nibble and shift into the next nibble
//	       IO_OPs &= (tempIO | 0x0F); // Reset and bits that are to be switched off leaving any that are going to be switched on if they are already on
//	       IO_OPs |= tempIO; // Switch bit on that are currently off
	       tempIO = FixOPStart & 0xF0; // Mask only the MS nibble
	       SolValves &= (tempIO | 0x0F); // Reset any bits that are to be switched off leaving any that are going to be switched on if they are already on
	       SolValves |= tempIO; // Switch bit on that are currently off
	       AUTOCONNON;
//BGB1 Close fixture
//	       FIX1ON;
	       stepCount = FIXOPDELAYSTEP;
//	          sprintf(buf, " Init A Done\r\n");
//	          logMessage(LOGPRIOMED, buf);
	       break;

	     case FIXOPDELAYSTEP:
	       if (FixOPTimer <= 0)
	       {  stepCount = FIXIPCHECKSTEP;
//	          sprintf(buf, " Init A Done\r\n");
//	          logMessage(LOGPRIOMED, buf);
	       }
	       FixOPTimer--;
	       DisplayValue += (FixOPTimer/10);
	       break;

	     case FIXIPCHECKSTEP:
		   stepCount = SPARESTEP; // Default to continue
	       if (EnCheckIP && ((READCHKIPS & CheckIP) != CheckIP))
	       {  LT_ResTemp.status = RESSTATIPSNOK; //Alarm Fault Dig IPs check NOk
			  LT_ResTemp.testp = 0;
			  LT_ResTemp.diffpAZ = 0;
			  LT_ResTemp.diffp = 0;
			  LT_ResTemp.leakRate = 0;
			  stepCount = FAULTSTEP; //Alarm if IPs not correct (usually from Pressure Switches)
		      IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
		      ISOLATEOFF;
		      FILLOFF;
//	          sprintf(buf, " Init A Done\r\n");
//	          logMessage(LOGPRIOMED, buf);
	       }
	       break;

	     case SPARESTEP:
	    	 FixATimer = FixATime; //Set Timer to FixA preset - save preset for use at end of test
	    	 FixBTimer = FixBTime; //Set Timer to FixB preset - save preset for use at end of test
	    	 FixCTimer = FixCTime; //Set Timer to FixC preset - save preset for use at end of test
	    	 stepCount = FIXASTEP;
	       break;

	     case FIXASTEP:
		   if (FixAEn) {
		       FIXAON;
		       if (FixAFBack) {
		           if (FIXAFBIP) {
		               stepCount = FIXBSTEP; //Feedback input is On - step next
		           }
		           if (FixATimer <= 0) {
		        	   LT_ResTemp.status = RESSTATFIXNOK; //Alarm Fault Dig IPs check NOk
		        	   LT_ResTemp.testp = 0;
		        	   LT_ResTemp.diffpAZ = 0;
		        	   LT_ResTemp.diffp = 0;
		        	   LT_ResTemp.leakRate = 0;
		        	   stepCount = FAULTSTEP; //Alarm if Fixture Feedback timesout
		        	   IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
		        	   ISOLATEOFF;
		        	   FILLOFF;
		           }
		       }
		       else {
		           if (FixATimer <= 0) {
		               stepCount = FIXBSTEP;
		           }
		       }
		   }
		   else {
		       stepCount = FIXBSTEP;
		   }
		   FixATimer--;
		   break;

	     case FIXBSTEP:
		   if (FixBEn) {
		       FIXBON;
		       if (FixBFBack) {
		           if (FIXBFBIP) {
		               stepCount = FIXCSTEP; //Feedback input is On - step next
		           }
		           if (FixBTimer <= 0) {
		        	   LT_ResTemp.status = RESSTATFIXNOK; //Alarm Fault Dig IPs check NOk
		        	   LT_ResTemp.testp = 0;
		        	   LT_ResTemp.diffpAZ = 0;
		        	   LT_ResTemp.diffp = 0;
		        	   LT_ResTemp.leakRate = 0;
		        	   stepCount = FAULTSTEP; //Alarm if Fixture Feedback timesout
		        	   IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
		        	   ISOLATEOFF;
		        	   FILLOFF;
		           }
		       }
		       else {
		           if (FixBTimer <= 0) {
		               stepCount = FIXCSTEP;
		           }
		       }
		   }
		   else {
		       stepCount = FIXCSTEP;
		   }
		   FixBTimer--;
	       break;

	     case FIXCSTEP:
	    	 if (FixCEn) {
	    		 FIXCON;
			     if (FixCFBack) {
			    	 if (FIXCFBIP) {
			             stepCount = VENTOFFSTEP; //Feedback input is On - step next
			         }
			         if (FixCTimer <= 0) {
			        	 LT_ResTemp.status = RESSTATFIXNOK; //Alarm Fault Dig IPs check NOk
			        	 LT_ResTemp.testp = 0;
			        	 LT_ResTemp.diffpAZ = 0;
			        	 LT_ResTemp.diffp = 0;
			        	 LT_ResTemp.leakRate = 0;
			        	 stepCount = FAULTSTEP; //Alarm if Fixture Feedback timesout
			        	 IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
			        	 ISOLATEOFF;
			        	 FILLOFF;
			         }
			     }
			     else {
			    	 if (FixCTimer <= 0) {
			             stepCount = VENTOFFSTEP;
			         }
			     }
			 }
			 else {
			     stepCount = VENTOFFSTEP;
			 }
			 FixCTimer--;
	         break;

	     case VENTOFFSTEP:
	       if (!logNow) startGraphData(); // ---LZ Sep 2020, first time through only
	       logNow = TRUE; //Start logging data
	       VENTON;
	       AUTOCONNON;
	       PASSOFF;
	       FAILOFF;
	       FAULTOFF;
	       DONEOFF;
	       if (VentOffTimer <= 0)
	       {  stepCount = FILLSTEP;
	          sprintf(buf, " Vent Off A Done\r\n");
	          logMessage(LOGPRIOMED, buf);
	       }
	       VentOffTimer--;
	       DisplayValue += (VentOffTimer/10);
	       break;

	     case FILLSTEP:
	       FILLON;
	       VENTON;
	       if ( FillToPresEn && ( (vacTest && (testp <= TestPressure)) || (!vacTest && (testp >= TestPressure)) ) )
	       {
	    	   PresOverFillTimer--;
	       }
	       if ((FillTimer <= 0) || (FillToPresEn && (PresOverFillTimer <= 0)))
	       {  stepCount = STABILISESTEP;
	          sprintf(buf, " Fill A Done\r\n");
	          logMessage(LOGPRIOMED, buf);
	       }
	       FillTimer--;
	       DisplayValue += (FillTimer/10);
	       break;

	     case STABILISESTEP:
	       FILLOFF;
	       if (TestPressure != TestPressurePrev)
	       {  if (IsolatePlsTimer <= 0)
	          {  IsolatePlsTimer = 0; //Fix at 0 (or -1 when it's decremented)
	    	     ISOLATEOFF;
	          } else
	          {  ISOLATEON;
	          }
	       }
           if (StabTimer <= 0)
	       {  ISOLATEOFF; //Default to isolate valve off (open) in case StabTimer < IsolatePlsTimer
	          TestPressurePrev = TestPressure; //Save test pressure of this test
	          stepCount = ISOLATEDWELLSTEP;
	          sprintf(buf, " Stabilise A Done\r\n");
	          logMessage(LOGPRIOMED, buf);
//Only isolate diffp transducer if test not already failed and using DiffP Transducer
	          if ((LT_ResTemp.status == 0) && (!useTestP)) ISOLATEON;
	       }
	       StabTimer--;
	       IsolatePlsTimer--;
	       DisplayValue += (StabTimer/10);
	       break;

	     case ISOLATEDWELLSTEP:
//      	logNow = TRUE;
	       if (diffpOverp == true)   //Abort if diffp too high
	       {
		     ISOLATEOFF;
		     if (LT_ResTemp.status == 0)
		     {
		       LT_ResTemp.status = RESSTATDIFFPHI; //Diff Pressure too high - protect transducer
		       LT_ResTemp.testp = testp;
		       LT_ResTemp.diffpAZ = 0;
		       LT_ResTemp.diffp = diffp;
		       LT_ResTemp.leakRate = 999999; // Infinite - set to 9999.99
	           ResTPChange = 0.0;
			   IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
		       stepCount = FAILSTEP; //DiffP > Max Safety limit Jump to Fail Step
//	           stepCountA = FAILSTEP; //Do not jump to end continue to avoid interaction
//	           printf(" Abort");
//	           printf(" A TPX=%6.0f mb", (float)(LTA_Results.testp / 10.0));
//	           printf(" DifP=%7.1f Pa", (float)(LTA_Results.diffp / 10.0));
//	           printf(" LR=%8.2f mm3/s", (float)(LTA_Results.leakRate / 100.0));
//             printf(" DP0=%7.1f Pa\r\n", (float)(LTA_Results.diffpAZ / 10.0));
//	           sprintf(buf, " Test %c Fail High DiffP TestP=%5.1f DiffP=%5.1f Time=%5.3f Leak Rate=%5.2f\r\n", NV_Params.OptionsNV.LTA_ID, float(LTA_Results.testp)/10.0, float(LTA_Results.diffp)/10.0, float(MeasureTimerInc)/1000.0, float(LTA_Results.leakRate)/100.0);
//	           logMessage(LOGPRIOHIGH, buf);
		     }
	       }
	       if (IsolateTimer <= 0)
	       {
	         diffpZero = diffp;
	         testpstart = testp;
	         MeasureTimerInc = 0;
		     IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
	         stepCount = MEASURESTEP;
	         sprintf(buf, " Isolate Dwell A Done\r\n");
	         logMessage(LOGPRIOMED, buf);
	       }
	       IsolateTimer--;
	       DisplayValue += (IsolateTimer/10);
	       break;

	     case MEASURESTEP:
//	       ISOLATEON;
// Calculate running Offset Compensation
	       diffpOffset = int(MeasureTimerInc * OffComp) / 1000;
	       diffpCorrected = calcCorrectedDiffp(); //Calc Corrected leak pressure
	       if (diffpOverp == true)   //Abort if diffp too high
	       {
		     IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
		     ISOLATEOFF;
		     if (LT_ResTemp.status == 0)
		     {
		       LT_ResTemp.status = RESSTATDIFFPHI; //Diff Pressure to high - protect transducer
		       LT_ResTemp.testp = testp;
		       LT_ResTemp.diffpAZ = diffpZero;
		       LT_ResTemp.diffp = diffp;
	    	   if ((AlarmDiffP != 0) && (MeasureTimerInc != 0))
	    	   {
// abs removed
				 LT_ResTemp.leakRate = (diffpCorrected * AlarmLeakRate / AlarmDiffP * AlarmLeakRateTime / MeasureTimerInc);
	    	   } else LT_ResTemp.leakRate = 999999; // Infinite - set to 9999.99
	           ResTPChange = 0.0;
		       stepCount = FAILSTEP; //Abort measure DiffP > Max Safety limit Jump to Fail Step
//	           stepCountA = FAILSTEP; //Do not jump to end continue to avoid interaction
//	           sprintf(buf, " Test %c Fail High DiffP TestP=%5.1f DiffP=%5.1f Time=%5.3f Leak Rate=%5.2f\r\n", NV_Params.OptionsNV.LTA_ID, float(LTA_Results.testp)/10.0, float(LTA_Results.diffp)/10.0, float(MeasureTimerInc)/1000.0, float(LTA_Results.leakRate)/100.0);
//	           logMessage(LOGPRIOHIGH, buf);
		     }
	       }
	       if (diffpCorrected < NV_Params.OptionsNV.diffpNegAlarm)   //FAULT Negative diffp - Reference fault
	       {
	         IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
	         ISOLATEOFF;
		     if (LT_ResTemp.status == 0)
		     {
	    	   LT_ResTemp.status = RESSTATDIFFPREF; //Negative diffp - Reference fault
	    	   LT_ResTemp.testp = testp;
	    	   LT_ResTemp.diffpAZ = diffpZero;
	    	   LT_ResTemp.diffp = diffpCorrected;
	           if ((AlarmDiffP != 0) && (MeasureTimerInc != 0))
	           {
// abs removed
	        	 LT_ResTemp.leakRate = (diffpCorrected * AlarmLeakRate / AlarmDiffP * AlarmLeakRateTime / MeasureTimerInc);
	           } else LT_ResTemp.leakRate = 0; //avoid div by zero
	           ResTPChange = (float)(testp-testpstart)/10.0;
		       stepCount = FAULTSTEP; //Abort measure and Jump to Fault Step
//	           stepCountA = FAULTSTEP;
	           IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
	           ISOLATEOFF;
//	           sprintf(buf, " Test %c Done Ref Fault TestP=%5.1f DiffP=%5.1f Time=%5.3f Leak Rate=%5.2f\r\n", NV_Params.OptionsNV.LTA_ID, float(LTA_Results.testp)/10.0, float(LTA_Results.diffp)/10.0, float(MeasureTimerInc)/1000.0, float(LTA_Results.leakRate)/100.0);
//	           logMessage(LOGPRIOHIGH, buf);
		     }
	       }
	       if (diffpCorrected > AlarmDiffP)   //FAIL
	       {
		     if (LT_ResTemp.status == 0)
		     {
		       if (!DisResultsLamps)
		       {
		    	   LT_ResTemp.status = RESSTATFAIL; //Fail
		       } else
		       {
		    	   LT_ResTemp.status = RESSTATFAILNODISP; //Fail - But don't display
		       }
	    	   LT_ResTemp.testp = testp;
	    	   LT_ResTemp.diffpAZ = diffpZero;
	    	   LT_ResTemp.diffp = diffpCorrected;
	           if ((AlarmDiffP != 0) && (MeasureTimerInc != 0))
	           {
// abs removed
	        	 LT_ResTemp.leakRate = (diffpCorrected * AlarmLeakRate / AlarmDiffP * AlarmLeakRateTime / MeasureTimerInc);
	           } else LT_ResTemp.leakRate = 999999; // Infinite - set to 9999.99
	           ResTPChange = (float)(testp-testpstart)/10.0;
		       stepCount = FAILSTEP; //DiffP > Pass/Fail limit Jump to Fail Step
//	           stepCountA = FAILSTEP;
	           IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
	           ISOLATEOFF;
//	           sprintf(buf, " Test %c Done Fail TestP=%5.1f DiffP=%5.1f Time=%5.3f Leak Rate=%5.2f\r\n", NV_Params.OptionsNV.LTA_ID, float(LTA_Results.testp)/10.0, float(LTA_Results.diffp)/10.0, float(MeasureTimerInc)/1000.0, float(LTA_Results.leakRate)/100.0);
//	           logMessage(LOGPRIOHIGH, buf);
		     }
	       }
	       if (MeasureTimer <= 0) //PASS if not already failed for some other reason
	       {
		     if (LT_ResTemp.status >= RESSTATTESTPHI)
			 {
		       stepCount = FAULTSTEP;
			 }
		     else if (LT_ResTemp.status == RESSTATRESET)
		     {
		       stepCount = EVACSTEP;
		     }
		     else if (LT_ResTemp.status >= RESSTATFAIL)
		     {
		       stepCount = FAILSTEP;
		     }
		     else if (LT_ResTemp.status == 0)
		     {
			   if (!DisResultsLamps)
			   {
			      LT_ResTemp.status = RESSTATPASS; //Pass
			   } else
			   {
			      LT_ResTemp.status = RESSTATPASSNODISP; //Pass - But don't display
			   }
	    	   LT_ResTemp.testp = testp;
	    	   LT_ResTemp.diffpAZ = diffpZero;
	    	   LT_ResTemp.diffp = diffpCorrected;
	    	   if ((AlarmDiffP != 0) && (MeasureTimerInc != 0))
	    	   {
// abs removed
				 LT_ResTemp.leakRate = (diffpCorrected * AlarmLeakRate / AlarmDiffP * AlarmLeakRateTime / MeasureTimerInc);
	    	   } else LT_ResTemp.leakRate = 0;
	    	   ResTPChange = (float)(testp-testpstart)/10.0;
	    	   stepCount = PASSSTEP;
		     }
		     else stepCount = FAILSTEP; //Catch all - should never arrive here - but just in case
	         IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
	         ISOLATEOFF;
//	         sprintf(buf, " Test %c Done Pass TestP=%5.1f DiffP=%5.1f Time=%5.3f Leak Rate=%5.2f\r\n", NV_Params.OptionsNV.LTA_ID, float(LTA_Results.testp)/10.0, float(LTA_Results.diffp)/10.0, float(MeasureTimerInc)/1000.0, float(LTA_Results.leakRate)/100.0);
//	         logMessage(LOGPRIOHIGH, buf);
	       }
	       MeasureTimerInc ++;
	       MeasureTimer--;
	       DisplayValue += (MeasureTimer/10);
	       break;

	     case PASSSTEP:
//         logNow = FALSE;
	       ISOLATEOFF;
	       if (IsolateTimer <= 0)
	       {
	    	   if ((!failFlag) && (!faultFlag))
	    	   {
	    		   passFlag = true; //Only set Pass flag if a previous chained test has not already failed or alarmed
	    	   }
	    	   PASSOFF; //Switch all result ops off and only show the result if not disabled
	    	   FAILOFF;
	    	   FAULTOFF;
	    	   if (!DisResultsLamps)
	    	   {
		    	   PASSON; //Only show result if not disabled
	    	   }
// HRC1 20160218 Use IO Dig O/P 4,5,6,7 as Fixture Pass Indicators
// Switch ON the same bit that is configured in the FixOPStart parameter.  This parameter MUST be either H10, H20, H40 or H80
// This is also used to select the required Fixture Select Sol Valve (switches one of the 4 fixtures to the Leak Test Valve Module)
//BGB1 Don't use these outputs like this
//	    	   tempIO = FixOPStart & 0xF0; // Mask only the MS nibble
//		       IO_OPs &= (tempIO | 0x0F); // Reset any bits that are to be switched off leaving any that are going to be switched on if they are already on
//		       IO_OPs |= tempIO; // Switch OP bit on to drive the appropriate PASS indicator

	    	   nextConfig = NextTestPass;
	    	   stepCount = EVACSTEP;
	    	   logNow = FALSE;
	    	   logFull = TRUE;
	       }
	       IsolateTimer--;
	       DisplayValue += (IsolateTimer/10);
	       break;

	     case FAILSTEP:
//    	   logNow = FALSE;
	       ISOLATEOFF;
	       if (IsolateTimer <= 0)
	       {
    		   passFlag = false; //Clear Pass flag in case a previous chained test has already passed
    		   failFlag = true;
	    	   PASSOFF; //Switch all result ops off and only show the result if not disabled
	    	   FAILOFF;
	    	   FAULTOFF;
	    	   if (!DisResultsLamps)
	    	   {
		    	   FAILON; //Only show result if not disabled
	    	   }
	    	   nextConfig = NextTestFail;
	    	   stepCount = EVACSTEP;
	    	   logNow = FALSE;
	    	   logFull = TRUE;
	       }
	       IsolateTimer--;
	       DisplayValue += (IsolateTimer/10);
	       break;

	     case FAULTSTEP:
	       ISOLATEOFF;
	       if (IsolateTimer <= 0)
	       {
    		   passFlag = false; //Clear Pass flag in case a previous chained test has already passed
    		   failFlag = false;
    		   faultFlag = true;
	    	   PASSOFF; //Switch all result ops off and only show the result if not disabled
	    	   FAILOFF;
	    	   FAULTON;
	    	   nextConfig = 0; //On a fault - alarm terminate any chained tests
	    	   stepCount = EVACSTEP;
	    	   logNow = FALSE;
	    	   logFull = TRUE;
	       }
	       IsolateTimer--;
	       DisplayValue += (IsolateTimer/10);
	       break;

	     case EVACSTEP:
	       ISOLATEOFF;
	       FILLOFF;
	       VENTOFF;
	       diffpZero = 0;
	       diffpOffset = 0;
	       if (! evacPls)
	       {
	    	   endGraphData(); //lz Sep 2020 firt time through EVAC
		       LT_ResTemp.dateTime = dateTime; //Set Date and Time at end of test
	    	   LT_Results = LT_ResTemp;
	    	   resIndex = findFreeResultsFifoStruct();
	    	   if (resIndex >= 0)
	    	   {
	    		   ResultsFIFO[resIndex] = LT_ResTemp;
	    		   ResultsFIFO[resIndex].free = false;  //Make sure this element is marked as in use
// Post the message in the Fifo.  This is then handled in the resultsCommsTask
	    		   OSFifoPost( &ResultsOSFIFO, ( OS_FIFO_EL * ) &ResultsFIFO[resIndex] );
//	    		   iprintf("FIFO Index %d\r\n", resIndexA);
	    	   }
		   sim2.dac[1].data = 0;  //E to P uses DAC1 on J2 p13
		   //endGraphData(); //lz Sep 2020 firt time through EVAC
	       }
//Pass Marker
	       if (MarkerEn && passFlag && !failFlag && !faultFlag) {
	    	   if (MarkerTime > 0) {
	    		   MARKERON;
	    	   }
	    	   else {
	    		   MARKEROFF;
	    	   }
	    	   MarkerTime--;
	       }
//Step complete when Evac Time done AND if Pass Marking Marker Time done
	       if ((EvacTimer <= 0) && (!MarkerEn || !passFlag || (MarkerEn && MarkerTime <= 0)))
	       {
// HRC1 20160218 Use IO Dig O/P 4,5,6,7 as Fixture Pass Indicators so do NOT change here
//BGB1 Dont use this comment out
//	    	   tempIO = ((FixOPEnd & 0x0F) << 4) & 0xF0; // Mask only the LS nibble and shift into the next nibble
//		       IO_OPs &= (tempIO | 0x0F); // Reset and bits that are to be switched off leaving any that are going to be switched on if they are already on
//		       IO_OPs |= tempIO; // Switch bit on that are currently off
		       tempIO = FixOPEnd & 0xF0; // Mask only the MS nibble
		       SolValves &= (tempIO | 0x0F); // Reset any bits that are to be switched off leaving any that are going to be switched on if they are already on
		       SolValves |= tempIO; // Switch bit on that are currently off
		       MARKEROFF; //Make sure Marker is retracted
		       ChainTimer = 100;
			   stepCount = CHAINTESTSTEP;
			   //endGraphData(); //lz Sep 2020 last time through EVAC
	       }
	       evacPls = true;  //Used to detect first scan on evac step
	       EvacTimer--;
	       DisplayValue += (EvacTimer/10);
	       break;

	     case CHAINTESTSTEP:
		   if (ChainTimer <= 0)
		   {
			   stepCount = FIXCENDSTEP; //Default to end of test in case Reset was pressed before any result flag has been set
			   if ((nextConfig > 0) && (nextConfig <= 15) && (nextConfig != testConfigA))
	    	   {
	    		   testConfigA = nextConfig; //Make sure next Config is valid and not the same as the current test number!
	    		   stepCount = LOADPARAMSSTEP;

	    	   }
	    	   else if (faultFlag)
		       {
			    	   PASSOFF; //Fault - Alarm OP
			    	   FAILOFF;
			    	   FAULTON;
		    		   stepCount = FIXCENDSTEP;
		       }
	    	   else if (failFlag)
		       {
			    	   PASSOFF; //Fail OP
			    	   FAULTOFF;
			    	   if (!DisResultsLamps)
			    	   {
			    		   FAILON;
			    	   }
		    		   stepCount = FIXCENDSTEP;
		       }
	    	   else if (passFlag)
		       {
			    	   FAULTOFF;
			    	   FAILOFF;
			    	   if (!DisResultsLamps)
			    	   {
			    		   PASSON;  //Pass OP
			    	   }
		    		   stepCount = FIXCENDSTEP;
		       }
		   }
		   ChainTimer--;
	       DisplayValue += (ChainTimer/10);
	       FixATimer = FixATime; //Set Timer to FixA preset - save preset for use at end of test
	       FixBTimer = FixBTime; //Set Timer to FixA preset - save preset for use at end of test
	       FixCTimer = FixCTime; //Set Timer to FixA preset - save preset for use at end of test
	       break;

	     case FIXCENDSTEP:
	    	 if (FixCEn) {
	    	     FIXCOFF; // should this be switched off even if not enabled?
	    	     if (FixCTime <= 0) {
	    	         stepCount = FIXBENDSTEP;
	    	     }
	    	 }
	    	 else {
	    	     stepCount = FIXBENDSTEP;
	    	 }
	    	 FixCTime--;
	       break;

	     case FIXBENDSTEP:
	    	 if (FixBEn) {
	    	     FIXBOFF; // should this be switched off even if not enabled?
	    	     if (FixBTime <= 0) {
	    	    	 webReset = false; // reset Reset flag
	    	    	 stepCount = FAILACKSTEP;
	    	     }
	    	 }
	    	 else {
	    		 webReset = false; // reset Reset flag
	    		 stepCount = FAILACKSTEP;
	    	 }
	    	 FixBTime--;
	       break;

	     case FAILACKSTEP:
	    	 if (FailAckEn && !passFlag) {
	    	     if (RESETLT || webReset) {
	    	    	 webReset = false; // reset Reset flag
	    	    	 stepCount = FIXAENDSTEP;
	    	     }
	    	 }
	    	 else {
	    	     stepCount = FIXAENDSTEP;
	    	 }
	       break;

	     case FIXAENDSTEP:
	    	 if (FixAEn) {
	    	     FIXAOFF; // should this be switched off even if not enabled?
	    	     if (FixATime <= 0) {
	    	         stepCount = COMPLETESTEP;
	    	     }
	    	 }
	    	 else {
	    	     stepCount = COMPLETESTEP;
	    	 }
	    	 FixATime--;
	       break;

	     case COMPLETESTEP:
	       logNow = FALSE;
	       evacPls = false;
	       ISOLATEOFF;
	       FILLOFF;
	       VENTOFF;
	       AUTOCONNOFF;
//BGB1 Open Fixture
//	       FIX1OFF;
	       DONEON;
	       diffpZero = 0;
	       diffpOffset = 0;
	       strcpy(textInput, "None"); //V2.8_20200305  BAE8 - Init Serial Data Input
	       barCodeOk = false;
	       if (NV_Params.TestSetupNV[testConfigA].spare2 <= 0) {
		       stepCount = RESETSTEP;
		       flashDoneFlag = false;
	    	   webStart = false; // make sure these web button interface flags are reset at the end of test
//    	   webStartAB = false; // so any button clicks during the test are ignored
	    	   webReset = false;
		       sprintf(buf, " Test A Complete\r\n\r\n");
		       logMessage(LOGPRIOMED, buf);
	       }
	       else {
	    	   if (interTestTimer <= 0) {
			       stepCount = RESETSTEP;
			       flashDoneFlag = true;
		    	   webStart = false; // make sure these web button interface flags are reset at the end of test
	//    	   webStartAB = false; // so any button clicks during the test are ignored
		    	   webReset = false;
			       sprintf(buf, " Test A Complete\r\n\r\n");
			       logMessage(LOGPRIOMED, buf);
	    	   }
	       }
	       interTestTimer--;
	       break;

	     case DIFFPCAL1:
		   ISOLATEOFF;
		   FILLOFF;
		   VENTON;
		   AUTOCONNOFF;
	       PASSOFF;
	       FAILOFF;
	       FAULTOFF;
	       DONEOFF;
	       diffpZero = 0;
	       diffpOffset = 0;
	       if (CalTimer <= 0)
	       {  stepCount = DIFFPCAL2;
	          CalTimer = 500;
	       }
	       CalTimer--;
	       break;

	     case DIFFPCAL2:
		   ISOLATEON;
		   FILLOFF;
		   VENTON;
		   AUTOCONNOFF;
	       PASSOFF;
	       FAILOFF;
	       FAULTOFF;
	       DONEOFF;
           CalTimer = 200;
// To leave this step either use web button or Reset input
	       break;

	     case TESTPCAL1:
		   ISOLATEOFF;
		   FILLOFF;
		   VENTON;
		   AUTOCONNON;
	       PASSOFF;
	       FAILOFF;
	       FAULTOFF;
	       DONEOFF;
	       diffpZero = 0;
	       diffpOffset = 0;
	       if (CalTimer <= 0)
	       {  stepCount = TESTPCAL2;
	          CalTimer = 500;
	       }
	       CalTimer--;
	       break;

	     case TESTPCAL2:
		   ISOLATEOFF;
		   FILLON;
		   VENTON;
		   AUTOCONNON;
	       PASSOFF;
	       FAILOFF;
	       FAULTOFF;
	       DONEOFF;
           CalTimer = 200;
// To leave this step either use web button or Reset input
	       break;

	     case ENDCALSTEP:
		   ISOLATEOFF;
		   FILLOFF;
		   VENTOFF;
	       if (CalTimer <= 0)
	       {
	    	 stepCount = COMPLETESTEP;
	    	 CalTimer = 500;
	       }
	       CalTimer--;
	       break;

	     default:
	       break;
	   } //Switch

//If Dosing and Gross Leak detected after dose
	  if ((stepCount == STABILISESTEP) && (StabTimer < 1000) && (DosingGLPLimit != 0))
	  {   //Vac Mods
		  if ( (vacTest && (testp > (TestPressure + DosingGLPLimit )))
			|| (!vacTest && (testp < (TestPressure - DosingGLPLimit ))) )
		  {
  		      ISOLATEOFF;
			  if (LT_ResTemp.status == 0)
			  {
			    LT_ResTemp.status = RESSTATTESTPLOW; //Fail Pressure too low
			    LT_ResTemp.testp = testp;
			    LT_ResTemp.diffpAZ = 0;
			    LT_ResTemp.diffp = 0;
			    LT_ResTemp.leakRate = 0;
			    stepCount = FAILSTEP;
		        IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s.
//		        sprintf(buf, " Test %c Done TestP Low Fail TestP=%5.1f DiffP=%5.1f Time=%5.3f Leak Rate=%5.2f\r\n", NV_Params.OptionsNV.LTA_ID, float(LTA_Results.testp)/10.0, float(LTA_Results.diffp)/10.0, float(MeasureTimerInc)/1000.0, float(LTA_Results.leakRate)/100.0);
//		        logMessage(LOGPRIOHIGH, buf);
		      }
		  }
	  }

//Check any Enabled Fixture Feedback I/Ps in the final 0.2s of Stabilisation
	  if ((stepCount == STABILISESTEP) && (StabTimer < 200)) {
		  if ((FixAEn && FixAFBack && !FIXAFBIP) || (FixBEn && FixBFBack && !FIXBFBIP) || (FixCEn && FixCFBack && !FIXCFBIP) ){
	        	   LT_ResTemp.status = RESSTATFIXNOK; //Alarm Fault Dig IPs check NOk
	        	   LT_ResTemp.testp = 0;
	        	   LT_ResTemp.diffpAZ = 0;
	        	   LT_ResTemp.diffp = 0;
	        	   LT_ResTemp.leakRate = 0;
	        	   stepCount = FAULTSTEP; //Alarm if Fixture Feedback not correct at end of Stabilisation
	        	   IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
	        	   ISOLATEOFF;
	        	   FILLOFF;
		  }
	  }

// Check for Gross Leak due to test pressure change
// 20160926 Only check during final 0.5s of stabilise to allow
// the correct Test Pressure to be measured in very low volume fast systems
//	  if ((stepCount >= STABILISESTEP) && (stepCount <= MEASURESTEP))
	  if (((stepCount == STABILISESTEP) && (StabTimer < 500)) || ((stepCount > STABILISESTEP) && (stepCount <= MEASURESTEP)))
	  {   //Vac Mods
		  if ( (vacTest && (testp > (TestPressure - (TestPressure * NV_Params.OptionsNV.testpTol /100))))
			|| (!vacTest && (testp < (TestPressure - (TestPressure * NV_Params.OptionsNV.testpTol /100)))) )
		  {
  		      ISOLATEOFF;
			  if (LT_ResTemp.status == 0)
			  {
			    LT_ResTemp.status = RESSTATTESTPLOW; //Fail Pressure too low
			    LT_ResTemp.testp = testp;
			    LT_ResTemp.diffpAZ = 0;
			    LT_ResTemp.diffp = 0;
			    LT_ResTemp.leakRate = 0;
			    stepCount = FAILSTEP;
			    IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s.
//Next If used on Dual MALT to only abort if in the early part of Stabilise to avoid interaction with other fixture
//			    if ((stepCount == STABILISESTEP) && (StabTimer > 4000))
//			    {  //If still got more then 4s of stabilise to go - jump to end
//		          stepCount = FAILSTEP;
//		          IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s.
//			    }
//		        sprintf(buf, " Test %c Done TestP Low Fail TestP=%5.1f DiffP=%5.1f Time=%5.3f Leak Rate=%5.2f\r\n", NV_Params.OptionsNV.LTA_ID, float(LTA_Results.testp)/10.0, float(LTA_Results.diffp)/10.0, float(MeasureTimerInc)/1000.0, float(LTA_Results.leakRate)/100.0);
//		        logMessage(LOGPRIOHIGH, buf);
		      }
		  }
	  }

// Check for Fault due to test pressure excessive
// 20160926 Only check during final 0.5s of stabilise to allow
// the correct Test Pressure to be measured in very low volume fast systems
// that might need to allow the Test Pressure to overshoot before settling back
//	  if ((stepCount >= FILLSTEP) && (stepCount <= MEASURESTEP))
	  if (((stepCount == STABILISESTEP) && (StabTimer < 500)) || ((stepCount > STABILISESTEP) && (stepCount <= MEASURESTEP)))
	  {   //Vac Mods
		  if ( (vacTest && (testp < (TestPressure + (TestPressure * NV_Params.OptionsNV.testpTol /100))))
			|| (!vacTest && (testp > (TestPressure + (TestPressure * NV_Params.OptionsNV.testpTol /100)))) )
		  {  //even if already failed for any other reason - always jump to evacuate
			  LT_ResTemp.status = RESSTATTESTPHI; //Fail Pressure too high
			  LT_ResTemp.testp = testp;
			  LT_ResTemp.diffpAZ = 0;
			  LT_ResTemp.diffp = 0;
			  LT_ResTemp.leakRate = 0;
			  stepCount = FAULTSTEP;
		      IsolateTimer = 100; // Use for open Isolate valve delay - Fix at 0.1s
		      ISOLATEOFF;
		      FILLOFF;
//		      sprintf(buf, " Test %c Done TestP High Fault TestP=%5.1f DiffP=%5.1f Time=%5.3f Leak Rate=%5.2f\r\n", NV_Params.OptionsNV.LTA_ID, float(LTA_Results.testp)/10.0, float(LTA_Results.diffp)/10.0, float(MeasureTimerInc)/1000.0, float(LTA_Results.leakRate)/100.0);
//		      logMessage(LOGPRIOHIGH, buf);
		  }
	  }

// Simple Reset Switch operation - Exclude Test Reset if already on the Reset step (0) and if already finished measure and evacuating and end of Calibration steps
//	  if (( RESETLT || webReset) && (stepCount != RESETSTEP) && (stepCount != EVACSTEP) && (stepCount != CHAINTESTSTEP) && (stepCount != COMPLETESTEP) && (stepCount != ENDCALSTEP) )
	  if (( RESETLT || webReset) && (stepCount != RESETSTEP) && (stepCount != ENDCALSTEP) && !((stepCount >= PASSSTEP) && (stepCount <= COMPLETESTEP))  )
	  {  //Set ALL TestResults fields as we might have arrived here from calibration mode so the fields might not be set
	      LT_ResTemp.msgID = 0; //Structure contains results data
	      LT_ResTemp.LT_ID = NV_Params.OptionsNV.LT_ID; //Set LT A identifier character
	      LT_ResTemp.params = testConfigA; //Test Parameter set
		  LT_ResTemp.status = RESSTATRESET; //Test Sequence Reset
		  LT_ResTemp.testp = testp;
		  LT_ResTemp.diffpAZ = 0;
		  LT_ResTemp.diffp = 0;
		  LT_ResTemp.leakRate = 0;
	      LT_ResTemp.dateTime = dateTime; //Set Date and Time
          passFlag = false; //Clear these flags so on Reset neither Pass / Fail is shown
          failFlag = false; //...leave faultFlag though so any Alarm condition is still shown
	      nextConfig = 0; // Zero to prevent chaining to the next test at the end of evacuation
    	  webReset = false; // reset Reset flag
   	      webStart = false; // reset Start flag so test

		  if ((stepCount >= DIFFPCAL1) && (stepCount <= TESTPCAL2))
		  {
			stepCount = ENDCALSTEP;
		  } else
		  {
		    stepCount = EVACSTEP;
		  }
//	      sprintf(buf, " Test %c Reset TestP=%5.1f DiffP=%5.1f Time=%5.3f Leak Rate=%5.2f\r\n", NV_Params.OptionsNV.LTA_ID, float(LTA_Results.testp)/10.0, float(LTA_Results.diffp)/10.0, float(MeasureTimerInc)/1000.0, float(LTA_Results.leakRate)/100.0);
//	      logMessage(LOGPRIOHIGH, buf);
	      calibrationBusy[0]=false; //reset flags for Calibration web buttons start/stop test
	      calibrationBusy[1]=false;
	  }

//Reset when waiting to Start a test - switch Pass / Fail and Fault lamps off
	  if (( RESETLT || webReset) && (stepCount == RESETSTEP))
	  {
	      PASSOFF;
	      FAILOFF;
	      FAULTOFF;
	      DONEON; //Lamps Off - but leave Done / Ready ON
//BGB1 - Open Fixture
//          FIX1OFF;

// HRC1 20160218 Use IO Dig O/P 4,5,6,7 as Fixture Pass Indicators so switch ALL Off at Start of Test
//BGB1 Don't use these outputs
//	      IO_OPs &= 0x000f;

    	  webReset = false; // reset Reset flag
   	      webStart = false; // reset Start flag so test
//Next lines are unnecessary - but do no harm
		  passFlag = false; //Clear these flags so on Reset neither Pass / Fail is shown
          failFlag = false; //...leave faultFlag though so any Alarm condition is still shown
	      nextConfig = 0; // Zero to prevent chaining to the next test at the end of evacuation

	      calibrationBusy[0]=false; //reset flags for Calibration web buttons start/stop test
	      calibrationBusy[1]=false;
	  }


	  writeOutputs( SolValves, IO_OPs ); // Update on and off board outputs
// Save data in Running Values structure
      if (stepCount == MEASURESTEP)
    	  dataIn.diffp = calcCorrectedDiffp(); //Calc Corrected leak pressure
      else
    	  dataIn.diffp = diffp;
      dataIn.diffpCount = ADAve.chan0;
	  dataIn.testp = testp;
	  dataIn.testpCount = ADAve.chan2;

} // End of leakTestASeq



void testSeqTask( void *pdata )
{
//	char buf[80];
	WORD data = *( WORD * ) pdata; // cast passed parameter
	sprintf(gLogBuf, "Data passed to testSeqTask(): %d\r\n", data );
    logMessage(LOGPRIOLOW, gLogBuf);
    OSSemInit( &PitSem1, 0 );
    sprintf(gLogBuf, "Setting PIT 1 timer to 1ms\r\n");
    logMessage(LOGPRIOLOW, gLogBuf);
    // Init for timer 1, at 1ms intervals
    InitPitOSSem( 1, &PitSem1, 1000 );
	while ( 1 )
	{
       BYTE status = OSSemPend( &PitSem1, TICKS_PER_SECOND * 5 );
       if ( status == OS_NO_ERR )
       {
//    	    J2[39] = 1; //Debug to measure interrupt time on scope
//    	    LoopMain();
// Process the Control Loop and Sequence
    	    readAnalogue(); // Read average and scale all analogues
// Read the Digital Inputs
   		    DigIPs = readInputs(); // Read the inputs (write outputs at the end of each sequence
            leakTestSeq(); // Process Leak Test A sequence
//            leakTestBSeq(); // Process Leak Test B sequence

//            testp = 3000;	//LZ testing purposes
//            diffp = 50;		//LZ testing purposes

            //log graph data ---LZ Nov 2019
        	if (gPitCount[1]%40==0) { //every 40 beats (millisec)
        		pushGraphData(
        				stepCount, //LZ Oct 2020
        				testp,
        				(stepCount==MEASURESTEP || stepCount==PASSSTEP)? calcCorrectedDiffp() : diffp,
        				gPitCount[1] //duration()+getChainedDuration()
        		);
        	}


//    	   if ((gPitCount[1] % 25) == 0)
//    	   {
//    		 if ((stepCount >= INITSTEP) && (stepCount <= EVACSTEP))
//    		 {
//// Send periodic messages to listening PC via UDP
//    		   udpMessageIndex = findFreeResultsFifoStruct();
//	    	   if (udpMessageIndex >= 0)
//	    	   {
//	    		   ResultsFIFO[udpMessageIndex].msgID = 11; //Pressure Data
//				   ResultsFIFO[udpMessageIndex].LT_ID = NV_Params.OptionsNV.LT_ID; //Set LT A identifier character
//				   ResultsFIFO[udpMessageIndex].params = 1; //Test Parameter set (only 1 available at the present)
//				   ResultsFIFO[udpMessageIndex].status = gPitCount[1]; //1ms time base. zero at start of test
//				   ResultsFIFO[udpMessageIndex].testp = testp;
//				   ResultsFIFO[udpMessageIndex].diffp = diffp - diffpZero - diffpOffset;
//				   ResultsFIFO[udpMessageIndex].diffpAZ = stepCount;
//	    		   ResultsFIFO[udpMessageIndex].free = false;  //Make sure this element is marked as in use
//// Post the message in the Fifo.  This is then handled in the resultsCommsTask
//	    		   OSFifoPost( &ResultsOSFIFO, ( OS_FIFO_EL * ) &ResultsFIFO[udpMessageIndex] );
////	    		   iprintf("FIFO Index %d\r\n", resIndexA);
//	    	   }
//    		 }
//
////    		   gPitCount[1] = 0;
////    		   SolValves ^= 0x80;
////    		   iprintf("Tick - SolValves= %d\r\n", SolValves);
////    		   iprintf("Dig IPs = %2X\r\n", readInputs());
////    		   sprintf(buf, "TP= %d DP= %d FTimer= %d STimer= %d MTimer= %d\r\n", testpA, diffpA, FillTimer, StabTimer, MeasureTimer);
////    		   logMessage(LOGPRIOMED, buf);
//    	   }
//    	   J2[39] = 0; //Debug to measure interrupt time on scope

       }
       else
          iprintf("ERROR - OSSemPend(&PitSem1) returned %d\r\n", status);
	}
} //End of MyTask2


