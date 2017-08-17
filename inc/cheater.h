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
void changeCoins(int newVal) {
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
  pvz_write(bp + off - 4,&newVal,sizeof(newVal));
  printf("now set coins to %d\n",newVal);
}
void removeColdDown() {
  char *base = baseInfo.base;
  int *p = (int *)(base + getOffset("cannon")),val;
  for(int i = 0;i < 48;++i) {
    pvz_write(p,&val,sizeof(val));
    p -= 9;
  }
}
void letZombiesFragile() {
  size_t memsz = baseInfo.heap_end - baseInfo.heap_base,maxIndex = memsz - ZOM_HP_OFF;
  struct {
    size_t curHp;
    size_t totalHp;
    size_t armor;
  } Hp;
  struct {
    size_t newHp;
    size_t totalHp;
    size_t armor;
  } newHp;
  char *buf = (char *)malloc(memsz),*rp,*orig = buf;
  int *helper;
  while(1) {
    buf = orig;
    pvz_read(baseInfo.heap_base,buf,memsz);
    for(size_t i = 0;i < maxIndex;++i) {
      helper = (int *)buf;
      if(helper[0] == 0xffffffff &&
          helper[1] == 0x0 &&
          helper[2] == 0xffffffff &&
          helper[3] == 0xffffffff &&
          helper[4] == 0) {
        rp = baseInfo.heap_base + i + ZOM_HP_OFF;
        memcpy(&Hp,buf + ZOM_HP_OFF,sizeof(Hp));
        if(IN_RANGE(Hp.totalHp,270,6000)) {
          newHp.newHp = 10;
          newHp.armor = 0;
          printf("set %p (%zu %zu,%zu %zu)\n",rp,Hp.curHp,Hp.armor,newHp.newHp,newHp.armor);
          pvz_write(rp,&newHp,sizeof(newHp));
        }
      }
      ++buf;
    }
    usleep(200000);
  }
  free(buf);
}
void increaseCabbageHurler() {
  char *p = baseInfo.base + getOffset("cabbage");
  int v = 45;
  pvz_write(p + 8,&v,sizeof(v));
}
#endif //__CHEATER__H
