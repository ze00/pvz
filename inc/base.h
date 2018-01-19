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
#include <stdlib.h>
#include <unistd.h>
#include <sys/cdefs.h>
#include "defs.h"
#include "utils.h"
struct {
  char *base;
  char *heap_base;
  char *heap_end;
  char *heap_buf;
  size_t heap_size;
  int val;
  pid_t pid;
  ProcessDIR processDIR;
  __task *task;
  __task *task_helper;
} baseInfo;
void initBase() {
  baseInfo.base = NULL;
  baseInfo.heap_base = NULL;
  baseInfo.heap_end = NULL;
  baseInfo.heap_buf = NULL;
  baseInfo.task = NULL;
  baseInfo.task_helper = NULL;
}

#endif //__BASE__H
