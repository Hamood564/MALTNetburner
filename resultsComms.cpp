/**
 * resultsComms.h
 * Implementation of task to capture and communicate results over TCP/UDP.
 * @author gavin.murray
 * @version MALT5.5
 * @date 2014-12-02
 */

#include <utils.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdlib.h>
#include <bsp.h>
#include <constants.h>
#include <iosys.h>
//#include <ucos.h>
//#include <cfinter.h>

#include <tcp.h>
#include <udp.h>
#include <effs_fat/multi_drive_mmc_mcf.h>
#include "ftp/FileSystemUtils.h"

#include "message_log.h"
#include "MLT_globalExterns.h"

#include "graph/graphData.h" //LZ 2020-09


// Test sequence text states - index with stepCountA-B
// Master copy is in ajaxCallbackFunctions.cpp
static char* resultText[20] = {
		"No Result", "Pass", "Fail", "Gross Fail", "Fail - Test Pres Low",  //Result 0 to 4
		"No Result", "No Result", "No Result", "Pass - No Display", "Fail - No Display",  //Result 5 to 9
		"Diff Pres High", "Test Reset", "Fail - Test Pres High", "Reference Error", "No Result",  //Result 10 to 14
		"Dig IPs NOk", "Fixture NOk", "No Result", "No Result", "No Result"  //Result 15 to 19
};

/*-------------------------------------------------------------------
Init FIFO members
------------------------------------------------------------------*/
void initResultsFifoMembers()
{
	for ( int i = 0; i < RESULTSFIFOSIZE; i++ )
	{
		ResultsFIFO[i].LT_ID = 'A'; //Not needed! Done when used
		ResultsFIFO[i].free = true; //Init to free ie available
	}
}

/*-------------------------------------------------------------------
Find free FIFO structure.
ResultsFIFO is an array of structures with a structure element that
indicates if a particular structure is being used. This function
will return the index of the first free structure array member
or -1 if all members are being used.
------------------------------------------------------------------*/
int findFreeResultsFifoStruct()
{
	int index = -1;
	int i = 0;
	do {
		if ( ResultsFIFO[i].free )
			index = i;
		else
			i++;
	} while ( ( i < RESULTSFIFOSIZE ) && ( index == -1 ) );
	return index;
}

/*
 *  Send via UDP to a listening ethernet UDP server.
 *  Message disappears into the ether if there is no server listening
 */
int send_UDP(char * msgUDP)
{
    int DestPort;
    int n;
	IPADDR DestIp;
	char buf[50];
	DestPort = NV_Params.OptionsNV.logPort;
	DestIp = NV_Params.OptionsNV.logIP;
//	DestIp = SysLogAddress; //Send to Options IP OR SysLogAddress (should be the same!)
// Create a UDP socket for sending/receiving (local port same as remote port)
// int CreateTxUdpSocket(IPADDR send_to_addr, WORD remote_port, WORD local_port).
    int UdpFd = CreateTxUdpSocket( DestIp, DestPort, DestPort );
    if ( UdpFd <= 0 )
    {
       n = -1; //Return error code
       sprintf(buf, "Error %d Creating UDP Socket: %d\r\n", UdpFd, DestPort);
       logMessage(LOGPRIOLOW, buf);
    }
    else
    {
        iprintf("%s\r\n", msgUDP);
    	n = sendto( UdpFd, (BYTE *)msgUDP, strlen(msgUDP), DestIp, DestPort );
        close( UdpFd );
    }
    return n;
}


/*
 *  Send via TCP to a listening ethernet TCP server
 *  3 seconds timeout if there is no server listening
 */
int resultsCSV_TCP(char * resTCP)
{
    int DestPort;
    int n;
	IPADDR DestIp;
	char buf[50];
	DestPort = NV_Params.OptionsNV.logPort;
	DestIp = NV_Params.OptionsNV.logIP;
	DestIp = SysLogAddress; //Send to Options IP OR SysLogAddress (should be the same!)
//	DestIp = AsciiToIp( "192.168.115.209" );
	int fd = connect( DestIp, 0, DestPort, TICKS_PER_SECOND * 3);
    if (fd < 0)
    {
    	n = -1; //Return error code
    	sprintf(buf, "Connection to TCP port %d failed\r\n", DestPort);
    	logMessage(LOGPRIOLOW, buf);
    }
    else
    {
       	n = write( fd, resTCP, strlen(resTCP) );
        close( fd );
    }
    return n;
}


/*
 *  Save results in SD memory card
 *
 */
void resultsMemoryCard(char * resTCP, int resYear, int resMon, int resDay) //Save in memory card
{
/*
 * Calculate the results folder name and results file name
 * mkdir the results folder
 * if no error - chdir to the results folder
 * if no error - open the results file
 * if no error - write the results (it already includes a CRLF)
 * close the file
 * chdir up one level
 *
 */
char sdFolderBuf[10];
char sdFileBuf[15];

    sprintf(sdFolderBuf, "Y%04d%02d", resYear, resMon); //Folder name
    sprintf(sdFileBuf, "%04d%02d%02d.csv", resYear, resMon, resDay); //Folder name
	int fold = f_mkdir(sdFolderBuf);
	if ((fold == F_NO_ERROR) || (fold == F_ERR_DUPLICATED))
	{
		if (! f_chdir(sdFolderBuf))
	    {
		    iprintf( "folder change to %s ok\r\n", sdFolderBuf);
//	          F_FILE* fp = f_open( "FileOnBoardSD.txt", "w+" );
	        F_FILE* fp = f_open( sdFileBuf, "a" );
		    if ( fp )
		    {
//		        const char * cp="This file written to SD card on the Module\r\n";
	            int n = f_write( resTCP, 1, strlen( resTCP ), fp );
//                iprintf( "Wrote %d bytes: %s", n, resTCP);
		        f_close(fp);
		    }
		    f_chdir("..");
//	        DumpDir();
	    }
	    else
	    {
		    iprintf( "chdir failed: \r\n" );
//		      DisplayEffsErrorCode( rv );
	    }
	}
	else
	{
		iprintf( "mkdir failed: \r\n" );
//		   DisplayEffsErrorCode( rv );
	}

} //resultsMemoryCard()


/*-------------------------------------------------------------------
resultsCommsTask - Wait for messages posted in the ResultsFIFO
The messages will be in the format of the TestResults structure
------------------------------------------------------------------*/
void resultsCommsTask( void *pdata )
{
	char buf[128];
	char bufTCP[128];
	int textIndex;

	int rv = f_enterFS(); //needed in every task that uses the EFFS file system (SD Card)
//    if ( rv != F_NO_ERROR )
//    {
//          iprintf( "Results Comms Task f_enterFS failed: %d\r\n", rv);
////      DisplayEffsErrorCode( rv );
//    }
    rv = f_chdrive( EXT_FLASH_DRV_NUM ); //Set SD Card current drive number for this task
    if ( rv == F_NO_ERROR )
    {
       iprintf( "Drive change successful - Drive No: %d\r\n", EXT_FLASH_DRV_NUM);
    }
    else
    {
       iprintf( "Drive change failed: ");
 //      DisplayEffsErrorCode( rv );
    }

	while ( 1 )
	{
		TestResults *pData = ( TestResults * ) OSFifoPend( &ResultsOSFIFO, TICKS_PER_SECOND * 5 );
		if ( pData == NULL )
		{
//			iprintf( " Timeout in OSFifoPend(), waiting for next FIFO Post\r\n" );
//			SysLog( " Timeout in OSFifoPend(), waiting for next FIFO Post\r\n" );
		}
		else
		{
// Message received - so process it and then release it by setting its free field to true
	      if (pData->msgID == 0)
	      {  //FIFO message is a test result to be saved and sent by UDP SysLog and TCP to PC logger
			textIndex = pData->status; //Check for result status being out of range of the text array
	        if ((textIndex < 0) || (textIndex > 19)) textIndex = 0;
	        sprintf(buf, "Part ID %s Fixture ID %c Prog %2d Result %2d TestP=%7.1f DiffP=%6.1f Leak Rate=%7.2f %s\r\n",
	        		 pData->msg,
	        		 pData->LT_ID,
	        		 pData->params,
	        		 pData->status,
	        		 float(pData->testp)/10.0,
	        		 float(pData->diffp)/10.0,
	        		 float(pData->leakRate)/100.0,
	        		 resultText[textIndex]);
	        logMessage(LOGPRIOHIGH, buf); //Send via UDP to a listening ethernet UDP server
	        sprintf(bufTCP, "%d,%d,%d,%d,%d,%d,\"%s\",%c,%d,%d,%5.1f,%5.1f,%5.2f,\"%s\"\r\n",
	        		 pData->dateTime.tm_year +1900,
	        		 pData->dateTime.tm_mon + 1,
	        		 pData->dateTime.tm_mday,
	        		 pData->dateTime.tm_hour,
	        		 pData->dateTime.tm_min,
	        		 pData->dateTime.tm_sec,
	        		 pData->msg,
	        		 pData->LT_ID,
	        		 pData->params,
	        		 pData->status,
	        		 float(pData->testp)/10.0,
	        		 float(pData->diffp)/10.0,
	        		 float(pData->leakRate)/100.0,
	        		 resultText[textIndex]);
//Save in memory card
	        resultsMemoryCard(bufTCP, pData->dateTime.tm_year +1900, pData->dateTime.tm_mon + 1, pData->dateTime.tm_mday);
	        resultsCSV_TCP(bufTCP); //Send via TCP to a listening ethernet TCP server

	        //LZ 2020 Sep
	        if (NV_Params.OptionsNV.option3) saveGraphData(pData->dateTime);
	      }
	      else if (pData->msgID == 10)
	      {  // Message is Start of Test
//	    	sprintf(buf, "Start ID %c Prog %2d\r\n", pData->LT_ID, pData->params);
			sprintf(buf, "%d,%c,%d,0,%d,%7.1f,%6.1f\r\n",
					pData->msgID,
					pData->LT_ID,
					pData->params,
					pData->status,
					float(pData->testp)/10.0,
					float(pData->diffp)/10.0);
	    	send_UDP(buf);
	      }
	      else if (pData->msgID == 11)
	      {  // Message is Pressure data
//	    	sprintf(buf, "Data ID %c Prog %2d Step %d Time %d TestP %7.1f DiffP %6.1f\r\n",
//	    			pData->LT_ID,
//	    			pData->params,
//	    			pData->diffpAZ,
//	    			pData->status,
//	    			float(pData->testp)/10.0,
//	    			float(pData->diffp)/10.0);
	    	sprintf(buf, "%d,%c,%d,%d,%d,%7.1f,%6.1f\r\n",
	    			pData->msgID,
	    			pData->LT_ID,
	    			pData->params,
	    			pData->diffpAZ,
	    			pData->status,
	    			float(pData->testp)/10.0,
	    			float(pData->diffp)/10.0);
	    	send_UDP(buf);
	      }
		  pData->free = true;
		}
//	   OSTimeDly( TICKS_PER_SECOND / 20 );
	}
}


