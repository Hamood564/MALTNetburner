/**
 * main.cpp
 * MALT web interface - initialisation and main loop
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT1.1
 * @date 2015-01-13
 */

#include <predef.h>
#include <stdio.h>
#include <startnet.h>
#include <init.h>
#include <assert.h>
#include <syslog.h>
//#include <mcf5441x_rtc.h>
#include<nbtime.h>
//#include <pins.h>

// MALT Project includes
#include "mcf5441x_rtc.h"  //Local 54415 RTC support
#include "MLT_globalTypes.h" // Data type definitions - particularly record structures
#include "MLT_globalDefs.h"  // Globals defined here - everywhere else should use "MLT_globalExterns.h"
#include "test_seq.h"  // MALT leak test sequence routines
#include "udp_input.h"  // MALT UDP input
#include "serial_input.h"  // MALT Serial RS232 input
#include "tcp/tcp_server.h"  // MALT Serial RS232 input
#include "datacapture.h"
#include "resultsComms.h"  // MALT Results Comms and logging
//#include "message_log.h"

//includes for ftp and effs_time - added 22/12/2014
#include <ftpd.h>
#include "ftp/effs_time.h"
#include "ftp/FileSystemUtils.h"

#include "importexport/userParameters.h" //import and export functions

#define MULTI_MMC TRUE	// For modules with onboard flash sockets, even if you are using external flash cards
#include <effs_fat/multi_drive_mmc_mcf.h>



#define FTP_PRIO ( MAIN_PRIO + 2 )

extern "C" {
void UserMain(void * pd);
void MQTTTask(void * pd);
}

//Declare our function from our form post code.
void RegisterPost();

DWORD testSeqTaskStack[USER_TASK_STK_SIZE];
DWORD udpInputTaskStack[USER_TASK_STK_SIZE];
DWORD serialInputTaskStack[USER_TASK_STK_SIZE];
DWORD tcpServerTaskStack[USER_TASK_STK_SIZE];
DWORD resultsCommsTaskStack[3072];
//DWORD resultsCommsTaskStack[USER_TASK_STK_SIZE];

//const char * AppName = "MALT4_TCP_V1.1 20240102"; //Moved to MLT_globalDefs.h

void getConfigParamsFromSavedData();
void saveNV_Params();
void initNV_Params();
void printNV_Params();
void initNV_ParamsOptions(); // Only used to separately initialise the Options

void initTime() {
	// Set system time from RTC
	int RTC_result = MCF541X_RTCSetSystemFromRTCTime();
	// Print time to check it's ok
//	DWORD tv = time(NULL);
//	gmtime_r((const time_t *) &tv, &dateTime);
	time_t tv = time(NULL); //note needs #include nbtime.h
	gmtime_r( &tv, &dateTime);

	iprintf("RTC time = ");
	printTimeStruct(dateTime);
	iprintf("\r\n\r\n");
}

/* Code to start FTP server - Added 22/12/2014 */
void initFTPServer() {
	OSChangePrio(FTP_PRIO);  // Priority defined above
	f_enterFS();
	OSChangePrio(MAIN_PRIO);

	int status = FTPDStart(21, FTP_PRIO);
	if (status == FTPD_OK) {
		iprintf("Started FTP Server on ");
		ShowIP(EthernetIP);
		iprintf("\r\n");
	} else {
		iprintf("** Error: %d. Could not start FTP Server\r\n", status);
	}
}
/*--------------------------------------------*/

void UserMain(void * pd) {
	initWithWeb(); // Perform all the nice backend setup: network stack, DHCP, Webserver, etc.

	initTime(); //time initialisation moved to be before mounting onboard SD card - see function above

	/*---Additional code for FTP Server and onboard SD Card- 22/12/2014 ---*/
	f_enterFS(); //needed in every task that uses the EFFS file system (SD Card)
	InitExtFlash(); // Initialize the CFC or SD/MMC external flash drive
					// This mounts the card as drive EXT_FLASH_DRV_NUM which is a macro to
					// MMC_DRV_NUM  declared in <effs_fat/fat.h>
					// This makes the SD Card accessible from this MAIN task
//	ReadWriteTest();
	initFTPServer(); //create FTP task and start FTP server
	/*-------------------------------------------------*/

	InitialiseDataLog();

	iprintf("Application: %s\r\nNNDK Revision: %s\r\n", AppName, GetReleaseTag());

	//load saved TestConfigParameters - dangerous unless can guarantee previously saved
	getConfigParamsFromSavedData();
	//NV_Params.OptionsNV.option1 =-1; //lz: to force initialisation - one time op
	if (NV_Params.OptionsNV.option1 < 0) //This int will be -1 if NV params never saved
	{
		printf("Bad Parameters - Initialisation in process \r\n");
		initNV_Params(); //initialise parameters
		saveNV_Params();
	}
	if (NV_Params.OptionsNV.option1 == 9999) //Initialise just the Option Parameters
	{
		printf("Bad Option Parameters - Initialisation in process \r\n");
		initNV_ParamsOptions(); //initialise parameters
		saveNV_Params();
	}

//	initNV_ParamsOptions();//Only uncomment to separately force an init of the Options
	printNV_Params(); //Send Parameters to debug Stdout
//  Call a registration function for Form code...see formcode.cpp
	RegisterPost();

	//SysLogAddress = NV_Params.OptionsNV.logIP; // Set IP for UDP sys log debug data from Param Options
	SysLogAddress = NV_Params.OptionsNV.logIP.Extract4();
//    SysLogAddress = AsciiToIp("192.168.115.209"); // Force initial IP address for UDP sys log output
	initHardware(); //Init system hardware
	initTestSoftware(); //Init system software

	// Initialise a new task that will wait for Results data to communicate and Log.
	// Results are sent via an OS_FIFO using an array ResultsFIFO[10] of type TestResults
	// The array is defined with 10 elements, hopefully no more than 2 (for LTA and LTB)
	// should be needed at any one time.
	// Create and launch the task. Note that we selected a priority of
	// MAIN_PRIO -3, which means the new task is of a higher priority than UserMain.
	// and udpInput. Hopefully this should mean that results are handled as they are posted.
	OSFifoInit(&ResultsOSFIFO);
	initResultsFifoMembers();
	if (OSTaskCreate(resultsCommsTask, NULL,
			(void *) &resultsCommsTaskStack[3072], //[USER_TASK_STK_SIZE]
			(void *) resultsCommsTaskStack, MAIN_PRIO - 3) != OS_NO_ERR)
	{
		iprintf("*** Error creating Results Comms task\r\n");
	}

	// Initialise a new task that will wait for UDP input
	// Pass the Port to listen for commands on - eventually a parameter perhaps?
	int udpInputTaskPort = 9284; //Default but use Options setting if not zero
	if (NV_Params.OptionsNV.logPort > 0)
		udpInputTaskPort = NV_Params.OptionsNV.logPort;

	// Create and launch the task. Note that we selected a priority of
	// MAIN_PRIO -2, which means the new task is of a higher priority than UserMain.
	// This should mean that udp packets are processed as they arrive.
	if (OSTaskCreate(udpInputTask, (void *) &udpInputTaskPort,
			(void *) &udpInputTaskStack[USER_TASK_STK_SIZE],
			(void *) udpInputTaskStack, MAIN_PRIO - 2) != OS_NO_ERR) //Task priority was MAIN_PRIO - 2
	{
		SysLog("*** Error creating udpInputTask\r\n");
	}

	// Initialise a new task that will wait for Serial Data input (Barcode Reader)
	// Pass other useful data - or NONE = NULL
/*	int udpInputTaskPort = 9284; //Default but use Options setting if not zero
	if (NV_Params.OptionsNV.logPort > 0)
		udpInputTaskPort = NV_Params.OptionsNV.logPort;  */

	// Create and launch the task. Note that we selected a priority of
	// MAIN_PRIO -1, which means the new task is of a higher priority than UserMain but lower than the other tasks.
	if (OSTaskCreate(serialInputTask, NULL,
			(void *) &serialInputTaskStack[USER_TASK_STK_SIZE],
			(void *) serialInputTaskStack, MAIN_PRIO - 1) != OS_NO_ERR) //Task priority was MAIN_PRIO - 1
	{
		SysLog("*** Error creating serialInputTask\r\n");
	}

	//-----------------TCP server--------------------
	// Create and launch the tcp_server task. Note: need to set the right priority.
	// ---- to disuss...
	if (OSTaskCreate(tcpServerTask, (void  *) TCP_LISTEN_PORT,
			(void *) &tcpServerTaskStack[USER_TASK_STK_SIZE],
			(void *) tcpServerTaskStack, MAIN_PRIO - 4) != OS_NO_ERR) //TCP_PRIO (40)
	{
		SysLog("*** Error creating tcp_server_Task\r\n");
	}
	//-----------------------------------------------


	// Initialise a new task that will wait for the 1ms timer interrupt semaphore
	// This example passes data to the task, but this is not used in this task
	WORD testSeqTaskData = 1234;
	// Create and launch the task. Note that we selected a priority of
	// 20, which means the new task is of a much higher priority than UserMain
	// or the built in webserver and network tasks.
	// Therefore this task MUST block or nothing else will ever run.
	if (OSTaskCreate(testSeqTask, (void *) &testSeqTaskData,
			(void *) &testSeqTaskStack[USER_TASK_STK_SIZE],
			(void *) testSeqTaskStack, 20) != OS_NO_ERR) //Task priority was MAIN_PRIO +2
	{
		SysLog("*** Error creating testSeqTask\r\n");
	}


	while (true) {

		static int hourPrev = 0; //Used locally to detect hour change

//		if (DigIPs & 0x04) //
//		{
////        iprintf ("C0=%5d C1=%5d C2=%5d C3=%5d",ADAve.chan0, ADAve.chan1, ADAve.chan2, ADAve.chan3);
//			printf(" DifP= %5.1f Pa", (float) (diffp - diffpZero - diffpOffset) / 10);
//			printf(" TPSun= %6.1f mb\r\n", (float) testp / 10);
////        printf(" TP= %5.1f mb\r\n", (float)testp1/10);
////        iprintf(" s= %d\n", isr_cnt/1000);  // display timer2 counter
//		}
//		if ((not J2[42]) and logFull) {
//			sendLog(logCount);
////    	  if (not J2[42]) {SendLog(logCount);} else {logFull = FALSE;}
//		}

		LogTestData();
		OSTimeDly(TICKS_PER_SECOND / 4); //Delay 0.25s

		//Keep current time and date in global dateTime struct
		//DWORD tv = time(NULL); //Use System time (corrected to the RTC time every hour see below)
		//gmtime_r((const time_t *) &tv, &dateTime);
		time_t tv = time(NULL); //note needs #include nbtime.h
		gmtime_r( &tv, &dateTime);

		if (hourPrev != dateTime.tm_hour)
		{
// Set system time from RTC - RTC is more accurate than system time so update every hour
			int RTC_result = MCF541X_RTCSetSystemFromRTCTime();
//			DWORD tv = time(NULL);
//			gmtime_r((const time_t *) &tv, &dateTime);
			time_t tv = time(NULL);
			gmtime_r( &tv, &dateTime);

			hourPrev = dateTime.tm_hour;
			iprintf("System time hourly update = ");
			printTimeStruct(dateTime);
			iprintf("\r\n");
		}

		//LZ import/export functionality - Dec 2019
    	//Service SD Card access requests - Note EFFS has been initiated for MAIN task
    	serviceMemoryCardReq();

	}
}

