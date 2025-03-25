/*
 * graphData.h
 *
 *  Created on: 19 Nov 2019 (originally runningValues.h Oct 2016
 *      Author: leszek.zarzycki
 *
 * Stores the latest running values. Up to MAXRV records.
 * The most recent n values can be retrieved in a string buffer.
 * Used by web callback for graph data.
 */

#ifndef GRAPHDATA_H_
#define GRAPHDATA_H_

#include<basictypes.h>
#include<time.h>

#define MAXRV  1000 //was 100
#define RVSIZE 25   //number of characters for a csv record of GraphData.

struct GraphData {
	int 	step;		//test sequence step number
	int 	testp;		//test pressure
	int 	diffp;		//diff pressure
	DWORD	duration;	//duration of test so far in milliseconds - based on gPitCount[1]
};

//adds a running value to the data store.
void pushGraphData(GraphData rv);
void pushGraphData(int step, int testp, int diffp, DWORD duration);

/* buf contains the most recent n triples of running values.
 * Also attach the index the current test (i), the test pressure (p) and tolerance% (l).
 * buf is formatted using JSON syntax, with fields (m,t,d) for
 * (duration in millis, testp, diffp).
 * i.e. buf is an array of records {rv:[{t:_,d:_,m:_}, {t:_,d:_,m:_}, ...], i:_, p:_, l:_}
 */
void pullGraphData(char * buf, int n);

//LZ Sep 2020
void startGraphData();
void endGraphData();
void getTestDataCSV(char * buf); //need to advise on size of buf...to do

void saveGraphData(tm dt);

#endif /* GRAPHDATA_H_ */
