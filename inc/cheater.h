/*
 * File    : cheater.h
 * Project : PVZ
 * Author  : ze00
 * Email   : zerozakiGeek@gmail.com
 * Date    : 2017-08-15
 * Module  :
 * License : MIT
 */
#ifndef __CHEATER__H
#define __CHEATER__H
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "pvz_offset.h"
#include "base.h"
#include "scanmem.h"
void pvz_write(void *rp, void *buf, size_t len) {
  if (!sm_write_array(baseInfo.pid, rp, buf, len)) {
    printf("cannot write memory at %p,'%s' has died?\n", rp, SPECIFIC_PACKAGE);
    exit(-1);
  }
}
void pvz_read(void *rp, void *buf, size_t len) {
  if (!sm_read_array(baseInfo.pid, rp, buf, len)) {
    printf("cannot read memory at %p,'%s' has died?\n", rp, SPECIFIC_PACKAGE);
    exit(-1);
  }
}
void *getBase(const char *spec, int findFirst,
              void (*action)(const char *, void *, void *), void *end) {
  void *base;
  FILE *maps = openProcFile(baseInfo.pid, "maps");
  BufferType buf;
  while (fgets(buf, BUFSIZE, maps) != NULL) {
    if (strstr(buf, spec)) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
      if (action == NULL)
        sscanf(buf, "%" PRIx64, (unsigned int *)&base);
      else
        action(buf, &base, end);
      if (findFirst)
        break;
#pragma GCC diagnostic pop
    }
  }
  fclose(maps);
  return base;
}
void *getDynamicBase() {
  pid_t pid = findPVZProcess();
  if (pid == -1) {
    printf("cannot locate '%s'\n", SPECIFIC_PACKAGE);
    exit(-1);
  }
  printf("Found %s:%d\n", SPECIFIC_PACKAGE, pid);
  baseInfo.pid = pid;
  return getBase(SPECIFIC_DYNAMIC_LIBRARIES, 1, NULL, NULL);
}
void getBaseAndEnd(const char *buf, void __unused *base, void __unused *end) {
  void *f, *g;
  // 禁止-Wformat的警告
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
  sscanf(buf, "%" PRIx64 "-%" PRIx64, &f, &g);
  // sscanf(buf, "%8x-8x", (int *)base,(int *)end)
  // 不知道为什么这条代码会出问题
  insert_heaps(&baseInfo.heap, f, g);
#pragma GCC diagnostic pop
}
void getHeapBase() {
  // 见kernel/Documentation/filesystems/proc.txt
  getBase("[anon:libc_malloc]", 0, getBaseAndEnd, NULL);
  getBase("[heap]", 0, getBaseAndEnd, NULL);
}

void changeCoins() {
  char *helper = baseInfo.base + getOffset("coins"),
       *bp = helper + COINS_HELPER_OFF, *hp = bp;
  char buf[COINS_HELPER_BUFF];
  // baseInfo.base + getOffset("coins") + 0x7aa400;
  pvz_read(bp, buf, COINS_HELPER_BUFF);
  hp = buf;
  off_t off;
  for (off = 0; off < sizeof(buf); ++off) {
    if (*(intptr_t *)hp == (intptr_t)helper) {
      break;
    }
    hp++;
  }
  pvz_write(bp + off - 4, &baseInfo.val, sizeof(baseInfo.val));
  printf("now set coins to %d\n", baseInfo.val);
}
void removeColdDown() {
  char *base = baseInfo.base;
  int32_t *p = (int32_t *)(base + getOffset("cannon")), val = 0;
  for (size_t i = 0; i < 48; ++i) {
    pvz_write(p, &val, sizeof(val));
    p -= 9;
  }
}
void letZombiesFragile(void *dp, void *rp) {
  struct Hp *hp = dp + ZOM_HP_OFF;
  if (hp->curHp != 10 && IN_RANGE(hp->totalHp, 270, 6000)) {
    hp->curHp = 10;
    hp->totalHp = 0;
    hp->armor = 0;
    pvz_write((char *)rp + ZOM_HP_OFF, hp, sizeof(*hp));
  }
}
void coverZombies(void *dp, void *rp) {
  pvz_write((char *)rp + 0x3c, "\x88\x13\x00\x00", sizeof(int32_t));
}
void increaseZombies(void *dp, void *rp) {
  baseInfo.val = *((int32_t *)dp + ZOM_HP_OFF / sizeof(int32_t)) * 2;
  pvz_write((char *)rp + ZOM_HP_OFF, &baseInfo.val, sizeof(baseInfo.val));
}
void increaseCabbagePult() {
  char *p = baseInfo.base + getOffset("cabbage");
  int32_t v = 45;
  pvz_write(p + 8, &v, sizeof(v));
}
void forEach(void *(entry)(void *), void (*op)(void *, void *)) {
  __heaps *heap = baseInfo.heap;
  thread_ids *ids = NULL;
  pthread_t id;
  pthread_mutex_init(&mutex, NULL);
  while (heap != NULL) {
    // 此处使用malloc而不使用一个局部对象
    // 可能会由于编译器的问题
    // 每次进入while内
    // 创建的arg都是同一个地址
    thread_arg *arg = malloc(sizeof(thread_arg));
    arg->heap = heap;
    arg->callable = op;
    pthread_create(&id, NULL, entry, arg);
    // pthread_detach(id);
    insert_thread_ids(&ids, id);
    heap = next(heap);
  }
  // FIXME:如果有线程被中断
  // 则内存泄露
  destroy_thread_ids(&ids);
  pthread_mutex_destroy(&mutex);
}
#define init()                                                                 \
  thread_arg *arg = my_arg;                                                    \
  __heaps *heap = arg->heap;                                                   \
  char *buf = heap->buf;

// 此处需要使用pthread_mutex_lock
// 一个进程不能同时被ptrace
#define read()                                                                 \
  pthread_mutex_lock(&mutex);                                                  \
  pvz_read(heap->base, buf, heap->heap_size);                                  \
  pthread_mutex_unlock(&mutex);

// arg->callable可能会使用pvz_write
#define call()                                                                 \
  pthread_mutex_lock(&mutex);                                                  \
  arg->callable(helper, heap->base + i);                                       \
  pthread_mutex_unlock(&mutex);

#define exit()                                                                 \
  free(arg);                                                                   \
  pthread_exit(NULL);

void *forEachPlants_child(void *my_arg) {
  init();
  read();
  size_t maxIndex = heap->heap_size - PLAN_HP_TOTAL_OFF;
  int32_t *helper;
  for (size_t i = 0; i < maxIndex; ++i) {
    helper = (int32_t *)buf;
    if (helper[0] == 0x43200000 && helper[1] == 0x42200000 && helper[2] == 1 &&
        IN_RANGE(helper[PLAN_HP_TOTAL_OFF / sizeof(int32_t)], 300, 8000)) {
      call();
    }
    ++buf;
  }
  exit();
}
void forEachPlants(void (*op)(void *, void *)) {
  forEach(forEachPlants_child, op);
}
void *forEachZombies_child(void *my_arg) {
  init();
  read();
  size_t maxIndex = heap->heap_size - ZOM_HP_OFF;
  int32_t *helper;
  for (size_t i = 0; i < maxIndex; ++i) {
    helper = (int32_t *)buf;
    if (helper[0] == 0xffffffff && helper[1] == 0x0 &&
        helper[2] == 0xffffffff && helper[3] == 0xffffffff && helper[4] == 0) {
      call();
    }
    ++buf;
  }
  exit();
}
void forEachZombies(void (*op)(void *, void *)) {
  forEach(forEachZombies_child, op);
}
#undef init
#undef read
#undef call
#undef exit
#define ROW(lp) (*INT32P(lp + getOffset("zombies_row")) + 1)
#define COL(lp) (*(float *)(lp + getOffset("zombies_pos_y")))
#define HP(lp) (*INT32P(lp + ZOM_HP_OFF))
#define CODE(lp) (*INT32P(lp + getOffset("zombies_type")))
void reportZombies(void *local, void *rp) {
  printf("Found at %p (row@%d x pos_y@%f)(hp:%d code:%d)\n", rp, ROW(local),
         COL(local), HP(local), CODE(local));
}
#undef ROW
#undef COL
#undef HP
#undef CODE
void increasePlants(void *dp, void *rp) {
  baseInfo.val = (*(int32_t *)((char *)dp + PLAN_HP_OFF)) * 2;
  pvz_write((char *)rp + PLAN_HP_OFF, &baseInfo.val, sizeof(baseInfo.val));
}
void increasePlantsAttack(void *dp, void *rp) {
  baseInfo.val = (*(int32_t *)((char *)dp + PLAN_ATT_TOTAL_OFF)) / 2;
  pvz_write((char *)rp + PLAN_ATT_TOTAL_OFF, &baseInfo.val,
            sizeof(baseInfo.val));
}
void putLadder(void *local, void *remote) {

  if (baseInfo.task != NULL) {
    int32_t type = *(int32_t *)(local + getOffset("zombies_type"));
    if (type == 21) {
      float f = baseInfo.task->col * 100, of;
      int32_t row = baseInfo.task->row - 1;
      pvz_read(remote + getOffset("zombies_pos_x"), &of, sizeof(of));
      if (f > of)
        return;
      pvz_write(remote + getOffset("zombies_row"), &row, sizeof(row));
      pvz_write(remote + getOffset("zombies_pos_x"), &f, sizeof(f));
      pvz_write(remote + getOffset("zombies_pos_y"), &f, sizeof(f));
      printf("put ladder on %d:%d\n", baseInfo.task->row, baseInfo.task->col);
      pop(&baseInfo.task);
    }
  }
}
#define ROW(lp) (*INT32P(lp + getOffset("plants_row")) + 1)
#define COL(lp) (*INT32P(lp + getOffset("plants_col")) + 1)
#define HP(lp) (*INT32P(lp + PLAN_HP_OFF))
#define CODE(lp) (*INT32P(lp + getOffset("plants_type")))
#define ATTACK(lp) (*INT32P(lp + getOffset("plants_attack")))
void reportPlants(void *plant, void *rp) {
  printf("Found at %p (row@%d x col@%d)(hp:%d code:%d)\n", rp, ROW(plant),
         COL(plant), HP(plant), CODE(plant));
}
void fuck_LilyPad_Pumpkin(void *local, void *remote) {
  if (has(baseInfo.task, ROW(local), COL(local))) {
    switch (CODE(local)) {
    case LILYPAD_CODE:
      baseInfo.val = 0;
      pvz_write(remote + getOffset("plants_vis"), &baseInfo.val,
                sizeof(baseInfo.val));
      break;
    case PUMPKIN_CODE:
      baseInfo.val = 1332;
      pvz_write(remote + getOffset("plants_hp"), &baseInfo.val,
                sizeof(baseInfo.val));
    }
  }
}
void plants_freeze(void *local, void *remote) {
  if (ATTACK(local) == 0)
    return;
  insert_images(&baseInfo.images, ATTACK(local),
                remote + getOffset("plants_attack"));
  baseInfo.val = 0;
  pvz_write(remote + getOffset("plants_attack"), &baseInfo.val,
            sizeof(baseInfo.val));
}
void plants_attack(void *local, void *remote) {
  recover_images(baseInfo.images);
  destroy((__list **)&baseInfo.images, NULL);
}
#undef ROW
#undef COL
#undef HP
#undef CODE
#undef ATTACK
void catchSIGINT() {
  fflush(stdout);
  setbuf(stdin, NULL);
  destroy((__list **)&baseInfo.task, NULL);
  longjmp(env, SETJMP_RET);
}
void registeSigHandle() { signal(SIGINT, catchSIGINT); }
#endif //__CHEATER__H
