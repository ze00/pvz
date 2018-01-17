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
typedef struct ladder_task {
  int row;
  int col;
  struct ladder_task *next;
} ladder_task;
struct {
  char *base;
  char *heap_base;
  char *heap_end;
  char *heap_buf;
  size_t heap_size;
  int newVal;
  pid_t pid;
  ProcessDIR processDIR;
  ladder_task *task;
  ladder_task *task_helper;
} baseInfo;
void initBase() {
  baseInfo.base = NULL;
  baseInfo.heap_base = NULL;
  baseInfo.heap_end = NULL;
  baseInfo.heap_buf = NULL;
  baseInfo.task = NULL;
  baseInfo.task_helper = NULL;
}
inline static void insert(ladder_task **target, int row, int col) {
  ladder_task *node = malloc(sizeof(ladder_task));
  node->row = row;
  node->col = col;
  node->next = NULL;
  if (*target == NULL) {
    *target = node;
  } else {
    (*target)->next = node;
  }
}
inline static void destroy(ladder_task **node) {
  ladder_task *helper;
  while (*node != NULL) {
    helper = (*node)->next;
    free(*node);
    *node = helper;
  }
  *node = NULL;
}
#endif //__BASE__H
