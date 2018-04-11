/*
 * File    : pvz.h
 * Project : PVZ
 * Author  : ze00
 * Email   : zerozakiGeek@gmail.com
 * Date    : 2017-08-15
 * Module  :
 * License : MIT
 */
#ifndef __PVZ__H
#define __PVZ__H
#include <sys/cdefs.h>
#include <pthread.h>
#include <setjmp.h>
#ifndef SPECIFIC_PACKAGE
#define SPECIFIC_PACKAGE "com.popcap.pvz_na"
#endif
#ifndef SPECIFIC_DYNAMIC_LIBRARIES
#define SPECIFIC_DYNAMIC_LIBRARIES "libpvz.so"
#endif
#define PUMPKIN_CODE (30)
#define LILYPAD_CODE (16)
struct Hp {
  size_t curHp;
  size_t totalHp;
  size_t armor;
};
jmp_buf env;
#endif //__PVZ__H
