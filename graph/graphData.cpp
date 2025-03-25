/*
 * runningValues.cpp
 * *  Created on: 18 Oct 2016
 *      Author: leszek.zarzycki
 *
 * Stores the latest running values. Up to MAXRV records.
 * The most recent n values can be retrieved in a string buffer.
 * Used by web callback for graph data.
 *
 * Sep 2020
 * Extended to support 'save' and 'get' last set of data from a MALT run.
 *
 */
#include<stdio.h>
#include<string.h>
#include<iosys.h>
#include "ftp/FileSystemUtils.h" //save to file
#include "MLT_globalExterns.h"
#include "MLT_globalTypes.h"
#include "graphData.h"

#include "message_log.h"

extern int testConfigA;  // 26/01/2015 test program to use during testing
extern NV_Params_Struct NV_Params;

GraphData stk[MAXRV]; //circular array for dynamic access of graph data
int top = 0; //write index to stk circular array

GraphData lastTest[MAXRV]; //data from latest MALT leak test run.
int gtop = 0; //write index to lastTest linear array.
bool testRunning = false;

struct GraphLTData {
	int		testIndex;		//index of current test configuration
	int		targetPressure; //leak test pressure
	int		tolerance;		// percentage %
};

char  buf[64];

/* Adds rv to the top of the stack. Implemented as a fixed size array.
 * Overwrites oldest record as it goes along.
 * Sep 2020 - writes to lastTest array
 * */
void pushGraphData(GraphData rv) {
	//write to circular array
	stk[top]=rv;
	top=(top+1) % MAXRV;

	//write to test array
	if (testRunning) {
		lastTest[gtop] = rv;
		gtop++;
	}
	sprintf(buf, "%d,%d,%d\r\n", rv.testp, rv.diffp, rv.duration);
	logMessage(LOGPRIOLOW, buf);
}

void pushGraphData(int step, int testp, int diffp, DWORD duration) {
	GraphData rv = {step, testp, diffp, duration};
	pushGraphData(rv);
}

/* buf contains the most recent n triples of running values.
 * Also attach the index the current test (i), the test pressure (p) and tolerance% (l).
 * buf is formatted using JSON syntax, with fields (m,t,d) for
 * (duration in millis, testp, diffp).
 * i.e. buf is an array of records {rv:[{t:_,d:_,m:_}, {t:_,d:_,m:_}, ...], i:_, p:_, l:_}
 */
void pullGraphData(char* buf, int n) {
	int first=((top+MAXRV)-n) % MAXRV;
	buf[0]='\0';

	char temp[48];
	sprintf(temp, "{\"rv\":[{\"t\":%d, \"d\":%d,\"m\":%lu}",
			stk[first].testp ,stk[first].diffp, stk[first].duration);
	strcat(buf,temp);

	for (int i=1; i<n; i++) {
		int idx = (first + i) % MAXRV;
		sprintf(temp, ",{\"t\":%d, \"d\":%d,\"m\":%lu}",
				stk[idx].testp ,stk[idx].diffp, stk[idx].duration);
		strcat(buf,temp);
	}
	strcat(buf,"]");

	//MALT3_V1.2 add on running test program, target test pressure, and tolerance
	int i=testConfigA;
	sprintf(temp, ",\"i\":%d,\"p\":%d,\"l\":%d}",
			i, NV_Params.TestSetupNV[i].testpressure, NV_Params.OptionsNV.testpTol);
	strcat(buf,temp);
}

void getTestDataCSV(char * buf) {

	buf[0] = '\0';
	//add a row for each data point (time,testp,diffp)
	char row[48];
	//Header row
	sprintf(row, "Step,Time(ms),Test Pres,Diff Pres\r");
	strcat(buf,row);

	int i = 0; //startIndex;
	while (i!=gtop) { //endIndex
		sprintf(row, "%d,%lu,%d,%d\r",
					lastTest[i].step,
					lastTest[i].duration,
					lastTest[i].testp,
					lastTest[i].diffp);
		strcat(buf,row);
		i++; //i = (i + 1) % MAXRV;
	}
}

/* Write graph data to memory card.
 * The data csv files are written to a 'day' folder within the month folder that holds
 * the daily results csv file. Name of data file DDhhmmss.csv matches the day and
 * time of the corresponding result record.
 */
void saveGraphData(tm dt) {

	char sdFolderBuf[16];
	char sdFileBuf[16];

	//results folder 'Y'YYYYMM e.g.Y202009
	strftime( sdFolderBuf, 16, "Y%Y%m", &dt );
	iprintf("sdfolder %s\r\n", sdFolderBuf);

	if (f_chdir(sdFolderBuf) == F_NO_ERROR) {
		iprintf( "folder change to %s ok\r\n", sdFolderBuf);

		//Graph data folder YYYYMMDD g.20200916
		strftime( sdFolderBuf, 16, "%Y%m%d", &dt );

		//Graph data file name DDhhmmss e.g. 16143205.csv day_hour_min_sec.csv
		strftime( sdFileBuf, 16, "%d%H%M%S.csv", &dt );

		int fold = f_mkdir(sdFolderBuf);
		if ((fold == F_NO_ERROR) || (fold == F_ERR_DUPLICATED)) {
			if (! f_chdir(sdFolderBuf))
			{
				iprintf( "folder change to %s ok\r\n", sdFolderBuf);
				F_FILE* fp = f_open( sdFileBuf, "w" ); //was "a"
				if ( fp ) {
					//build data array
					char buf[gtop*RVSIZE]; //should be plenty with some to spare
					getTestDataCSV(buf);
					int n = f_write( buf, 1, strlen( buf), fp );
					//iprintf( "Wrote %d bytes to file %s:\r\n", n, sdFileBuf);
					iprintf( "Wrote %d bytes to file:\r\n", n);
					f_close(fp);
				}
				f_chdir(".."); //back to results folder
			}
			else {
				iprintf( "chdir failed: \r\n" );
			}
		}
		else {
			iprintf( "mkdir failed: \r\n" );
		}
		f_chdir(".."); //back to root
	}
	else {
		iprintf("chdir %s failed\r\n", sdFolderBuf);
	}
	iprintf("save data done\r\n");
}

void startGraphData() {
	gtop = 0;
	testRunning = true;
}

void endGraphData() {
	testRunning = false;
}
