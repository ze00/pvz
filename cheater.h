 /*
  * File    : cheater.h
  * Project :
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
void checkRootState() {
  if(getuid() != 0 || getgid() != 0) {
    printf("must run me under root mode\n");
    exit(-1);
  }
}
int *getDynamicBase() {
  ProcessDIR processDIR;
  pid_t pid = findPVZProcess(processDIR);
  if(pid == -1) {
    printf("cannot locate '%s'\n",SPECIFIC_PACKAGE);
    exit(-1);
  }
  baseInfo.pid = pid;
  int *base = NULL;
  Path vmMaps;
  sprintf(vmMaps,"%s/maps",processDIR);
  FILE *maps = fopen(vmMaps,"r");
  BufferType buf;
  while(fgets(buf,BUFSIZE,maps) != NULL) {
    if(strstr(buf,SPECIFIC_DYNAMIC_LIBRARIES)) {
      sscanf(buf,"%8x",&base);
      break;
    }
  }
  fclose(maps);
  return base;
}
void changeCoins(int newVal) {
  char *helper = baseInfo.base + getOffset("coins"),*bp = helper + COINS_HELPER_OFF,*hp = bp;
  char buf[COINS_HELPER_BUFF];
  // baseInfo.base + getOffset("coins") + 0x7aa400;
  sm_read_array(baseInfo.pid,bp,buf,COINS_HELPER_BUFF);
  hp = buf;
  off_t off;
  for(off = 0;off < sizeof(buf);++off) {
    if(*(int*)hp == helper) {
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
void drawMenu() {
  // TODO:implement this
}
#endif //__CHEATER__H
