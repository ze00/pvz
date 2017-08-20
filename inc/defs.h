 /*
  * File    : defs.h
  * Project : PVZ
  * Author  : ze00
  * Email   : zerozakiGeek@gmail.com
  * Date    : 2017-08-17
  * Module  : 
  * License : MIT
  */
#ifndef __DEFS__H
#define __DEFS__H
#include <limits.h>
#ifndef SPECIFIC_PACKAGE
#define SPECIFIC_PACKAGE "com.popcap.pvz_na"
#endif
#ifndef SPECIFIC_DYNAMIC_LIBRARIES
#define SPECIFIC_DYNAMIC_LIBRARIES "libpvz.so"
#endif
#define BUFSIZE 255
#define COINS_HELPER_BUFF 256
#define COINS_HELPER_OFF 0x7aa300
#define IN_RANGE(obj,min,max) (obj >= min && obj <= max)
#define SETJMP_RET 0xff
typedef char BufferType[BUFSIZE];
typedef char ProcessDIR[PATH_MAX];
typedef char Path[PATH_MAX];
#endif //__DEFS__H
