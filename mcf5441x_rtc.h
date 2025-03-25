/**
 * mmcf5441x_rtc.h
 * Modified version of Netburner RTC
 * @author gavin.murray
 * @version MALT5.5
 * @date 2014-12-02
 */

#ifndef MCF5441X_RTC_H_
#define MCF5441X_RTC_H_

//#ifdef __cplusplus
//extern "C"
//{
//#endif /* __cplusplus */


/**
 * Gets the current RTC time.
 *
 * bts    - The basic time structure used to store the current time read from
 *          the RTC.
 *
 * return - '0' if successful, '1' if failure.
 */
int MCF541X_RTCGetTime( struct tm &bts );


/**
 * Sets the RTC time with a given time structure.
 *
 * bts    - The basic time structure used to set the RTC time.
 *
 * return - '0' if successful, '1' if failure.
 */
int MCF541X_RTCSetTime( struct tm &bts );


/**
 * Sets the system time with the current RTC time.
 *
 * return - '0' if successful, '1' if failure.
 */
int MCF541X_RTCSetSystemFromRTCTime( void );


/**
 * Sets the RTC time with the current system time.
 *
 * return - '0' if successful, '1' if failure.
 */
int MCF541X_RTCSetRTCfromSystemTime( void );



/* Read and write data to from the 2K bytes of back up SRAM */
int MCF541X_GetBackUpRam(PBYTE  pDest, int len);

int MCF541X_SetBackUpRam(PBYTE  pSrc, int len);

/* Read RTC Compensation INTERVAL and VALUE */
int MCF541X_GetCompData();

//#ifdef __cplusplus
//};
//#endif /* __cplusplus */
#endif /* MCF5441X_RTC_H_ */

