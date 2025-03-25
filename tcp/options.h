/*
 * options.h
 *
 *  Created on: 10 May 2016
 *      Author: leszek.zarzycki
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <nettypes.h>

enum OptionField {
	NOOPTION,			//0
	logPort,			//1 int Port for TCP and UDP comms - Local port for UDP listener
	logIP,				//2 IPADDR Default IP Address to log to (overridden by UDP Comms)
	logLevel,			//3 int Logging message level to send
	testpTol,			//4 Percentage +/- tolerance for Test Pressure
	diffpAbort,			//5 int Abort test at this diff pressure
	diffpNegAlarm,		//6 int Alarm at this negative diffp
	LT_ID,				//7 char Identifier for Leak Test A
	startenable,		//8 bool Fixture is ready at start of test sequence
	option1,			//9 int
	option2,			//10 float
	option3,			//11 int
	option4,			//12 float
	option5,			//13 int
	option6,			//14 float
	password1,			//15 char[25] Authentication to access configuration web pages
	password2			//16
};

/*
struct OptionStruct {
	OptionField	option;				// option field identifier
	char		name[32];			// readable field name
	void		(*set) (char * v); 	// function pointer to the set option to value v.
};
*/

/* Set an option field with the given value.
 * The value (string) is converted to the type for the field.
 */
void setOption(int k, char *value);
void setOption(OptionField, char *v);

char* getOption(OptionField k);
void getAllOptions(char * buf);

//result comms and UDP input
void setLogPort(int logPort);
void setLogIP(IPADDR4 logIP);

/* System log (broadcast)
 * one of 0=LOGPRIOALWAYS, 1=LOGPRIOHIGH, 2=LOGPRIOMED or 3=LOGPRIOLOW.*/
void setLogLevel(int logLevel);

//destination: LOGOPOFF 0, LOGOPSTDIO 1, LOGOPUDP 2, LOGOPSTDIO|LOGOPUDP 3
//void setLogDestination(int destination);

void setTestpTol(int testpTol);
void setDiffpAbort(int diffpAbort);
void setDiffpNegAlarm(int diffpNegAlarm);
void setLT_ID(char LT_ID);
void setStartEnable(bool enable);
//void setUseEtoP(bool useEtoP);
void setOption1(int option);
void setOption2(float option);
void setOption3(int option);
void setOption4(float option);
void setOption5(int option);
void setOption6(float option);
void setPassword1(char* password);
void setPassword2(char* password);
//void setUseEtoP(bool useEtoP);

//results & comms
int getLogPort();
IPADDR getLogIP();

//system log (messages - debug)
int getLogLevel();
//int getLogDestination();
//bool getSysLogUDP();
//bool getSysLogStdIO();


int getTestpTol();
int getDiffpAbort();
int getDiffpNegAlarm();
char getLT_ID();
bool getStartEnable();
bool usingEtoP();
int getOption1();
float getOption2();
int getOption3();
float getOption4();
int getOption5();
float getOption6();
void getPassword1(char * buf);
void getPassword2(char * buf);

#endif /* OPTIONS_H_ */
