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
#include <scanmem/scanmem.h>
#include "pvz.h"
#include "pvz_offset.h"
#include "base.h"
void checkRootState() {
  if(getuid() != 0 || getgid() != 0) {
    printf("must run me under root mode\n");
    exit(-1);
  }
}
void pvz_write(void *rp,void *buf,size_t len) {
  if(!sm_write_array(baseInfo.pid,rp,buf,len)) {
    printf("cannot write memory,'%s' has died?\n",SPECIFIC_PACKAGE);
    exit(-1);
  }
}
void pvz_read(void *rp,void *buf,size_t len) {
  if(!sm_read_array(baseInfo.pid,rp,buf,len)) {
    printf("cannot read memory,'%s' has died?\n",SPECIFIC_PACKAGE);
    exit(-1);
  }
}
int getBase(const char *spec,int findFirst,void (*action)(void *,void *,void *),void *end) {
  int base;
  Path vmMaps;
  sprintf(vmMaps,"%s/maps",baseInfo.processDIR);
  FILE *maps = fopen(vmMaps,"r");
  BufferType buf;
  while(fgets(buf,BUFSIZE,maps) != NULL) {
    if(strstr(buf,spec)) {
      if(action == NULL)
        sscanf(buf,"%8x",&base);
      else
        action(buf,&base,end);
      if(findFirst)
        break;
    }
  }
  fclose(maps);
  return base;
}
int *getDynamicBase() {
  pid_t pid = findPVZProcess(baseInfo.processDIR);
  if(pid == -1) {
    printf("cannot locate '%s'\n",SPECIFIC_PACKAGE);
    exit(-1);
  }
  baseInfo.pid = pid;
  return (int *)getBase(SPECIFIC_DYNAMIC_LIBRARIES,1,NULL,NULL);
}
void getBaseAndEnd(void *buf,void *base,void *end) {
  sscanf((char*)buf,"%8x-%8x",(int*)base,(int*)end);
}
int *getHeapBase() {
  return (int *)getBase("[heap]",0,getBaseAndEnd,&baseInfo.heap_end);
}
void changeCoins() {
  char *helper = baseInfo.base + getOffset("coins"),*bp = helper + COINS_HELPER_OFF,*hp = bp;
  char buf[COINS_HELPER_BUFF];
  // baseInfo.base + getOffset("coins") + 0x7aa400;
  pvz_read(bp,buf,COINS_HELPER_BUFF);
  hp = buf;
  off_t off;
  for(off = 0;off < sizeof(buf);++off) {
    if(*(int*)hp == (int)helper) {
      break;
    }
    hp++;
  }
  pvz_write(bp + off - 4,&baseInfo.newVal,sizeof(baseInfo.newVal));
  printf("now set coins to %d\n",baseInfo.newVal);
}
void removeColdDown() {
  char *base = baseInfo.base;
  int *p = (int *)(base + getOffset("cannon")),val;
  for(int i = 0;i < 48;++i) {
    pvz_write(p,&val,sizeof(val));
    p -= 9;
  }
}
void *createBuf(char *end,char *begin,size_t *size) {
  size_t memsz = end - begin;
  *size = memsz;
  return malloc(memsz);
}
void findZombies(void (*op)(void *,void *)) {
  char *buf = baseInfo.heap_buf;
  pvz_read(baseInfo.heap_base,buf,baseInfo.heap_size);
  size_t maxIndex = baseInfo.heap_size - ZOM_HP_OFF;
  int *helper;
  for(size_t i = 0;i < maxIndex;++i) {
    helper = (int *)buf;
    if(helper[0] == 0xffffffff &&
        helper[1] == 0x0 &&
        helper[2] == 0xffffffff &&
        helper[3] == 0xffffffff &&
        helper[4] == 0) {
      op(helper,baseInfo.heap_base + i);
    }
    ++buf;
  }
}
void letZombiesFragile(void *dp,void *rp) {
  memcpy(&Hp,(char *)dp + ZOM_HP_OFF,sizeof(Hp));
  if(Hp.curHp != 10 && IN_RANGE(Hp.totalHp,270,6000)) {
    newHp.newHp = 10;
    newHp.totalHp = 0;
    newHp.armor = 0;
    pvz_write((char *)rp + ZOM_HP_OFF,&newHp,sizeof(newHp));
  }
}
void coverZombies(void *dp,void *rp) {
  baseInfo.newVal = 5000;
  pvz_write((char *)rp + 0x3c,&baseInfo.newVal,sizeof(baseInfo.newVal));
}
void increaseZombies(void *dp,void *rp) {
  baseInfo.newVal = *((int *)dp + ZOM_HP_OFF / sizeof(int)) * 2;
  pvz_write((char *)rp + ZOM_HP_OFF,&baseInfo.newVal,sizeof(baseInfo.newVal));
}
void increaseCabbageHurler() {
  char *p = baseInfo.base + getOffset("cabbage");
  int v = 45;
  pvz_write(p + 8,&v,sizeof(v));
}
void findPlants(void (*op)(void *,void *)) {
  char *buf = baseInfo.heap_buf;
  pvz_read(baseInfo.heap_base,buf,baseInfo.heap_size);
  size_t maxIndex = baseInfo.heap_size - PLAN_HP_OFF;
  int *helper;
  for(size_t i = 0;i < maxIndex;++i) {
    helper = (int *)buf;
    if(helper[0] == 0x43200000 && helper[1] == 0x42200000 && IN_RANGE(helper[PLAN_HP_OFF / sizeof(int)],300,8000)) {
      op(helper,baseInfo.heap_base + i);
    }
    ++buf;
  }
}
void report(void *__unused __,void *p) {
  printf("found at %p\n",p);
}
void increasePlants(void *dp,void *rp) {
  baseInfo.newVal = (*(int*)((char *)dp + PLAN_HP_OFF)) * 2;
  pvz_write((char *)rp + PLAN_HP_OFF,&baseInfo.newVal,sizeof(baseInfo.newVal));
}
void increasePlantsAttack(void *dp,void *rp) {
  baseInfo.newVal = (*(int*)((char *)dp + PLAN_ATT_TOTAL_OFF)) / 2;
  pvz_write((char *)rp + PLAN_ATT_TOTAL_OFF,&baseInfo.newVal,sizeof(baseInfo.newVal));
}
void catchSIGINT() {
  fflush(stdout);
  longjmp(env,SETJMP_RET);
}
void registeSigHandle() {
  signal(SIGINT,catchSIGINT);
}
#endif //__CHEATER__H
