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
#include <stdint.h>
#include <unistd.h>
#include <sys/cdefs.h>
#include "defs.h"
#include "utils.h"
struct {
  char *base;
  char *bss;
  int32_t val;
  pid_t pid;
  __task *task;
  __images *images;
} baseInfo;
void initBase() {
  baseInfo.base = NULL;
  baseInfo.bss = NULL;
  baseInfo.task = NULL;
  baseInfo.images = NULL;
}
#endif //__BASE__H
