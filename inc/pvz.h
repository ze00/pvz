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
#include <ctype.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <setjmp.h>
#include <fcntl.h>
#include <unistd.h>
#include "base.h"
#ifndef SPECIFIC_PACKAGE
#define SPECIFIC_PACKAGE "com.popcap.pvz_na"
#endif
#ifndef SPECIFIC_DYNAMIC_LIBRARIES
#define SPECIFIC_DYNAMIC_LIBRARIES "libpvz.so"
#endif
#define COINS_HELPER_BUFF 256
#define COINS_HELPER_OFF 0x7aa300
#define CARDS_PLAN_TPYE_OFF 0x8
#define PUMPKIN_CODE (30)
#define LILYPAD_CODE (16)
int isReadable(Path path) { return access(path, R_OK) == 0; }
pid_t findPVZProcess(ProcessDIR processDIR) {
  DIR *dp = opendir("/proc");
  FILE *fp;
  struct dirent *dirHandle;
  pid_t pid = -1;
  Path packageNameProvider;
  BufferType buf;
  while ((dirHandle = readdir(dp))) {
    // 文件名第一个字符是数字
    if (isdigit(*dirHandle->d_name) && dirHandle->d_type & DT_DIR) {
      sprintf(processDIR, "/proc/%s", dirHandle->d_name);
      sprintf(packageNameProvider, "%s/cmdline", processDIR);
      if (isReadable(packageNameProvider)) {
        fp = fopen(packageNameProvider, "r");
        fgets(buf, BUFSIZE, fp);
        if (strcmp(buf, SPECIFIC_PACKAGE) == 0) {
          pid = atoi(dirHandle->d_name);
          fclose(fp);
          break;
        }
        fclose(fp);
      }
    }
  }
  printf("found %s:%d\n", SPECIFIC_PACKAGE, pid);
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
