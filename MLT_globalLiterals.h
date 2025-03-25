/**
 * MLT_globalLiterals.h
 * Literals for test results - inspect board outputs IO_OPs.
 * @author gavin.murray
 * @version Single_MALT1.1
 * @date 2015-01-13
 */

#ifndef MLT_GLOBALLITERALS_H_
#define MLT_GLOBALLITERALS_H_

#include "MLT_globalExterns.h"


#define PASSBIT       (IO_OPs & 0x0001) // True if Pass OP is On
#define FAILBIT       (IO_OPs & 0x0002) // True if Fail OP is On
#define DONEBIT       (IO_OPs & 0x0004) // True if Done OP is On
#define FAULTBIT      (IO_OPs & 0x0008) // True if LTA Alarm OP is on

//#define PASSBITB       (IO_OPs & 0x0010) // True if Pass OP is On
//#define FAILBITB       (IO_OPs & 0x0020) // True if Fail OP is On
//#define DONEBITB       (IO_OPs & 0x0040) // True if Done OP is On
//#define FAULTBITB      (IO_OPs & 0x0080) // True if LTB Alarm OP is on


#endif /* MLT_GLOBALLITERALS_H_ */
