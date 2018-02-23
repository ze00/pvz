/*
 * File    : inc/utils.h
 * Project :
 * Author  : ze00
 * Email   : zerozakiGeek@gmail.com
 * Date    : 2018-01-19
 * Module  :
 */
#ifndef __UTILS__H
#define __UTILS__H
#include <stdint.h>
#include "defs.h"
#define PANIC                                                                  \
  do {                                                                         \
    printf("无效输入\n");                                                      \
    raise(SIGINT);                                                             \
  } while (0)
typedef struct __list {
  void *next;
  void *real;
} __list;
typedef struct __task {
  __list list;
  int32_t row;
  int32_t col;
} __task;
typedef struct __images {
  __list list;
  int32_t attack;
  void *remote;
} __images;
typedef struct __heaps {
  __list list;
  char *base;
  char *end;
  char *buf;
  size_t heap_size;
} __heaps;

#define next(x) ((x)->list.next)
#define real(x) ((x)->list.real)
extern void pop(__task **);
extern int has(__task *, int, int);
extern void insert_images(__images **, int, void *);
extern void recover_images(__images *);
extern void insert_heaps(__heaps **, char *, char *);
extern void destroy_heaps(__heaps **);
extern void destroy(__list **, void (*)(void *));
extern void *insert(__list **, size_t);
extern void parseRowAndCol(const char *, __task **);
extern void checkRootState();
extern pid_t findPVZProcess(ProcessDIR);
#endif //__UTILS__H
