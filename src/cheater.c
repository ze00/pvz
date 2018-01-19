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
#include <ctype.h>
#include "defs.h"
#include "base.h"
#include "pvz.h"
#include "pvz_offset.h"
#include "cheater.h"
int main(int argc, char **argv) {
  checkRootState();
  int *base = getDynamicBase();
  baseInfo.base = (char *)base;
  baseInfo.heap_base = (char *)getHeapBase();
#ifdef DEBUG
  printf("Dynamic base %p,heap base %p\n", baseInfo.base, baseInfo.heap_base);
#endif
  baseInfo.heap_buf =
      createBuf(baseInfo.heap_end, baseInfo.heap_base, &baseInfo.heap_size);
  int option;
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
    puts("13.退出");
#define PANIC                                                                  \
  do {                                                                         \
    printf("无效输入\n");                                                      \
    raise(SIGINT);                                                             \
  } while (0)

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
        findZombies(letZombiesFragile);
        usleep(500000);
      }
      break;
    case 3:
      increaseCabbagePult();
      break;
    case 4:
      removeColdDown();
      break;
    case 5:
      findZombies(coverZombies);
      break;
    case 6:
      findZombies(increaseZombies);
      break;
    case 7:
      findPlants(reportPlants);
      break;
    case 8:
      findZombies(report);
      break;
    case 9:
      findPlants(increasePlants);
      break;
    case 10:
      findPlants(increasePlantsAttack);
      break;
    case 11: {
      BufferType buf;
      const char *val = buf;
      int row, col;
      enum statusMachine {
        NEED_DOT,
        NEED_COMMA,
        NEED_ROW,
        NEED_COL,
      } status = NEED_ROW;
      printf("要将梯子僵尸放于何列?\n例如:1.2,1.3,(行与列以英文句号分隔,"
             "多个行列以英文逗号分隔)");
      setbuf(stdin, NULL);
      if (fgets(buf, sizeof(buf), stdin) == NULL)
        PANIC;
    parse:
#define CHECK(stmt)                                                            \
  if (!(stmt)) {                                                               \
    printf("%s", buf);                                                         \
    printf("%*s\n", val - buf + 1, "^");                                       \
    goto panic;                                                                \
  }
#define DIGIT() (*val - '0')
      if (*val == '\n') {
        if (status != NEED_COMMA) {
          --val;
          CHECK(false);
        }
        goto putladder;
      }
      while (isspace(*val))
        ++val;
      switch (status) {
      case NEED_ROW:
        CHECK(isdigit(*val) && IN_RANGE(DIGIT(), 1, 6));
        row = DIGIT();
        status = NEED_DOT;
        val++;
        goto parse;
      case NEED_COL:
        CHECK(isdigit(*val) && IN_RANGE(DIGIT(), 1, 9));
        col = DIGIT();
        status = NEED_COMMA;
        val++;
        if (col != 1) {
          insert(&baseInfo.task_helper, row, col);
          if (baseInfo.task == NULL)
            baseInfo.task = baseInfo.task_helper;
          else
            baseInfo.task_helper = baseInfo.task_helper->next;
        } else {
          printf("WARNING:忽略%d.%d\n", row, col);
        }
        goto parse;
      case NEED_DOT:
        CHECK('.' == *val);
        status = NEED_COL;
        val++;
        goto parse;
      case NEED_COMMA:
        CHECK(',' == *val);
        status = NEED_ROW;
        val++;
        goto parse;
      }
    panic:
      PANIC;
    putladder:
      while (baseInfo.task != NULL) {
        findZombies(putLadder);
        usleep(250000);
      }
    } break;
    case 12: {
    } break;
    case 13:
      free(baseInfo.heap_buf);
      return 0;
    default:
      printf("输入错误...\n");
    }
  }
  free(baseInfo.heap_buf);
  return 0;
}
