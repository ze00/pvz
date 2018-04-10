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
int32_t getI32(void *rp) {
  static int32_t val;
  pvz_read(rp, &val, sizeof(val));
  return val;
}
void *getP32(void *rp) {
  static void *val;
  pvz_read(rp, &val, sizeof(val));
  return val;
}
float getF32(void *rp) {
  static float val;
  pvz_read(rp, &val, sizeof(val));
  return val;
}
void setI32(void *rp, int32_t v) { pvz_write(rp, &v, sizeof(v)); }
void setF32(void *rp, float v) { pvz_write(rp, &v, sizeof(v)); }
void *getBase(const char *spec, int findFirst, void (*action)(void *, void *),
              void **end) {
  void *base;
  FILE *maps = openProcFile(baseInfo.pid, "maps");
  BufferType buf;
  while (fgets(buf, BUFSIZE, maps) != NULL) {
    if (strstr(buf, spec)) {
      sscanf(buf, "%p-%p", &base, end);
      if (action != NULL)
        action(base, *end);
      if (findFirst)
        break;
    }
  }
  fclose(maps);
  return base;
}
void detectPVZ() {
  pid_t pid = findPVZProcess();
  if (pid == -1) {
    printf("cannot locate '%s'\n", SPECIFIC_PACKAGE);
    exit(-1);
  }
  printf("Found %s:%d\n", SPECIFIC_PACKAGE, pid);
  baseInfo.pid = pid;
}
void getHeapBaseAndEnd(void *base, void *end) {
  insert_heaps(&baseInfo.heap, base, end);
}
void *getDynamicBase() {
  void *v;
  return getBase(SPECIFIC_DYNAMIC_LIBRARIES, 1, NULL, (void **)&v);
}
void getHeapBase() {
  void *v;
  // 见kernel/Documentation/filesystems/proc.txt
  getBase("[anon:libc_malloc]", 0, getHeapBaseAndEnd, &v);
  getBase("[heap]", 0, getHeapBaseAndEnd, &v);
}
void getBssBase() {
  getBase(SPECIFIC_DYNAMIC_LIBRARIES, 0, NULL, (void **)&baseInfo.bss);
}

void removeColdDown() {
  char *base = baseInfo.base;
  int32_t *p = (int32_t *)(base + getOffset("cannon"));
  for (size_t i = 0; i < 48; ++i) {
    setI32(p, 0);
    p -= 9;
  }
}
void letZombiesFragile(void *rp) {
  struct Hp hp = {
      .curHp = 10,
      .totalHp = 10,
      .armor = 0,
  };
  pvz_write((char *)rp + ZOM_HP_OFF, &hp, sizeof(hp));
}
void coverZombies(void *rp) { setI32(rp + 0xbc, 5000); }
void increaseZombies(void *rp) {
  setI32(rp + ZOM_HP_OFF, getI32(rp + ZOM_HP_OFF) * 2);
}
void increaseCabbagePult() {
  char *p = baseInfo.base + getOffset("cabbage");
  setI32(p + 8, 45);
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
#undef init
#undef read
#undef call
#undef exit
void *getField() {
  void *helper = getP32(baseInfo.bss + getOffset("base"));
  return getOffset("field") + helper;
}
void *getStatus() {
  void *status = getP32(getField() + getOffset("status"));
  return status;
}
void forEachPlants(void (*op)(void *, void *)) {
  forEach(forEachPlants_child, op);
}
void forEachZombies(void (*op)(void *)) {
  size_t zcnt = getI32(getStatus() + getOffset("zombies_count"));
  int32_t *entry = getP32(getStatus() + getOffset("zombies_list"));
  void *zp;
  for (size_t idx = 0; idx < zcnt;) {
    // 在僵尸地址前
    // 有一些小的数据
    // 不知道干嘛的
    zp = getP32(entry);
    if (zp > (void *)0x10000000) {
      op(zp);
      idx++;
      // 僵尸地址后面有一个指针
      // 同不知道干嘛的
      entry++;
    }
    entry++;
  }
}
#define ROW(lp) (getI32(lp + getOffset("zombies_row")) + 1)
#define COL(lp) (getF32(lp + getOffset("zombies_pos_y")))
#define HP(lp) (getI32(lp + ZOM_HP_OFF))
#define CODE(lp) (getI32(lp + getOffset("zombies_type")))
void reportZombies(void *rp) {
  printf("Found at %p(row@%d x pos_y@%f)(hp:%d code:%d)\n", rp, ROW(rp),
         COL(rp), HP(rp), CODE(rp));
}
#undef ROW
#undef COL
#undef HP
#undef CODE
void increasePlants(void *dp, void *rp) {
  baseInfo.val = (*(int32_t *)((char *)dp + PLAN_HP_OFF)) * 2;
  setI32(rp + PLAN_HP_OFF, baseInfo.val);
}
void increasePlantsAttack(void *dp, void *rp) {
  baseInfo.val = (*(int32_t *)((char *)dp + PLAN_ATT_TOTAL_OFF)) / 2;
  setI32(rp + PLAN_ATT_TOTAL_OFF, baseInfo.val);
}
void putLadder(void *remote) {

  if (baseInfo.task != NULL) {
    int32_t type = getI32(remote + getOffset("zombies_type"));
    if (type == 21) {
      float f = baseInfo.task->col * 100;
      int32_t row = baseInfo.task->row - 1;
      if (f > getF32(remote + getOffset("zombies_pos_x")))
        return;
      setI32(remote + getOffset("zombies_row"), row);
      setF32(remote + getOffset("zombies_pos_x"), f);
      setF32(remote + getOffset("zombies_pos_y"), f);
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
      setI32(remote + getOffset("plants_vis"), 0);
      break;
    case PUMPKIN_CODE:
      setI32(remote + getOffset("plants_hp"), 1332);
    }
  }
}
void plants_freeze(void *local, void *remote) {
  if (ATTACK(local) == 0)
    return;
  insert_images(&baseInfo.images, ATTACK(local),
                remote + getOffset("plants_attack"));
  setI32(remote + getOffset("plants_attack"), 0);
}
void plants_attack(void *local, void *remote) {
  recover_images(baseInfo.images);
  destroy((__list **)&baseInfo.images, NULL);
}
void changeCoins() {}
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
