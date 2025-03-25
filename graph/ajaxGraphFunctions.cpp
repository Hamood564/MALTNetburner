/*
 * dataCallbackFunctions.cpp
 *
 *  Created on: 14 Jun 2016
 *      Author: leszek.zarzycki
 */

#include <stdio.h>
#include <iosys.h>
#include <utils.h>

#include "graph/graphData.h"

extern int selectedConfig; //first defined in formcode.cpp

/* =======================================================================================
 * Call back function to populate the Test Parameters Configuration Table
 * Note: Called back functions with parameters must be declared in htmlvar.h, with format
 * 		const char * function(int sock, int v, ...)
 */


//Graph data - LZ nov 2019
const char * getHTMLRunningValues(int sock,int v) {
	//buf is an array of running values
	//JSON format {rv:[{t:_,d:_,m:_}, {t:_,d:_,m:_}, ...], i:_, p:_, l:_}
	char buf[v*48];
	pullGraphData(buf, v);
	writestring( sock, buf );
	return "\0"; //must return something
}


