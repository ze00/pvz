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
#include <stdlib.h>
#include <string.h>
#include "pvz.h"
#include "pvz_offset.h"
#include "base.h"
#include "scanmem.h"
void checkRootState() {
  if (getuid() != 0 || getgid() != 0) {
    printf("must run me under root mode\n");
    exit(-1);
  }
}
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
int getBase(const char *spec, int findFirst,
            void (*action)(void *, void *, void *), void *end) {
  unsigned int base;
  Path vmMaps;
  sprintf(vmMaps, "%s/maps", baseInfo.processDIR);
  FILE *maps = fopen(vmMaps, "r");
  BufferType buf;
  while (fgets(buf, BUFSIZE, maps) != NULL) {
    if (strstr(buf, spec)) {
      if (action == NULL)
        sscanf(buf, "%8x", &base);
      else
        action(buf, &base, end);
      if (findFirst)
        break;
    }
  }
  fclose(maps);
  return base;
}
int *getDynamicBase() {
  pid_t pid = findPVZProcess(baseInfo.processDIR);
  if (pid == -1) {
    printf("cannot locate '%s'\n", SPECIFIC_PACKAGE);
    exit(-1);
  }
  baseInfo.pid = pid;
  return (int *)getBase(SPECIFIC_DYNAMIC_LIBRARIES, 1, NULL, NULL);
}
void getBaseAndEnd(void *buf, void *base, void *end) {
  sscanf(buf, "%8x-%8x", (int *)base, (int *)end);
}
void getNoughtBaseAndEnd(void *buf, void __unused *base, void __unused *end) {
  int f, g;
  sscanf(buf, "%8x-%8x", &f, &g);
  // sscanf(buf, "%8x-8x", (int *)base,(int *)end)
  // 不知道为什么这条代码会出问题
  if (g - f == 0x300000) {
    baseInfo.heap_base = (void *)f;
    baseInfo.heap_end = (void *)g;
  }
}
int *getHeapBase() {
#ifdef NOUGHT
  getBase("[anon:libc_malloc]", 0, getNoughtBaseAndEnd, &baseInfo.heap_end);
  return (int *)baseInfo.heap_base;
#else
  return (int *)getBase("[heap]", 0, getBaseAndEnd, &baseInfo.heap_end);
#endif
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
    if (*(int *)hp == (int)helper) {
      break;
    }
    hp++;
  }
  printf("%p\n", bp + off - 4);
  pvz_write(bp + off - 4, &baseInfo.val, sizeof(baseInfo.val));
  printf("now set coins to %d\n", baseInfo.val);
}
void removeColdDown() {
  char *base = baseInfo.base;
  int *p = (int *)(base + getOffset("cannon")), val = 0;
  for (int i = 0; i < 48; ++i) {
    pvz_write(p, &val, sizeof(val));
    p -= 9;
  }
}
void *createBuf(char *end, char *begin, size_t *size) {
  size_t memsz = end - begin;
  *size = memsz;
  return malloc(memsz);
}
void findZombies(void (*op)(void *, void *)) {
  char *buf = baseInfo.heap_buf;
  pvz_read(baseInfo.heap_base, buf, baseInfo.heap_size);
  size_t maxIndex = baseInfo.heap_size - ZOM_HP_OFF;
  int *helper;
  for (size_t i = 0; i < maxIndex; ++i) {
    helper = (int *)buf;
    if (helper[0] == 0xffffffff && helper[1] == 0x0 &&
        helper[2] == 0xffffffff && helper[3] == 0xffffffff && helper[4] == 0) {
      op(helper, baseInfo.heap_base + i);
    }
    ++buf;
  }
}
void letZombiesFragile(void *dp, void *rp) {
  memcpy(&Hp, (char *)dp + ZOM_HP_OFF, sizeof(Hp));
  if (Hp.curHp != 10 && IN_RANGE(Hp.totalHp, 270, 6000)) {
    newHp.newHp = 10;
    newHp.totalHp = 0;
    newHp.armor = 0;
    pvz_write((char *)rp + ZOM_HP_OFF, &newHp, sizeof(newHp));
  }
}
void coverZombies(void *dp, void *rp) {
  pvz_write((char *)rp + 0x3c, "\x88\x13\x00\x00", sizeof(int));
}
void increaseZombies(void *dp, void *rp) {
  baseInfo.val = *((int *)dp + ZOM_HP_OFF / sizeof(int)) * 2;
  pvz_write((char *)rp + ZOM_HP_OFF, &baseInfo.val, sizeof(baseInfo.val));
}
void increaseCabbagePult() {
  char *p = baseInfo.base + getOffset("cabbage");
  int v = 45;
  pvz_write(p + 8, &v, sizeof(v));
}
void findPlants(void (*op)(void *, void *)) {
  char *buf = baseInfo.heap_buf;
  pvz_read(baseInfo.heap_base, buf, baseInfo.heap_size);
  size_t maxIndex = baseInfo.heap_size - PLAN_HP_OFF;
  int *helper;
  for (size_t i = 0; i < maxIndex; ++i) {
    helper = (int *)buf;
    if (helper[0] == 0x43200000 && helper[1] == 0x42200000 && helper[2] == 1 &&
        IN_RANGE(helper[PLAN_HP_OFF / sizeof(int)], 300, 8000)) {
      op(helper, baseInfo.heap_base + i);
    }
    ++buf;
  }
}
void report(void *__unused __, void *p) { printf("found at %p\n", p); }
void increasePlants(void *dp, void *rp) {
  baseInfo.val = (*(int *)((char *)dp + PLAN_HP_OFF)) * 2;
  pvz_write((char *)rp + PLAN_HP_OFF, &baseInfo.val, sizeof(baseInfo.val));
}
void increasePlantsAttack(void *dp, void *rp) {
  baseInfo.val = (*(int *)((char *)dp + PLAN_ATT_TOTAL_OFF)) / 2;
  pvz_write((char *)rp + PLAN_ATT_TOTAL_OFF, &baseInfo.val,
            sizeof(baseInfo.val));
}
void putLadder(void *local, void *remote) {

  if (baseInfo.task != NULL) {
    int type = *(int *)(local + getOffset("zombies_type"));
    if (type == 21) {
      float f = baseInfo.task->col * 100, of;
      int row = baseInfo.task->row - 1;
      pvz_read(remote + getOffset("zombies_pos_x"), &of, sizeof(of));
      if (f > of)
        return;
      pvz_write(remote + getOffset("zombies_row"), &row, sizeof(row));
      pvz_write(remote + getOffset("zombies_pos_x"), &f, sizeof(f));
      pvz_write(remote + getOffset("zombies_pos_y"), &f, sizeof(f));
      printf("put ladder on %d:%d\n", baseInfo.task->row, baseInfo.task->col);
      baseInfo.task_helper = baseInfo.task->next;
      free(baseInfo.task);
      baseInfo.task = baseInfo.task_helper;
    }
  }
}
#define ROW(lp) (*INTP(lp + getOffset("plants_row")) + 1)
#define COL(lp) (*INTP(lp + getOffset("plants_col")) + 1)
#define HP(lp) (*INTP(lp + PLAN_HP_OFF))
#define CODE(lp) (*INTP(lp + getOffset("plants_type")))
void reportPlants(void *plant, void *rp) {
  printf("found at %p (row@%d x col@%d)(hp:%d code:%d)\n", rp, ROW(plant),
         COL(plant), HP(plant), CODE(plant));
}
void fuck_LilyPad_Pumpkin(void *local, void *remote) {}
#undef ROW
#undef COL
#undef HP
#undef CODE

void catchSIGINT() {
  fflush(stdout);
  setbuf(stdin, NULL);
  destroy(&baseInfo.task);
  baseInfo.task_helper = NULL;
  longjmp(env, SETJMP_RET);
}
void registeSigHandle() { signal(SIGINT, catchSIGINT); }
#endif //__CHEATER__H
