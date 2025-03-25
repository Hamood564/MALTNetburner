/*
 * cmdprocessor.h
 *
 *  Created on: 17 Jun 2020
 *      Author: leszek.zarzycki
 */

#ifndef CMDPROCESSOR_H_
#define CMDPROCESSOR_H_

char * processCommand(char *cmdstr);

void exportSettings(char * filename);

//char * parseKVPairs(void (*processPair)(int, char *), int testindex=0);

#endif /* CMDPROCESSOR_H_ */
