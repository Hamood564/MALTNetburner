/*
 * userParameters.h
 *
 *  Created on: 13 May 2016
 *      Author: leszek.zarzycki
 *  Modified on: 12 Dec 2019 to retro-fit MALT_DEV projects
 */
/*
 * The MOD54415 has 128kB of persistent storage for user parameters.
 * Parameters are saved as a blob, and retrieved using a read only
 * pointer. The save and get functions are defined in system.h
 * References:
 * 		MOD54415 Platform Reference
 * 		NeturnerRuntimeLibrary
 *
 * This file provides functions to save user data so that it can be restored
 * on power up. The save function gathers together leak test configuration
 * parameters, options, calibration data and (in future) any other data
 * that needs to be saved. The load function accesses the saved data and restores
 * it to the various data structures.
 *
 * Settings can be imported/exported to onboard SD card
 */

#ifndef USERPARAMETERS_H_
#define USERPARAMETERS_H_

/* Retrieves the stored settings from MALT on-chip flash memory;
 * Restores default settings if (internal) verification key is not valid. This may
 * only happen on the first system boot up after a software update.*/
//void loadSettings();
//void saveSettings();

/*print the Settings to standard io (ie using printf) */
//void printSettings();

/*/Requests to export/import settings to the given filename
 * Only the most recent export/import request is stored,
 * so call to service the requests should be made regularly.
 */
void setExportReq(char * filename);
void setImportReq(char * filename);

//Services the most recent request (if any).
void serviceMemoryCardReq();

#endif /* USERPARAMETERS_H_ */
