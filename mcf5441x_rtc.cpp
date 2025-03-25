/**
 * mmcf5441x_rtc.cpp
 * Modified version of Netburner RTC
 * @author gavin.murray
 * @version MALT5.5
 * @date 2014-12-02
 */

#include <predef.h>
#include <time.h>
#include <constants.h>
#include <basictypes.h>
#include <sim.h>     // On-chip register definitions
#include <system.h>
//#include <mcf5441x_rtc.h>
#include <ucos.h>
#include <nbtime.h>

#include "mcf5441x_rtc.h"


/*
 struct tm
 * {
 *    int tm_sec;     // Second (0-59)
 *    int tm_min;     // Minute (0-59)
 *    int tm_hour;    // Hour (0-23)
 *    int tm_mday;    // Day of the month (1-31)
 *    int tm_mon;     // Month of the year [January(0)-December(11)]
 *    int tm_year;    // Years since 1900
 *    int tm_wday;    // Day of the week [Sunday(0)-Saturday(6)]
 *    int tm_yday;    // Days since January 1st (0-365)
 *    int tm_isdst;   // Daylight Saving Time flag (in effect if greater than
 *                    // zero, not in effect if equal to zero, information not
 *                    // available if less than zero)
 * };
*/

/**
 * Gets the current RTC time.
 *
 * bts    - The basic time structure used to store the current time read from
 *          the RTC.
 *
 * return - '0' if successful, '1' if failure.
 */


static const int d_inmob[12]=
{
//0, //0  //GM Fixed because month is 0 to 11
0, //jan
31, //feb
59, //mar
90, //april
120, //may
151, //jun
181, //jul
212, //aug
243, //sep
273, //oct
304, //now
334  //dec
};




int MCF541X_RTCGetTime( struct tm &bts )
{

sim2.rtc.cr=0;
asm("nop");
sim2.rtc.cfg=0x08;
asm("nop");
sim2.rtc.cr=0x02; //Write protect and set binary mode DST off...
asm("nop");


int nTries=0;
WORD ym;
WORD dy;
WORD hm;
WORD s;

do
{
WORD sr1=sim2.rtc.sr;
ym =sim2.rtc.yearmon;
dy =sim2.rtc.days;
hm =sim2.rtc.hourmin;
s  =sim2.rtc.seconds;
WORD sr2=sim2.rtc.sr;
if((sr1& 1) || (sr2&1))
{
	OSTimeDly(1);
}
else break;

} while(nTries++ < 5);

if(nTries==5) return 1;
long year=2112+(((short)ym)>>8);
if(year<2012) return 1;


bts.tm_sec=(s & 0xFF); // Second (0-59)
bts.tm_min=(hm & 0xFF); // Minute (0-59)
bts.tm_hour=(hm>>8); // Hour (0-23)
bts.tm_mday=(dy & 0xff); // Day of the month (1-31)
bts.tm_mon=((ym & 0xff) - 1); //GM Month of the year in RTC January(1)-December(12) in tm struct January(0)-December(11)
bts.tm_year=(year - 1900); // Years since 1900
bts.tm_wday=((dy>>8) & 0x07); // Day of the week [Sunday(0)-Saturday(6)]

bts.tm_isdst=0;
//bts.tm_yday=d_inmob[bts.tm_mon]+(bts.tm_mday-1); //Original yday = 0 to 364/365
bts.tm_yday=d_inmob[bts.tm_mon]+(bts.tm_mday); //GM Fixed so that yday = 1 to 365/366

//Fix up leap year
//if((bts.tm_mon>2)&& ((year % 4 == 0 && year % 100 != 0 || year % 400 == 0))) //Original
if((bts.tm_mon>=2) && (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))) //GM fixed to work from March onwards not April
{
bts.tm_yday++;
}

return 0;
}


/**
 * Sets the RTC time with a given time structure.
 *
 * bts    - The basic time structure used to set the RTC time.
 *
 * return - '0' if successful, '1' if failure.
 */
int MCF541X_RTCSetTime( struct tm &bts )
{

long year=bts.tm_year+1900;
year-=2112;
year=year<<8;

WORD ym=(WORD)year|((WORD)(bts.tm_mon + 1)); //GM Month of the year in RTC January(1)-December(12) in tm struct January(0)-December(11)
WORD dy=((bts.tm_wday)<<8)|(bts.tm_mday);
WORD hm=(bts.tm_hour<<8) | (bts.tm_min);
WORD s=bts.tm_sec;



sim2.rtc.cfg=0x08;



sim2.rtc.cr=0x00; //Write protect and set binary mode DST off...
asm(" nop");
sim2.rtc.cr=0x01; //Write protect and set binary mode DST off...
asm(" nop");
sim2.rtc.cr=0x03; //Write protect and set binary mode DST off...
asm(" nop");
sim2.rtc.cr=0x02; //Write protect and set binary mode DST off...

int nTries=0;

do
{
WORD sr1=sim2.rtc.sr;
sim2.rtc.yearmon = ym;
sim2.rtc.days    = dy;
sim2.rtc.hourmin = hm;
sim2.rtc.seconds = s;
WORD sr2=sim2.rtc.sr;
//sim2.rtc.compen = 0x04FB; //Compensation interval 4s value -5 (0xFB)
sim2.rtc.compen = 0x04FC; //Compensation interval 4s value -5 (0xFB) 3/12/14 try -4
if((sr1 & 1) || (sr2 & 1))
{
	OSTimeDly(1);
}
else break;
} while(nTries++ < 5);

sim2.rtc.cr=0;
asm("nop");
sim2.rtc.cfg=0x08;
asm("nop");
sim2.rtc.cr=0x02; //Write protect and set binary mode DST off...
asm("nop");


if(nTries<5) return 0;



return 1;
}


/**
 * Sets the system time with the current RTC time.
 *
 * return - '0' if successful, '1' if failure.
 */
int MCF541X_RTCSetSystemFromRTCTime( void )
{
struct tm bts;
int rv = MCF541X_RTCGetTime( bts );

if ( rv )
{
   return rv;
}

time_t t = mktime( &bts );
set_time( t );
return 0;

}


/**
 * Sets the RTC time with the current system time.
 *
 * return - '0' if successful, '1' if failure.
 */
int MCF541X_RTCSetRTCfromSystemTime( void )
{
   time_t t = time( NULL );
   struct tm *ptm = gmtime( &t );
   return MCF541X_RTCSetTime( *ptm );

}




/* Read and write data to from the 2K bytes of back up SRAM */
int MCF541X_GetBackUpRam(PBYTE pDest, int len)
{
if(len>2048 ) len =2048;
PBYTE pb=(PBYTE)sim2.rtc.stdbyram;
for(int i=0; i<len; i++) pDest[i]=pb[i];
return len;
}

int MCF541X_SetBackUpRam(PBYTE pSrc, int len)
{
if(len>2048 ) len =2048;
PBYTE pb= (PBYTE)sim2.rtc.stdbyram;

sim2.rtc.cr=0x00; //Write protect and set binary mode DST off...
asm(" nop");
sim2.rtc.cr=0x01; //Write protect and set binary mode DST off...
asm(" nop");
sim2.rtc.cr=0x03; //Write protect and set binary mode DST off...
asm(" nop");
sim2.rtc.cr=0x02; //Write protect and set binary mode DST off...

for(int i=0; i<len; i++) pb[i]=pSrc[i];

sim2.rtc.cr=0;
asm("nop");
sim2.rtc.cfg=0x08;
asm("nop");
sim2.rtc.cr=0x02; //Write protect and set binary mode DST off...
asm("nop");

return len;
}



/* Read RTC Compensation Done (CDON) bit from Status Reg
 * and  Compensation INTERVAL and VALUE from Compensation Reg */
int MCF541X_GetCompData()
{
	int comp=sim2.rtc.compen & 0xffff; //Read INTERVAL (MSB) and VALUE (LSB)
	int compdone=sim2.rtc.sr & 2; //Read Compensation Done bit (bit 1 in Status Reg
/*	if (compdone != 0)  //If Comp Done bit is set reset it
	{                     //First enable writing to RTC (Comp Done isn't cleared if RTC is Write Protected)
		sim2.rtc.cr=0x00; //Write protect and set binary mode DST off...
		asm(" nop");
		sim2.rtc.cr=0x01; //Write protect and set binary mode DST off...
		asm(" nop");
		sim2.rtc.cr=0x03; //Write protect and set binary mode DST off...
		asm(" nop");
		sim2.rtc.cr=0x02; //Write protect and set binary mode DST off...
		sim2.rtc.sr = 0x02; // Write to Comp Done bit to clear it
		sim2.rtc.cr=0;      //Now Write protect again
		asm("nop");
		sim2.rtc.cfg=0x08;
		asm("nop");
		sim2.rtc.cr=0x02; //Write protect and set binary mode DST off...
		asm("nop");
	} */
    return ((compdone<<16) | comp);
}


