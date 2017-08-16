 /*
  * File    : base.h
  * Project : PVZ
  * Author  : ze00
  * Email   : zerozakiGeek@gmail.com
  * Date    : 2017-08-15
  * Module  : 
  * License : MIT
  */
#ifndef __BASE__H
#define __BASE__H
#include <unistd.h>
#include <sys/cdefs.h>
struct {
  char *base;
  pid_t pid;
} baseInfo;
#endif //__BASE__H
