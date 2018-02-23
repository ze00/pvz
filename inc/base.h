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
  __heaps *heap;
  int val;
  pid_t pid;
  ProcessDIR processDIR;
  __task *task;
  __task *task_helper;
  __images *images;
  __images *images_helper;
} baseInfo;
void initBase() {
  baseInfo.base = NULL;
  baseInfo.heap = NULL;
  baseInfo.task = NULL;
  baseInfo.task_helper = NULL;
  baseInfo.images = NULL;
  baseInfo.images_helper = NULL;
}

#endif //__BASE__H
