/**
 * test_seq.h
 * Interface to MALT engine.
 * @author gavin.murray
 * @version MALT5.5
 * @date 2014-12-02
 */

#ifndef TEST_SEQ_H_
#define TEST_SEQ_H_

/*
 * "test_seq.cpp" function prototypes to satisfy compilation of other units that call them.
 */
void sendLog(int count);
//void writeOutputs( BYTE SolMask, BYTE IO_Mask );
//BYTE readInputs();
void printTimeStruct(struct tm &bt );
void initTestSoftware();
void initHardware();
void testSeqTask( void *pdata );


#endif /* TEST_SEQ_H_ */
