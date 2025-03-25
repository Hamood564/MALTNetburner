/**
 * datalog.cpp
 * Circular array used to store captured test data
 *  - Netburner example - with small modification.
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT1.1
 * @date 2015-01-13

 */
/**************************************************************************//**
 * Based on code found at:
 * C:\nburn\examples\StandardStack\Web\AjaxGraph
 *
 *****************************************************************************/
/*
 * Defines a circular buffer to store sensorStruct tuples.
 *
 */

#ifndef  __DATALOG_H
#define  __DATALOG_H

#include <stdint.h>
#include "MLT_globalExterns.h"

class RingLog{
    private:
        RunningValues  *pHead; //pointer to cell with oldest data item
        RunningValues  *pTail; //pointer to next cell to assign data
        RunningValues  *current; //pointer to most recently added data (pTail-1 except on wrap)
        int32_t        count;  //number of elements in the buffer

        const RunningValues  *pStart; //pointer to first cell of buffer, i.e. buffer[0]
        const RunningValues  *pEnd;   //pointer to end of buffer, i.e. buffer[maxSize]
        int32_t      	maxSize; //length of buffer
        TestResults results;
        void IncrementTail();
        void IncrementHead();
        void IncrementPtr( RunningValues *&ptr );

    public:
        RingLog();
        RingLog( RunningValues *buffer, uint32_t bufferSize );
        ~RingLog();

        void init(RunningValues *buffer, uint32_t bufferSize);

        void Add( RunningValues *item );
        void Remove( RunningValues *item );
        void Clear();
        int32_t GetCount();
        void Dump();

        //lz - accessor to the most recent record in data log
        RunningValues * getCurrent(); //

        // lz - accessors to retrieve computed results
        int getResTP();
        int getResDP();
        int getResLR();
        int getResAZ();
        int getResTimeStamp();

        TestResults getResults() const;
        void setResults(TestResults results);
}
;

#endif   /* ----- #ifndef __DATALOG_H  ----- */
