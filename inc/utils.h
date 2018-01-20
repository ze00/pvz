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

#define PANIC                                                                  \
  do {                                                                         \
    printf("无效输入\n");                                                      \
    raise(SIGINT);                                                             \
  } while (0)
typedef struct __task {
  int row;
  int col;
  struct __task *next;
} __task;
typedef struct __images {
  struct __images *next;
  int attack;
  void *remote;
} __images;
extern void insert(__task **, int, int);
extern void destroy(__task **);
extern void pop(__task **);
extern int has(__task *, int, int);
extern void insert_images(__images **,int,void *);
extern void destroy_images(__images **);
extern void recover_images(__images *);
extern void parseRowAndCol(const char *, __task **, __task **);
#endif //__UTILS__H
