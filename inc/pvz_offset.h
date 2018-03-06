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
    // 场景特效代码的偏移
    {"field_effect_code", 0x2e7d8},
    // FIXME:以下两个偏移是动态的
    // 场景
    {"field_code", 0x78aa40},
    // 冒险关卡进度
    {"adventure_progress", 0x78aa44},
    // 可以找到.bss植物/僵尸的列表
    {"count_entry", 0xc2b780},
    // 于.bbs
    {"zombies_list_actu", 0x8364c},
    {"zombies_count_actu", 0xc07c},
    {"plants_count_actu", 0xc054},
    // 于libpvz
    // +0xC下一个
    {"zombies_list", 0x77660},
    {"zombies_count", 0x9c},
    {"plants_list", 0x776e4},
    {"plants_count", 0x74},
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
