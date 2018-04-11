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
#define BUFSIZE 255
#define IN_RANGE(obj, min, max) (obj >= min && obj <= max)
#define SETJMP_RET 0xff
#define WAIT_USECONDS (500000)
typedef char BufferType[BUFSIZE];
typedef char Path[PATH_MAX];
#endif //__DEFS__H
