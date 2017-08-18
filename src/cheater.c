 /*
  * File    : cheater.c
  * Project : PVZ
  * Author  : ze00
  * Email   : zerozakiGeek@gmail.com
  * Date    : 2017-08-15
  * Module  : 
  * License : MIT
  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <scanmem/scanmem.h>
#include <setjmp.h>
#include "pvz.h"
#include "pvz_offset.h"
#include "cheater.h"
int main(int argc,char **argv) {
  checkRootState();
  int *base = getDynamicBase();
  baseInfo.base = (char *)base;
  baseInfo.heap_base = (char *)getHeapBase();
  createZombiesBuf();
  int option;
  registeSigHandle();
  setjmp(env);
  while(1) {
    puts("1.改金币");
    puts("2.僵尸秒杀");
    puts("3.卷心菜投手加强");
    puts("4.无冷却");
    puts("5.黄油糊脸");
    puts("6.僵尸加强二倍");
    puts("7.退出");
    printf("请选择:");
    if(scanf("%d",&option) == EOF) {
      // getchar();
      setbuf(stdin,NULL);
      printf("无效输入\n");
      continue;
    }
    switch(option) {
      case 1:
        printf("更改为?");
        if(scanf("%d",&baseInfo.newVal) != EOF) {
          changeCoins();
        } else {
          getchar();
          printf("输入错误...\n");
        }
        break;
      case 2:
        while(1) {
          findAllZombies(letZombiesFragile);
          usleep(500000);
        }
        break;
      case 3:
        increaseCabbageHurler();
        break;
      case 4:
        removeColdDown();
        break;
      case 5:
        findAllZombies(coverZombies);
        break;
      case 6:
        findAllZombies(increaseZombies);
        break;
      case 7:
        free(baseInfo.zombies_buf);
        return 0;
      default:
        printf("输入错误...\n");
    }
  }
  free(baseInfo.zombies_buf);
  return 0;
}
