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
#include "defs.h"
struct {
  char *base;
  char *heap_base;
  char *heap_end;
  char *heap_buf;
  size_t heap_size;
  int newVal;
  pid_t pid;
  ProcessDIR processDIR;
} baseInfo;
#endif //__BASE__H
