/*
 * cmdparser.cpp
 *
 *  Created on: 17 Jun 2020
 *      Author: leszek.zarzycki
 */

#include <basictypes.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iosys.h>
#include <system.h> // get and save user parameters
#include "ftp/FileSystemUtils.h" //save to file
#include <effs_fat/effs_utils.h>
#include <mcf5441x_rtc.h>  //Local 54415 RTC support
#include "datalog.h"
#include "cmdprocessor.h"
#include "testparams.h"
#include "options.h"
#include "calibrationdata.h"
#include "graph/graphData.h"
#include "MLT_globalExterns.h"
#include "MLT_globalLiterals.h" //PASSBIT, FAILBIT, DONEBIT, FAULTBIT
#include "importexport/userParameters.h" //for file export/import

extern int      appliedConfig; //Test Parameters selected
extern bool 	webStart;
extern bool 	webReset;
extern volatile unsigned short int stepCount; //stage of test
extern TestResults LT_Results;
extern RingLog datalog[2];


/* Process key value pairs in command list.
 * String tokenizer should be at the start of a list {k:v...}
 * @param processPair: function that do something with k,v pair,
 * i.e. setTestParam, setOption, SetCalibration
 * @param index: optional argument for test index, or calibration channel
 *
*/
//default function with index=0;
void parseKVPairs(void (*processPair)(int, char *), int index=0);

//overloaded function with optional parameter
void parseKVPairs(void (*processPair)(int, char *), int index) {
	printf ("(key,value) pairs:\n");
	char *pch;
	int k = 0;
	pch = strtok (NULL," :,{}"); //key
	while (pch != NULL)
	{
		//printf ("%s:%lu:%x\n",pch, strlen(pch), pch[0]);
		k = atoi(pch); //(Command) atoi(pch);
		pch = strtok (NULL, ",}"); //value //was strtok (NULL, " :,{}");
//		iprintf ("(%d:%s)\n",k, pch);
		if (pch) processPair(256*index + k,pch);
		pch = strtok (NULL, " :,{}");
	}
}

char response[8192]; //response to ?D request (graph data) may be large.

void exportSettings(char * filename) {

//	char sdFolderBuf[10];
	char sdFolderBuf[15];
	sprintf(sdFolderBuf, "TCP-IO");
//	iprintf("changing to %s\r\n", sdFolderBuf);

	int fold = f_mkdir(sdFolderBuf);
	if ((fold == F_NO_ERROR) || (fold == F_ERR_DUPLICATED)) {

//	if (f_chdir(sdFolderBuf) == F_NO_ERROR) {
//		iprintf( "folder change to %s ok\r\n", sdFolderBuf);
		if (!f_chdir(sdFolderBuf)) {
			F_FILE* fp = f_open( filename, "w" );
			if ( fp ) {
				//add a row for test configuration
				for (int i=0; i<16; i++) {
					getAllTestParams(i, response);
					strcat(response, "\r\n");
					int n = f_write( response, 1, strlen( response), fp );
					iprintf( "Writing %d bytes to file %s:\r\n", n, filename);
				}

				//options
				getAllOptions(response);
				strcat(response, "\r\n");
				int n = f_write( response, 1, strlen( response), fp );
				iprintf( "Writing %d bytes to file %s:\r\n", n, filename);

				//add a row for each channel calibration
				for (int i=0; i<4; i++) {
					getAllCalibrationValues(i, response);
					strcat(response, "\r\n");
					int n = f_write( response, 1, strlen( response), fp );
					iprintf( "Writing %d bytes to file %s:\r\n", n, filename);
				}
				f_close(fp);
				sprintf(response, "Settings saved to %s\r\n", filename);
			}
			else {
				iprintf( "open file failed %s:\r\n", filename );
			}
			f_chdir(".."); //back to root folder
		}
		else {
			iprintf( "chdir failed\r\n" );
		}
	}
	else iprintf("mkdir failed\r\n");
}

void importSettings(char * filename) {
	char buf[512];

	char path[128];
	sprintf(path, "TCP-IO/%s", filename);
	iprintf("Opening %s", path);
	F_FILE* file = f_open(path, "r");
	if (!file) {
		sprintf(response, "Failed to open: %s", path);
		iprintf("%s\r\n", response);
	}
	else {
		while (!f_eof(file)) {
			//char * str =
			f_fgets(buf, 512, file);
			strcat(buf,"\r"); //command processor requires \r or \n
			//iprintf("%s",buf);
			char * res = processCommand(buf);
			iprintf("%s\r\n",res);
		}
		iprintf("Imported settings from: %s\r\n", path);
		f_close(file);
	}
}

char * processCommand(char *cmdstr){
	/*validate here...to do...
	 * bool validated = true;
	 * if (!validated) ..to do? ..; //error messsage
	*/

	iprintf ("Splitting string \"%s\" into tokens:\n",cmdstr);

	response[0] = '\0';

	//Parse cmdstr using string tokeniser
	char *pch;
	pch = strtok (cmdstr," :,{}");


	if (pch != NULL)
	{
//		iprintf ("Header: %s:%lu:%x\n",pch, strlen(pch), pch[0]);
//		char response[1024];
		switch (toupper(pch[0])) {

			case 'M' : { //MALT (S)tart, (R)eset, (A)bort, select (T)estN
				switch (toupper(pch[1])) {
				case 'S':
					webStart = true;
					sprintf(response, "Start Test");
					break;
				case 'R':
					webReset = true;
					sprintf(response, "Reset");
					break;
				case 'A':
					webReset = true;
					sprintf(response, "Abort");
					break;
				case 'T':
					appliedConfig = atoi(&pch[2]);
					sprintf(response, "Select test %d", appliedConfig);
					break;
				default: ;
				} //end inner switch
				break;
			}	//end case 'M'
			case 'C' : { //Calibrate (P)ressure | (D)ifferential {key:value,...}
				int channel = atoi(&pch[1]);
				sprintf(response, "Calibration %d", channel); //get config number
				parseKVPairs(setCalibration, channel);
				break;
			}
			case 'P' : { //Parameters for TestNumber{key:value,...}
				int config = atoi(&pch[1]);
				sprintf(response, "Parameters %d", config); //get config number
				parseKVPairs(setTestParam, config);
				break;
			}
			case 'O' : {//Options {key:value,...}
				sprintf(response, "Options" );
				parseKVPairs(setOption);
				break;
			}
			case 'T' : { //set time in format YYYY-MM-DD-hh-mm-ss
				tm dt;
				int year = 1901;
				int month = 1;
				//strptime(&pch[1], "%Y-%m-%d-%H-%M-%S", &dt);
				sscanf(&pch[1], "%4d-%2d-%2d-%2d-%2d-%2d",
					&year, &month, &dt.tm_mday, &dt.tm_hour, &dt.tm_min, &dt.tm_sec);
				dt.tm_year= year-1900;
				dt.tm_mon = month-1;
				strftime(response, 32, "%F %T", &dt);
				MCF541X_RTCSetTime(dt);
				MCF541X_RTCSetSystemFromRTCTime();
				break;
			}
			case 'F' : { // File (E)xport | (I)mport filename
	//			sprintf(response, "File %s %s", (pch[1]=='E'?"Export":(pch[1]=='I'?"Import":"-")), (pch+2) );
				switch (toupper(pch[1])){
				case 'E':
					exportSettings(pch+2);
					sprintf(response, "Settings exported: TCP-IO\\%s", pch+2 );
					break;
				case 'I':
					importSettings(pch+2); //--- to do
					//setImportReq(pch+2); --- quick test
					sprintf(response, "Settings imported: TCP-IO\\%s", pch+2 );
					break;
				case 'S': {
					int b = SaveUserParameters(&NV_Params, sizeof(NV_Params_Struct));
					sprintf(response,"Save settings to memory: : %d bytes", b);
					break;
					}
				case 'L': {
					NV_Params_Struct *pItem = (NV_Params_Struct*) GetUserParameters();
					NV_Params = *pItem;
					sprintf(response,"Load settings from memory: %lu bytes", sizeof(*pItem));
					break;
					}
				default :
					;
				}
				break;
			}


			static char* teststate[40] = {"Waiting  ","Read Prog","Load Prog","Fix OPs  ","Fix OPs  ", //Steps 0 to 4
								"Fix IPs  ","Spare    ","Fixture A ","Fixture B ","Fixture C ","Spare    ","Spare    ", //Steps 5 to 11
								"Vent Off ","Filling  ","Stabilise","Dwell    ","Measure  ","Pass     ","Fail     ", //Steps 12 to 18
								"Fault    ","Vent     ","Chain Tst","Fixture C ","Fixture B ","Fail Ack? ","Fixture A ", //Steps 19 to 25
								"Done     ","Spare    ","Spare    ","Spare    ","Spare    ","Spare    ", //Steps 26 to 31
								"Spare    ","DiffP Cal","DiffP Cal","TestP Cal","TestP Cal","End Cal  ", //Steps 32 to 37
								"Spare    ","Manual Ops  "}; //Steps 38 to 39


			case '?' : { //Query Status | (P)arams for Test n | (C)alibration | (O)Options
				iprintf("Query %s, %d\r\n", pch, atoi(pch+2));
				switch (toupper(pch[1])) {
				case 'T': sprintf(response, "%d", appliedConfig ); break;//selected test
				case 'I': sprintf(response, "%d", (FAULTBIT | DONEBIT | FAILBIT | PASSBIT)); break;
				case 'S': sprintf(response, "%d", stepCount ); break; //stage of test
				case 'R': sprintf(response, "%d", LT_Results.status ); break; //result code
				case 'Q': sprintf(response, "%s", teststate[stepCount]  ); break; //test state
				//..expand to give each field...see resultsComms
				case 'D' :
					//pullGraphData(response, atoi(pch+2));
					getTestDataCSV(response);
					break; ///data
				case 'Z' : sprintf(response, "%d",LT_Results.diffpAZ); break;
				case 'L' : {
					//Retrieve the testp value from datalog[0] or datalog[1] as needed
					RunningValues *val = datalog[0].getCurrent(); //assuming datalog[0] is the target
					sprintf(response,"%1.0f", (val -> testp)/10.0);
					break;
				}
				case 'M': {
					//Retrieve the diffp value from datalog[0] or datalog[1] as needed
					RunningValues *val = datalog[0].getCurrent(); //assuming datalog[0] is the target
					sprintf(response,"%8.1f", (val -> diffp)/10.0);
					break;
				}
				case 'P': getAllTestParams(atoi(pch+2), response); break;
				case 'O': getAllOptions(response); break;
				case 'C': getAllCalibrationValues(atoi(pch+2), response); break;
				//case 'F': //echo setting file
				//	break;
				default: ;
				}
				break;
			}
			case 'Q' : sprintf(response, "Quit TCP" ); break;
			default:
				sprintf(response, "Unknown Command: %c", pch[0]);
		}
		pch = strtok (NULL, " :,{}");
	}

	//iprintf("%s, %d\r\n",response, strlen(response));
	return response;
}

