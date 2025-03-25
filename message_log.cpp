/**
 * message_log.cpp
 * Log status and diagnostic messages to the appropriate output device
 * 		dependant on the msgLevel parameter and the current systemLogLevel
 * @author gavin.murray
 * @version MALT5.5
 * @date 2014-12-02
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "message_log.h"

BYTE sysLogPrio     = LOGPRIOHIGH; // Default High priority. Results will be sent to the log destination
BYTE sysLogDest     = LOGOPUDP;   // Default to UDP logging output - Destination UDP sysLog port (514)
//Use these for final settings
//BYTE sysLogPrio     = LOGPRIOLOW; // Default Low priority. When logging enabled ALL messages will be sent
//BYTE sysLogDest     = LOGOPOFF;   // Default to no logging output - Destination off

void logMessage(BYTE msgLevel, char * buf )
{
    if ((sysLogDest == LOGOPUDP) && (sysLogPrio >= msgLevel)){
    	SysLog("%s", buf );
//		iprintf("UDP: %s", buf );
    }

    if ((sysLogDest == LOGOPSTDIO) && (sysLogPrio >= msgLevel))
    	iprintf("%s", buf );
}

