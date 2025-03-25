/**
 * serial_input.cpp
 * Implementation of Serial RS232 Input task.
 * @author gavin.murray
 * @version Single_MALT1.1
 * @date 2020-03-05

 */

// #include "predef.h"
#include <utils.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
//#include <stdlib.h>
//#include <bsp.h>
#include <serial.h>
//#include <stdio.h>
#include <iosys.h>

#include <effs_fat/multi_drive_mmc_mcf.h>
#include "ftp/FileSystemUtils.h"


#include "mcf5441x_rtc.h"  //Local 54415 RTC support
#include "message_log.h"
#include "MLT_globalExterns.h"


void serialInputTask( void *pdata )
{
// Check for serial data received - typically from a Barcode reader
   iprintf( "serialReaderTask listening for Data\r\n");
   OSTimeDly( 20 ); //Pause to let other debug messages to be output before accepting any input

//Example one - using OpenSerial - see Netburner Example C:\nburn\examples\StandardStack\serial\SerialToSerial
//Example two - see Netburner Example C:\nburn\examples\MOD5441X\cantoserial

// Used simple fgets function. See  C:\nburn\docs\NetworkProgrammersGuide\NNDKProgMan.pdf  15.1.3 Mailbox example

   while (1)
   {
	  static char barcode[50];

	  fgets(barcode, 40, stdin); // block until user enters a string
	  int len = strlen(barcode);
//	  iprintf("Received %d bytes\r\n", len);
// Remove any trailing \r or \n (CR LF)
	  if ( len >= 2 )
	  {
	    for (int i=0; i < 2; i++)
	    {
	      if ( ( barcode[len-i] == '\r' ) || ( barcode[len-i] == '\n' ) )
	    	  barcode[len-i] = '\0';
	    }
	  }
	  len = strlen(barcode);
      iprintf("Serial (Barcode) Data: %s  %d characters long\r\n", barcode, len);
// Received text is in local variable barcode.
// Must be minimum characters length set in Options to be valid
      if (len >= NV_Params.OptionsNV.option5)
      {
        strcpy(textInput, barcode);
        barCodeOk = true;
      }
      else
      {
	    strcpy(textInput, "None"); //V2.8_20200305  BAE8 - Init Serial Data Input
	    barCodeOk = false;
      }

//	  OSTimeDly( 5 );

   }

}


