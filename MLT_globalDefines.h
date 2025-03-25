/**
 * globalDefines.h
 * Constants for Test sequence steps and result status
 * @author gavin.murray
 * @version MALT5.5
 * @date 2014-12-02
 */

#ifndef MLT_GLOBALDEFINES_H_
#define MLT_GLOBALDEFINES_H_

//Test Sequence Steps
#define RESETSTEP           0
#define GETTESTPARAMSSTEP   1
#define LOADPARAMSSTEP   	2
#define SETFIXOPSTARTSTEP   3
#define FIXOPDELAYSTEP  	4
#define FIXIPCHECKSTEP  	5
#define SPARESTEP			6
#define FIXASTEP			7
#define FIXBSTEP			8
#define FIXCSTEP			9
#define VENTOFFSTEP         12
#define FILLSTEP            13
#define STABILISESTEP       14
#define ISOLATEDWELLSTEP    15
#define MEASURESTEP         16
#define PASSSTEP            17
#define FAILSTEP            18
#define FAULTSTEP           19
#define EVACSTEP            20
#define CHAINTESTSTEP       21
#define FIXCENDSTEP			22
#define FIXBENDSTEP			23
#define FAILACKSTEP			24
#define FIXAENDSTEP			25
#define COMPLETESTEP        26
#define DIFFPCAL1           33
#define DIFFPCAL2           34
#define TESTPCAL1           35
#define TESTPCAL2           36
#define ENDCALSTEP          37
#define MANUALOPSSTEP       39


//Test Results Status definitions
#define RESSTATNORESULT		0
#define RESSTATPASS			1
#define RESSTATFAIL			2
#define RESSTATGROSS		3
#define RESSTATTESTPLOW		4
#define RESSTAT5			5
#define RESSTAT6			6
#define RESSTAT7			7
#define RESSTATPASSNODISP	8
#define RESSTATFAILNODISP	9
#define RESSTATDIFFPHI		10
#define RESSTATRESET		11
#define RESSTATTESTPHI		12
#define RESSTATDIFFPREF		13
#define RESSTAT14			14
#define RESSTATIPSNOK		15
#define RESSTATFIXNOK		16

#define RESULTSFIFOSIZE		10

#endif /* MLT_GLOBALDEFINES_H_ */
