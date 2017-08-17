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
  sm_read_array(baseInfo.pid,bp,buf,COINS_HELPER_BUFF);
  hp = buf;
  off_t off;
  for(off = 0;off < sizeof(buf);++off) {
    if(*(int*)hp == (int)helper) {
      break;
    }
    hp++;
  }
  if(!sm_write_array(baseInfo.pid,bp + off - 4,&newVal,sizeof(newVal))) {
    printf("set coins error\n");
    exit(-1);
  } else {
    printf("now set coins to %d\n",newVal);
  }
}
void removeColdDown() {
  char *base = baseInfo.base;
  int *p = (int *)(base + getOffset("cannon")),val;
  for(int i = 0;i < 48;++i) {
    // sm_read_array(baseInfo.pid,p,&val,sizeof(int));
    // printf("%d\n",val);
    sm_write_array(baseInfo.pid,p,&val,sizeof(val));
    p -= 9;
  }
}
void letZombiesFragile() {
  size_t memsz = baseInfo.heap_end - baseInfo.heap_base,maxIndex = memsz - ZOM_HP_OFF;
  struct {
    size_t curHp;
    size_t totalHp;
  } Hp;
  struct {
    size_t newHp;
  } newHp;
  char *buf = (char *)malloc(memsz),*rp,*orig = buf;
  int *helper;
  // sm_read_array(baseInfo.pid,baseInfo.heap_base,buf,memsz);
  while(1) {
    buf = orig;
    if(!sm_read_array(baseInfo.pid,baseInfo.heap_base,buf,memsz)) {
      printf("cannot read memory,'%s' has died?\n",SPECIFIC_PACKAGE);
      exit(-1);
    }
    for(size_t i = 0;i < maxIndex;++i) {
      helper = (int *)buf;
      if(helper[0] == 0xffffffff &&
          helper[1] == 0x0 &&
          helper[2] == 0xffffffff &&
          helper[3] == 0xffffffff &&
          helper[4] == 0) {
        rp = baseInfo.heap_base + i + ZOM_HP_OFF;
        sm_read_array(baseInfo.pid,rp,(char *)&Hp,sizeof(Hp));
        if(Hp.totalHp >= 270 && Hp.totalHp <= 6000) {
          newHp.newHp = 10;
          printf("set %p (%zu,%zu)\n",rp,Hp.curHp,newHp.newHp);
          if(!sm_write_array(baseInfo.pid,rp,(char *)&newHp,sizeof(newHp))) {
            printf("cannot write memory,'%s' has died?\n",SPECIFIC_PACKAGE);
            exit(-1);
          }
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
  sm_write_array(baseInfo.pid,p + 8,&v,sizeof(v));
}
#endif //__CHEATER__H
