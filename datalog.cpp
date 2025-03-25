/**
 * datalog.cpp
 * Circular array used to store captured test data
 *  - Netburner example - with small modification.
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT1.1
 * @date 2015-01-13
 */
/**************************************************************************//**
 *
 * Copyright 1998-2013 NetBurner, Inc.  ALL RIGHTS RESERVED
 *   Permission is hereby granted to purchasers of NetBurner Hardware
 *   to use or modify this computer program for any use as long as the
 *   resultant program is only executed on NetBurner provided hardware.
 *
 *   No other rights to use this program or it's derivatives in part or
 *   in whole are granted.
 *
 *   It may be possible to license this or other NetBurner software for
 *   use on non-NetBurner Hardware.
 *   Please contact sales@Netburner.com for more information.
 *
 *   NetBurner makes no representation or warranties
 *   with respect to the performance of this computer program, and
 *   specifically disclaims any responsibility for any damages,
 *   special or consequential, connected with the use of this program.
 *
 *---------------------------------------------------------------------
 * NetBurner, Inc.
 * 5405 Morehouse Drive
 * San Diego, California 92121
 *
 * information available at:  http://www.netburner.com
 * E-Mail info@netburner.com
 *
 * Support is available: E-Mail support@netburner.com
 *
 *****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <utils.h>
#include <math.h>

#include "datalog.h"

RingLog::RingLog()
: pHead (NULL), pTail(NULL), count(0),
  pStart(NULL), pEnd(NULL), maxSize(0)
{
}

RingLog::RingLog( RunningValues *buffer, uint32_t bufferSize )
    : pHead(buffer), pTail(buffer),  current(buffer),   count(100),
      pStart(buffer), pEnd(buffer + bufferSize), maxSize(bufferSize)
{
}

RingLog::~RingLog( )
{
}


void RingLog::init(RunningValues *buffer, uint32_t bufferSize) {
	pHead=buffer;
	pTail=buffer;
	count=100;
	pStart=buffer;
	pEnd=buffer + bufferSize;
	maxSize=bufferSize;
}
/**
 * @brief Add
 *  Adds an item to the end of the RingLog. This performs a deep copy of the input.
 *
 * @param item The item to be added.
 */
void RingLog::Add( RunningValues *item )
{
	if (maxSize == 0) { return; }
    if (!pTail) { return; }

    memcpy( pTail, item, sizeof(*item) );
    count++;
    current = pTail;
    IncrementTail();
}

/**
 * @brief Remove
 *  Removes an item from the front of the RingLog. If the input is non-null,
 *  it copy the head element into the structure pointed to.
 *
 * @param item Pointer to a return structure
 */
void RingLog::Remove( RunningValues *item )
{
    if (maxSize == 0) { return; }

    if (count && item) {
        memcpy( item, pHead, sizeof(*item) );
        count--;
        IncrementHead();
    }
}


/**
 * @brief Clear
 *  Clears the RingLog
 */
void RingLog::Clear( )
{
    pHead = pTail = const_cast<RunningValues *>(pStart);
    count = 100;
}

void RingLog::IncrementTail( )
{
    if (pTail == pHead) {
        IncrementHead();
    }
    pTail++;
    if (pTail >= pEnd) {
        pTail = const_cast<RunningValues *>(pStart);
    }
}

void RingLog::IncrementHead()
{
    pHead++;
    if (pHead >= pEnd) {
        pHead = const_cast<RunningValues *>(pStart);
    }
}

void RingLog::IncrementPtr( RunningValues *&ptr)
{
    ptr++;
    if (ptr >= pEnd) {
        ptr = const_cast<RunningValues *>(pStart);
    }
}


/**
 * @brief GetCount
 *
 * @return A count of the number of entries currently in the RingLog
 */
int32_t RingLog::GetCount()
{
    return count;
}


/**
 * @brief Dump
 *  Prints the values of vital members of the RingLog.
 */
void RingLog::Dump()
{
    printf("  pHead: %p\r\n", pHead);	//pointer to cell with oldest data item
    printf("  pTail: %p\r\n", pTail);
    printf(" pStart: %p\r\n", pStart);
    printf("   pEnd: %p\r\n", pEnd);
    printf("  count: %ld\r\n", count);
    printf("maxSize: %ld\r\n", maxSize);
}

//lz
RunningValues * RingLog::getCurrent() { //(int sensorId) {
	return current;
}

// placeholders to retrieve computed results
int RingLog::getResTP() {
	return results.testp; /*   current->testp;*/
}

int RingLog::getResDP() {
	return results.diffp; /*count+1;*/
}

int RingLog::getResLR() {
	return results.leakRate; /*count+2;*/
}

int RingLog::getResAZ() {
	return results.diffpAZ; /*count+3;*/
}

int RingLog::getResTimeStamp() {
	return TimeTick;
}

TestResults RingLog::getResults() const {
	return results;
}

void RingLog::setResults(TestResults results) {
	this->results = results;
}


