/**
 * udp_input.cpp
 * Implementation of UDP IO task.
 * @author gavin.murray
 * @version Single_MALT1.1
 * @date 2015-01-13
 Command List.  Commands are single upper case characters
 A Log Priority Always
 L Log Priority Low
 M Log Priority Medium
 H Log Priority High
 U Log to UDP
 S Log to Serial
 O Logging Off (No log destination set)
 T Set the Time.  T&&dayofmonth=24&&month=1&&year=2017&&hour=18&&min=56&&sec=19&&wday=2
 R Force a Reboot
 P Output Option Parameters
 V Force a re-initialisation of just the Option Parameters
 G Go Start a Leak test.  G&&prog=NN   NN = 0 to 15 Test Program number



 */

#include <utils.h>
#include <stdio.h>
#include <udp.h>
#include <string.h>
#include <syslog.h>
//#include <mcf5441x_rtc.h>
#include <http.h>
#include <stdlib.h>
#include <bsp.h>

#include <effs_fat/multi_drive_mmc_mcf.h>
#include "ftp/FileSystemUtils.h"


#include "mcf5441x_rtc.h"  //Local 54415 RTC support
#include "message_log.h"
#include "MLT_globalExterns.h"


void udpInputTask( void *pdata )
{
//   WORD data = *( WORD * ) pdata; // cast passed parameter
   int port = *( int * ) pdata;
   iprintf( "UdpReaderTask listening for UDP packets on port %d\r\n", port );
// Create a UDP socket for receiving
   int UdpFd = CreateRxUdpSocket( port );
   if ( UdpFd <= 0 )
   {
	  sprintf(gLogBuf, "Error Creating UDP Listen Socket: %d\r\n", UdpFd);
      logMessage(LOGPRIOMED, gLogBuf);
	  while (1)
		 OSTimeDly(TICKS_PER_SECOND); //Hmm what's to be done if we can't create a listening socket?
	  	  	  	  	  	  	  	  	  //...should never happen - so nothing - UDP input won't work
   }
   else
   {
	  sprintf(gLogBuf, "UdpReaderTask listening for UDP packets on port %d\r\n", port );
      logMessage(LOGPRIOMED, gLogBuf);
   }

   while (1)
   {
	  IPADDR SrcIpAddr;  // UDP packet source IP address
	  WORD   LocalPort;  // Port number UDP packet was sent to
	  WORD   SrcPort;    // UDP packet source port number
	  char   recData[80];
	  char field[20];
	  int result;
	  tm setdt;

	  int len = recvfrom( UdpFd, (BYTE *)recData, 80, &SrcIpAddr, &LocalPort, &SrcPort );
	  recData[len] = '\0';

	  //SysLogAddress = SrcIpAddr; // Set future SysLog output to direct to the IP that this message arrived from
	  SysLogAddress = SrcIpAddr.Extract4(); // Set future SysLog output to direct to the IP that this message arrived from

	  sprintf(gLogBuf, "\r\nReceived a UDP packet with %d bytes from :", len );
      logMessage(LOGPRIOLOW, gLogBuf);
//	  ShowIP( SrcIpAddr );
	  snShowIP(gLogBuf, 16, SrcIpAddr); // Dump an IP address in ASCII IP string format to char
      logMessage(LOGPRIOLOW, gLogBuf);
	  sprintf(gLogBuf, "\r\n%s\r\n", recData );
      logMessage(LOGPRIOMED, gLogBuf);
//      OSTimeDly(1); // To allow the message to be sent before a re-boot (for the R command)
	  if (recData[0] == 'A')  sysLogPrio = LOGPRIOALLWAYS;
	  if (recData[0] == 'L')  sysLogPrio = LOGPRIOLOW;
	  if (recData[0] == 'M')  sysLogPrio = LOGPRIOMED;
	  if (recData[0] == 'H')  sysLogPrio = LOGPRIOHIGH;
	  if (recData[0] == 'U')  sysLogDest = LOGOPUDP;
	  if (recData[0] == 'S')  sysLogDest = LOGOPSTDIO;
	  if (recData[0] == 'O')  sysLogDest = LOGOPOFF;
	  if (recData[0] == 'T')  {
		result = -1;
		if (ExtractPostData("dayofmonth", recData, field, 20) != -1) result = atoi(field);
		setdt.tm_mday = result;
		result = -1;
		if (ExtractPostData("month", recData, field, 20) != -1) result = atoi(field);
		setdt.tm_mon = result - 1;
		result = -1;
		if (ExtractPostData("year", recData, field, 20) != -1) result = atoi(field);
		setdt.tm_year = result - 1900;
		result = -1;
		if (ExtractPostData("hour", recData, field, 20) != -1) result = atoi(field);
		setdt.tm_hour = result;
		result = -1;
		if (ExtractPostData("min", recData, field, 20) != -1) result = atoi(field);
		setdt.tm_min = result;
		result = -1;
		if (ExtractPostData("sec", recData, field, 20) != -1) result = atoi(field);
		setdt.tm_sec = result;
		result = -1;
		if (ExtractPostData("wday", recData, field, 20) != -1) result = atoi(field);
		setdt.tm_wday = result;
		setdt.tm_yday = 0;
		setdt.tm_isdst = 0;
		sprintf(gLogBuf, "Date Time: %d/%02d/%d  %02d:%02d:%02d  Weekday %d",
				setdt.tm_mday,
				setdt.tm_mon+1,
				setdt.tm_year+1900,
				setdt.tm_hour,
				setdt.tm_min,
				setdt.tm_sec,
				setdt.tm_wday);
	    logMessage(LOGPRIOHIGH, gLogBuf);
		result = MCF541X_RTCSetTime(  setdt ); //Set time and date in RTC of MOD54415
	  }
	  if (recData[0] == 'R') ForceReboot();
	  if (recData[0] == 'P')  {
		sprintf(gLogBuf, "NV_Params.OptionsNV:[%d, %d, %d, %d, %d, %c, %d, %.3f]\r\n",
			NV_Params.OptionsNV.logPort,
			NV_Params.OptionsNV.logLevel,
			NV_Params.OptionsNV.testpTol,
			NV_Params.OptionsNV.diffpAbort,
			NV_Params.OptionsNV.diffpNegAlarm,
			NV_Params.OptionsNV.LT_ID,
//			NV_Params.OptionsNV.LTB_ID,
			NV_Params.OptionsNV.option1,
			NV_Params.OptionsNV.option2
			);
	    logMessage(LOGPRIOHIGH, gLogBuf);
	    snShowIP(gLogBuf, 20, NV_Params.OptionsNV.logIP);
	    logMessage(LOGPRIOHIGH, gLogBuf);
	  }
	  if (recData[0] == 'V') NV_Params.OptionsNV.option1 = 1; //Force re-init of just the Options Params
	  // Must Save the NV Params first via the web interface!!
	  if (recData[0] == 'I')  {
		//ExtractPostData Returns -1 if no data found, 0 if found but empty, otherwise the number of chars xfered to field
		if (ExtractPostData("id", recData, field, 20) > 0) {  //Extract the Part ID or Serial Number
		  len = strlen(field);
	      if ((len < 20) && (len >= NV_Params.OptionsNV.option5)) {
			strcpy(textInput, field);
	        barCodeOk = true;
	      }
	      else {
		    strcpy(textInput, "None"); //No ID Data
		    barCodeOk = false;
	      }
		}
		else {
		  strcpy(textInput, "None"); //No ID Data
		  barCodeOk = false;
		}
	  }
	  if (recData[0] == 'G')  {
		result = -9999; //Indicates that there was either no prog data or it was present but empty
		if (ExtractPostData("prog", recData, field, 20) > 0) result = atoi(field); //Extract the Program number
		if (result == -1) {
			webReset = true; //Prog number -1 means Reset
		}
		else {
			if (result == -9999) result = 0; //If no prog data then default to program 0
			if ((result >= 0) && (result <= 15)) {
				appliedConfig = result;
				TestSetup = NV_Params.TestSetupNV[result];
				webStart = true;
			}
		}
	  }

   }
}


