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
void checkRootState() {
  if(getuid() != 0 || getgid() != 0) {
    printf("must run me under root mode\n");
    exit(-1);
  }
}
int *getDynamicBase() {
  ProcessDIR processDIR;
  int pid = findPVZProcess(processDIR);
  if(pid == -1) {
    printf("cannot locate '%s'\n",SPECIFIC_PACKAGE);
    exit(-1);
  }
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
}
void drawMenu() {
  // TODO:implement this
}
#endif //__CHEATER__H
