/**
 * datacapture.h
 * Interface to capture MALT running values and latest test results.
 * .cpp has the declarations for the RingLog circular arrays (datalog[2])...
 * 		...which are used in the ajaxCallbackFunctions to access running values
 * @author gavin.murray and leszek.zarzycki
 * @version SingleMALT1.1
 * @date 2015-01-13
 */

#ifndef  __DATAGENERATOR_H
#define  __DATAGENERATOR_H

#define LOG_SIZE 20

void InitialiseDataLog();
void LogTestData();


#endif   /* ----- #ifndef __DATAGENERATOR_H  ----- */
