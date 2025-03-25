/**
 * datacapture.h
 * Interface to capture MALT running values and latest test results.
 * Declarations the RingLog circular arrays (datalog[2])...
 * 		...which are used in the ajaxCallbackFunctions to access running values
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT1.1
 * @date 2015-01-13
 */

#include "datalog.h"
#include "datacapture.h"
#include "MLT_globalExterns.h"


static RunningValues logBufferA[LOG_SIZE];
static RunningValues logBufferB[LOG_SIZE];

RingLog datalog[2]; //datalog[0] = module A //datalog[1] = module B

/**
 * @brief LogTestData
 * Adds the latest test data and results to the datalogs
 */

void LogTestData() {
	RunningValues tmp;

	tmp.diffp = dataIn.diffp;
	tmp.diffpCount = dataIn.diffpCount;
	tmp.testp = dataIn.testp;
	tmp.testpCount = dataIn.testpCount;
	datalog[0].Add( &tmp );
//	tmp.diffp = dataInB.diffp;
//	tmp.diffpCount = dataInB.diffpCount;
//	tmp.testp = dataInB.testp;
//	tmp.testpCount = dataInB.testpCount;
	datalog[1].Add( &tmp );
	datalog[0].setResults(LT_Results);
//	datalog[1].setResults(LTB_Results);

}

/**
 * @brief InitialiseDataLog
 * Initialises the datalog instances and posts an initial value.
 *
 */
void InitialiseDataLog() {
	datalog[0].init( logBufferA, LOG_SIZE );
	datalog[1].init( logBufferB, LOG_SIZE );

 	LogTestData(); //post an initial set of values in the datalog - avoid null references
}
