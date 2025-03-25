/*
 * tcp_api.cpp
 *
 *  Created on: 28 Apr 2020
 *      Author: leszek.zarzycki
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strcpy, strlen, strcmp
#include <http.h>
#include <nettypes.h> 	//IPADDR
#include <iosys.h>		//read(fd,..)
#include <tcp.h>
#include <utils.h>
#include <effs_fat/multi_drive_mmc_mcf.h>
#include "ftp/FileSystemUtils.h"
#include "tcp_server.h"

#include "cmdprocessor.h"

/* Any address */
//#define INADDR_ANY (0)
#define RX_BUFSIZE (4096)

char RXBuffer[RX_BUFSIZE]; //used to capture input string

/* Convert IP address to a string */
void IPtoString(IPADDR4  ia, char *s)
{
    PBYTE ipb= (PBYTE) &ia;
    siprintf(s, "%d.%d.%d.%d",(int)ipb[0],(int)ipb[1],(int)ipb[2],(int)ipb[3]);
}


/* getRTXString reads incoming bytes into a buffer until '\r' or '\n' is encountered.
*  Returns the buffer terminated with null, i.e. '\0' in place of the \r or \n.
*  i.e. returns a string.
*  @ author TQC_LZ
*/
int getRXString(int fdnet) {
	int n = 0; 	//number of char read
	int zeroCount=0;
   	do {
		int n1 = read( fdnet, RXBuffer+n, RX_BUFSIZE-n ); //Note: blocks
		//iprintf("---%d\r\n",n1);
		if (n1>0) {
			//locate first \r or \n in RXBuffer[n .. n+n1]
			int i=n;
			while (i<n+n1 && RXBuffer[i]!='\r' && RXBuffer[i]!='\n') i++;
			n = i;
			//iprintf("+++%d\r\n",n);
		}
        else {
        	zeroCount++;
        	iprintf("No bytes: %d; ", zeroCount);
        }
	} while (n<RX_BUFSIZE && RXBuffer[n]!='\r' && RXBuffer[n]!='\n' && zeroCount<100);

   	//replace terminal character with '\0'
	if (n<RX_BUFSIZE) RXBuffer[n] = '\0'; //else RXBuffer[RX_BUFSIZE-1] = '\0';
	iprintf( "***\nRead %d bytes: %s\n", n, RXBuffer);

	return zeroCount<100 ? n :-1;
}


void tcpServerTask( void *pdata ) {
	iprintf("\r\ntcp task started\r\n");
//---------------
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
//-----------------

    int ListenPort = (int) pdata;

    // Set up the listening TCP socket
    int fdListen = listen(INADDR_ANY, ListenPort, 5);

    if (fdListen > 0)
    {
        IPADDR	client_addr;
        WORD	port;

        while(1)
        {
			// The accept() function will block until a TCP client requests
			// a connection. Once a client connection is accepting, the
			// file descriptor fd is used to read/write to it.
			iprintf( "Waiting for connection on port %d...\n", ListenPort );
			int fdnet = accept(fdListen, &client_addr, &port, 0);

			iprintf("Connected to: ");
			ShowIP(client_addr);
			iprintf(":%d\n", port);

			char ip[20];
			IPtoString(EthernetIP, ip);
			siprintf(RXBuffer,
				"MALT TCP Server\r\n" \
				"Connected to IP Address %s, port %d\r\n", ip, TCP_LISTEN_PORT);
			writestring(fdnet, RXBuffer);


			while (fdnet > 0)
            {
                /* Loop while connection is valid. The read() function will return
                   0 or a negative number if the client closes the connection, so we
                   test the return value in the loop.*/
                int n = 0;
                do
                {
                	n = getRXString(fdnet);
                	//copy buffer to local string
                	char str[256];	//review string length - (max command size)
                	strncpy(str, RXBuffer, 256);
                	iprintf("Received: %s, length=%lu\r\n", str, strlen(str));

                	char * response = processCommand(str);
                	strcat(response, "\r\n");

                	//iprintf("%s", response);
                	iprintf("response %lu bytes\r\n", strlen(response));
                	writeall(fdnet, response, strlen(response));
                	//writestring(fdnet, "\r\n");

                	OSTimeDly( TICKS_PER_SECOND / 4 );
                }
                while ( n>=0 && RXBuffer[0]!='q' && RXBuffer[0]!='Q');

    			siprintf(RXBuffer,
    					"Closing connection to IP Address %s, port %d\r\n",
    					ip, TCP_LISTEN_PORT);
    			writestring(fdnet, RXBuffer);

                iprintf("Closing client connection: ");
                ShowIP(client_addr);
                iprintf(":%d\n", port);
                close(fdnet);
                fdnet = 0;
            }
        } // while(1)
    } // while listen ---- LZ attention

}

