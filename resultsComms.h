/**
 * resultsComms.h
 * Header for task to capture and communicate results over TCP/UDP.
 * @author gavin.murray
 * @version MALT5.5
 * @date 2014-12-02
 */

#ifndef RESULTSCOMMS_H_
#define RESULTSCOMMS_H_


void initResultsFifoMembers();
int findFreeResultsFifoStruct();

void resultsCommsTask( void *pdata );


#endif /* RESULTSCOMMS_H_ */
