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
#define ZOM_HP_OFF 0x54
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
    {"coins", -0xfd2060},     {"gargantuar", 0xc4c6c4},
    {"cabbage", 0xc44460},    {"cannon", 0xc4467c},
    {"zombies_type", -0x50},  {"zombies_pos_x", -0x48},
    {"zombies_pos_y", -0x44}, {"zombies_row", -0x64},
    {"zombies_fly", 0x44},    {"zombies_hp", 0x54},
    {"plants_col", 0x10},     {"plants_vis", -0xc},
    {"plants_row", -0x8},     {"plants_type", 0xc},
    {"plants_hp", 0x28},      {"plants_attack", 0x30},
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
  return off;
}
#endif //__PVZ_OFFSET__H
