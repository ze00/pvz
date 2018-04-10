/*
 * File    : pvz_offset.h
 * Project : PVZ
 * Author  : ze00
 * Email   : zerozakiGeek@gmail.com
 * Date    : 2017-08-15
 * Module  :
 * License : MIT
 */
#ifndef __PVZ_OFFSET__H
#define __PVZ_OFFSET__H
#include <string.h>
#include <sys/cdefs.h>
#define ZOM_HP_OFF 0xd4
#define PLAN_HP_OFF 0x28
#define PLAN_HP_TOTAL_OFF 0x2c
#define PLAN_ATT_OFF 0x40
#define PLAN_ATT_TOTAL_OFF 0x44
#define PLAN_PEA_ATT_OFF 0x78
#define PLAN_STATUS_OFF 0xa8
struct pvz_offset {
  const char *name;
  off_t offset;
};
// 辅助地址-0xfd2060
struct pvz_offset pvz_off_tbl[] = {
    // 金币的辅助地址
    // 真正的金币地址需要这个辅助地址来计算得出
    // 但在不同的手机上可能找不到真正的金币地址
    {"coins", -0xfd2060},
    // 卷心菜头手的射速
    {"cabbage", 0xc44460},
    // 炮的冷却偏移
    {"cannon", 0xc4467c},
    // 僵尸首地址
    {"zombies_base", -0x80},
    // 僵尸类型代码
    {"zombies_type", 0x30},
    // 僵尸x坐标
    // 相对于格子的坐标
    {"zombies_pos_x", 0x38},
    // 僵尸y坐标
    {"zombies_pos_y", 0x3c},
    // 僵尸的行
    {"zombies_row", 0x1c},
    // 是否飞回家
    {"zombies_back", 0xc4},
    // 僵尸雪亮
    {"zombies_hp", 0xd4},
    // 植物的列
    {"plants_col", 0x10},
    // 植物是否可见
    {"plants_vis", -0xc},
    // 植物的行
    {"plants_row", -0x8},
    // 植物类型代码
    {"plants_type", 0xc},
    // 植物血量
    {"plants_hp", 0x28},
    // 植物攻击力
    {"plants_attack", 0x30},
    // bss + base
    {"base", 0x5138c8},
    // *base + 0x2dfa0
    {"field", 0x2dfa0},
    // field + 0x7c8
    {"status", 0x7c8},
    // status + ...
    {"zombies_list", 0xb4},
    {"zombies_count", 0xc4},
};
off_t getOffset(const char *name) {
  off_t off = -1;
  struct pvz_offset *pf;
  for (size_t i = 0; i < sizeof(pvz_off_tbl) / sizeof(pvz_off_tbl[0]); ++i) {
    pf = &pvz_off_tbl[i];
    if (strcmp(name, pf->name) == 0) {
      off = pf->offset;
      break;
    }
  }
  if (off == -1)
    printf("offset of '%s' not found!\n", name);
  return off;
}
#endif //__PVZ_OFFSET__H
