/**
 * htmlvar.h
 * Callback function declarations
 * Note: Called back functions with parameters must be declared in htmlvar.h with format
 * 		const char * function(int sock, int v, ...)
 * @author gavin.murray and leszek.zarzycki
 * @version MALT5.5
 * @date 2014-11-06
 */

#ifndef HTMLVARS_H_
#define HTMLVARS_H_

#include <constants.h>
#include <system.h>
#include <startnet.h>


const char * getHtmlParam(int fd, int v);

const char * getHtmlFixtureParam(int fd, int v); //2016-06-23 fixture control

const char * getHtmlConfigLabel(int fd, int v);

const char * getHtmlCalib(int fd, int v);

const char * getHtmlOption(int fd, int v);

const char * getHtmlTimestamp(int sock,int v);

const char * getHtmlTPCount(int sock,int v);

const char * getHtmlTP(int sock,int v);

const char * getHtmlDPCount(int sock,int v);

const char * getHtmlDP(int sock,int v);

const char * getHtmlPassFail(int sock, int module, int circle);

const char * getHtmlDone(int sock, int module);

const char * getHtmlError(int sock, int module);

const char * getHtmlResStatus(int sock,int v);

const char * getHtmlResTP(int sock,int v);

const char * getHtmlResDP(int sock,int v);

const char * getHtmlResLR(int sock,int v);

const char * getHtmlResAZ(int sock,int v);

const char * getHtmlResTimeStamp(int sock,int v);

const char * getHtmlTestState(int sock, int module);

const char * getHtmlBusy(int sock, int column);

//supporrt for graph - LZ nov 2019
const char * getHTMLRunningValues(int sock,int v);

#endif /*HTMLVARS_H_*/
