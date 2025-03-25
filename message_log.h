/**
 * message_log.h
 * Log status and diagnostic messages to the appropriate output device
 * 		dependant on the msgLevel parameter and the current systemLogLevel
 * @author gavin.murray
 * @version MALT5.5
 * @date 2014-12-02
 */

#ifndef MESSAGE_LOG_H_
#define MESSAGE_LOG_H_

// Log priority levels - each message has a logging priority
// and the system has a current priority level
// Message is only sent if the
#define LOGPRIOALLWAYS		0 // Message always sent - ?? I think this is wrong! To always be sent I think the Priority needs to be 99
#define LOGPRIOHIGH			1 // Only High Priority Messages
#define LOGPRIOMED			2 // Only High and Medium Priority Messages
#define LOGPRIOLOW			3 // Low Medium and High Priority Messages

// Log output to destination either
// Standard IO (RS232)
// network via UDP to socket 514 (the standard syslog port)
#define LOGOPOFF			0
#define LOGOPSTDIO			1
#define LOGOPUDP			2



// The system assigns the current log level required
extern BYTE sysLogPrio;
extern BYTE sysLogDest;

void logMessage(BYTE msgLevel, char * buf);

#endif /* MESSAGE_LOG_H_ */
