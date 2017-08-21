 /*
  * File    : pvz.h
  * Project : PVZ
  * Author  : ze00
  * Email   : zerozakiGeek@gmail.com
  * Date    : 2017-08-15
  * Module  : 
  * License : MIT
  */
#ifndef __PVZ__H
#define __PVZ__H
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <limits.h>
#include <setjmp.h>
#include <unistd.h>
#include <scanmem/scanmem.h>
#include "base.h"
int isReadable(Path path) {
  return access(path,R_OK) == 0;
}
pid_t findPVZProcess(ProcessDIR processDIR) {
  DIR *dp = opendir("/proc");
  FILE *fp;
  struct dirent *dirHandle;
  pid_t pid = -1;
  Path packageNameProvider;
  BufferType buf;
  while((dirHandle = readdir(dp))) {
    if(isdigit(*dirHandle->d_name)) {
      sprintf(processDIR,"/proc/%s",dirHandle->d_name);
      sprintf(packageNameProvider,"%s/cmdline",processDIR);
      if(isReadable(packageNameProvider)) {
        fp = fopen(packageNameProvider,"r");
        fgets(buf,BUFSIZE,fp);
        if(strcmp(buf,SPECIFIC_PACKAGE) == 0) {
          pid = atoi(dirHandle->d_name);
          fclose(fp);
          break;
        }
        fclose(fp);
      }
    }
  }
  closedir(dp);
  return pid;
}
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
jmp_buf env;
#endif //__PVZ__H
