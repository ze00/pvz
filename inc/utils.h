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
#include <pthread.h>
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
  int32_t value;
  void *remote;
} __images;
typedef struct __heaps {
  __list list;
  char *base;
  char *end;
  char *buf;
  size_t heap_size;
} __heaps;
typedef struct thread_ids {
  __list list;
  pthread_t id;
} thread_ids;
typedef struct thread_arg {
  __heaps *heap;
  void (*callable)(void *, void *);
} thread_arg;
#define next(x) ((x)->list.next)
#define real(x) ((x)->list.real)
#define back(x) ((__typeof__((x)))(x)->list.real)
extern void pop(__task **);
extern int has(__task *, int, int);
extern void insert_images(__images **, int, void *);
extern void recover_images(__images *);
extern void insert_heaps(__heaps **, char *, char *);
extern void destroy_heaps(__heaps **);
extern void destroy(__list **, void (*)(void *));
extern void *insert(__list **, size_t);
extern void insert_thread_ids(thread_ids **, pthread_t);
extern void destroy_thread_ids(thread_ids **);
extern void parseRowAndCol(const char *, __task **);
extern void checkRootState();
extern const char *readline(FILE *);
extern pid_t findPVZProcess();
extern FILE *openProcFile(pid_t, const char *);
#endif //__UTILS__H
