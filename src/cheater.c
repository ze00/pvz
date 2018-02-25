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
#include <setjmp.h>
#include <strings.h>
#include "defs.h"
#include "utils.h"
#include "base.h"
#include "pvz.h"
#include "cheater.h"
int main(int argc, char **argv) {
  checkRootState();
  baseInfo.base = getDynamicBase();
  getHeapBase();
  if (baseInfo.heap == NULL) {
    printf("Failed to get heap base,please restart the game!\n");
  }
  printf("Dynamic base %p\n", baseInfo.base);
  int option;
  BufferType buf;
  registeSigHandle();
  if (setjmp(env) == SETJMP_RET) {
    puts("");
  }
  while (1) {
    puts("1.改金币");
    puts("2.僵尸秒杀");
    puts("3.卷心菜投手加强");
    puts("4.无冷却");
    puts("5.黄油糊脸");
    puts("6.僵尸加强二倍");
    puts("7.输出植物地址");
    puts("8.输出僵尸地址");
    puts("9.植物血量增加二倍");
    puts("10.植物攻速增加二倍");
    puts("11.搭梯");
    puts("12.炸荷叶烂南瓜");
    puts("13.所有植物不攻击");
    puts("14.恢复攻击");
    puts("15.退出");

#define GETOPT(mess, opt)                                                      \
  printf(mess);                                                                \
  if (scanf("%d", &opt) != 1) {                                                \
    PANIC;                                                                     \
  }
    GETOPT("请输入:", option);
    switch (option) {
    case 1:
      GETOPT("更改为?", baseInfo.val);
      changeCoins();
      break;
    case 2:
      while (1) {
        forEachZombies(letZombiesFragile);
        usleep(WAIT_USECONDS);
      }
      break;
    case 3:
      increaseCabbagePult();
      break;
    case 4:
      removeColdDown();
      break;
    case 5:
      forEachZombies(coverZombies);
      break;
    case 6:
      forEachZombies(increaseZombies);
      break;
    case 7:
      forEachPlants(reportPlants);
      break;
    case 8:
      forEachZombies(report);
      break;
    case 9:
      forEachPlants(increasePlants);
      break;
    case 10:
      forEachPlants(increasePlantsAttack);
      break;
    case 11: {
      printf("要将梯子僵尸放于何列?\n例如:1.2,1.3,(行与列以英文句号分隔,"
             "多个行列以英文逗号分隔)");
      setbuf(stdin, NULL);
      if (fgets(buf, sizeof(buf), stdin) == NULL)
        PANIC;
      parseRowAndCol(buf, &baseInfo.task);
      while (baseInfo.task != NULL) {
        forEachZombies(putLadder);
        usleep(WAIT_USECONDS);
      }
    } break;
    case 12: {
      printf("要去除何处的莲叶或破坏何处的南瓜?(行与列以英文句号分隔,"
             "多个行列以英文逗号分隔");
      setbuf(stdin, NULL);
      if (fgets(buf, sizeof(buf), stdin) == NULL)
        PANIC;
      parseRowAndCol(buf, &baseInfo.task);
      forEachPlants(fuck_LilyPad_Pumpkin);
      destroy((__list **)&baseInfo.task, NULL);
    } break;
    case 13:
      forEachPlants(plants_freeze);
      break;
    case 14:
      forEachPlants(plants_attack);
      break;
    case 15:
      destroy_heaps(&baseInfo.heap);
      return 0;
    default:
      printf("输入错误...\n");
    }
  }
  destroy_heaps(&baseInfo.heap);
  return 0;
}
#undef GETOPT
