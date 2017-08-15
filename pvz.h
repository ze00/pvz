 /*
  * File    : pvz.h
  * Project :
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
#include <unistd.h>
#ifndef SPECIFIC_PACKAGE
#define SPECIFIC_PACKAGE "com.popcap.pvz_na"
#endif
#ifndef SPECIFIC_DYNAMIC_LIBRARIES
#define SPECIFIC_DYNAMIC_LIBRARIES "libpvz.so"
#endif
#define BUFSIZE 255
typedef char BufferType[BUFSIZE];
typedef char ProcessDIR[PATH_MAX];
typedef char Path[PATH_MAX];
int isReadable(Path path) {
  return access(path,R_OK) == 0;
}
int findPVZProcess(ProcessDIR processDIR) {
  DIR *dp = opendir("/proc");
  FILE *fp;
  struct dirent *dirHandle;
  int pid = -1;
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
#endif //__PVZ__H
