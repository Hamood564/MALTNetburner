/*
 * options.cpp
 *
 *  Created on: Aug 2020
 *      Author: leszek.zarzycki
 */
//#include <basictypes.h>
#include <stdlib.h>
#include <string.h> //strcpy, strlen, strcmp
#include <utils.h> //AcsiiToIP, snShowIP
#include <stdio.h>
#include "MLT_globalTypes.h"
#include "options.h"

extern NV_Params_Struct NV_Params;

//k is value of option field enum
//v is the value as a string
void setOption(int k, char *v){
	//belt and braces. Make sure test index is ignored
	setOption((OptionField) (k%1024), v);
}

void setOption(OptionField k, char *v) {
	switch (k) {
	case NOOPTION: break;
	case logPort: setLogPort(atoi(v)); break;
	case logIP: setLogIP(AsciiToIp4(v)); break;
	case logLevel: setLogLevel(atoi(v)); break;
	case testpTol: setTestpTol(atoi(v)); break;
	case diffpAbort: setDiffpAbort(atoi(v)); break;
	case diffpNegAlarm: setDiffpNegAlarm(atoi(v)); break;
	case LT_ID: setLT_ID(v[0]); break;
	case startenable: setStartEnable(( atoi(v)!=0 )); break;
	case option1: setOption1(atoi(v)); break;
	case option2: setOption2(atof(v)); break;
	case option3: setOption3(atoi(v)); break;
	case option4: setOption4(atof(v)); break;
	case option5: setOption5(atoi(v)); break;
	case option6: setOption6(atof(v)); break;
	case password1: setPassword1(v); break;
	case password2: setPassword2(v); break;
	default: ; //unknown option
	}
}

char bufo[32] = "\0";  // (LZ) make this a var parameter of function instead???

/* Returns the option field's value as a string */
char* getOption(OptionField k){
	switch (k) {
	case NOOPTION: break;
	case logPort: sprintf(bufo, "%d", getLogPort()); break;
	case logIP: snShowIP(bufo, 25, getLogIP()); break;
	case logLevel: sprintf(bufo, "%d", getLogLevel()); break;
	case testpTol: sprintf(bufo, "%d", getTestpTol()); break;
	case diffpAbort: sprintf(bufo, "%d", getDiffpAbort()); break;
	case diffpNegAlarm: sprintf(bufo, "%d", getDiffpNegAlarm()); break;
	case LT_ID: sprintf(bufo, "%c", getLT_ID()); break;
	case startenable: sprintf(bufo, "%d", getStartEnable()); break;
	case option1: sprintf(bufo, "%d", getOption1()); break;
	case option2: sprintf(bufo, "%f", getOption2()); break;
	case option3: sprintf(bufo, "%d", getOption3()); break;
	case option4: sprintf(bufo, "%f", getOption4()); break;
	case option5: sprintf(bufo, "%d", getOption5()); break;
	case option6: sprintf(bufo, "%f", getOption6()); break;
	case password1: getPassword1(bufo); break;
	case password2: getPassword2(bufo); break;
	default: ; //unknown option
	}
	return bufo;
}

void getAllOptions(char * buf) {
	buf[0]='\0';
	strcpy(buf,"O{");

	char temp[32];
	for (int i=1; i<=16; i++) {
		sprintf(temp, "%d:%s,", i,getOption((OptionField) i) );
		strcat(buf, temp);
	}
	buf[strlen(buf)-1]='}';
	//iprintf("returning buf = --%s--\r\n", buf);
}

void setLogPort(int logPort) {
	NV_Params.OptionsNV.logPort = logPort;
}

void setLogIP(IPADDR4 commsIP) {
	NV_Params.OptionsNV.logIP = commsIP;
}

void setLogLevel(int logLevel) {
	NV_Params.OptionsNV.logLevel = logLevel;
}

void setTestpTol(int testpTol) {
	NV_Params.OptionsNV.testpTol = testpTol;
}

void setDiffpAbort(int diffpAbort) {
	NV_Params.OptionsNV.diffpAbort = diffpAbort;
}

void setDiffpNegAlarm(int diffpNegAlarm) {
	NV_Params.OptionsNV.diffpNegAlarm = diffpNegAlarm;
}

void setLT_ID(char LT_ID) {
	NV_Params.OptionsNV.LT_ID = LT_ID;
}

void setStartEnable(bool enable) {
	NV_Params.OptionsNV.startenable = enable;
}

void setOption1(int option) {
	NV_Params.OptionsNV.option1 = option;
}

void setOption2(float option) {
	NV_Params.OptionsNV.option2 = option;
}

void setOption3(int option) {
	NV_Params.OptionsNV.option3 = option;
}

void setOption4(float option) {
	NV_Params.OptionsNV.option4 = option;
}

void setOption5(int option) {
	NV_Params.OptionsNV.option5 = option;
}

void setOption6(float option) {
	NV_Params.OptionsNV.option6 = option;
}

void setPassword1(char* password) {
	strcpy(NV_Params.OptionsNV.password1, password);
}

void setPassword2(char* password) {
	strcpy(NV_Params.OptionsNV.password2, password);
}


int getLogPort() {
	return NV_Params.OptionsNV.logPort;
}

IPADDR getLogIP() {
	return NV_Params.OptionsNV.logIP;
}

int getLogLevel(){
	return NV_Params.OptionsNV.logLevel;
}

int getTestpTol() {
	return NV_Params.OptionsNV.testpTol;
}

int getDiffpAbort() {
	return NV_Params.OptionsNV.diffpAbort;
}

int getDiffpNegAlarm() {
	return NV_Params.OptionsNV.diffpNegAlarm;
}

char getLT_ID() {
	return NV_Params.OptionsNV.LT_ID;
}

bool getStartEnable() {
	return NV_Params.OptionsNV.startenable;
}

int getOption1() {
	return NV_Params.OptionsNV.option1;
}

float getOption2() {
	return NV_Params.OptionsNV.option2;
}

int getOption3() {
	return NV_Params.OptionsNV.option3;
}

float getOption4() {
	return NV_Params.OptionsNV.option4;
}

int getOption5() {
	return NV_Params.OptionsNV.option5;
}

float getOption6() {
	return NV_Params.OptionsNV.option6;
}

void getPassword1(char * buf) {
	strcpy(buf, NV_Params.OptionsNV.password1);
}

void getPassword2(char * buf) {
	strcpy(buf, NV_Params.OptionsNV.password2);
}



