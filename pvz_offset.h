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
enum off_type {
  OFF_MISC,
  OFF_PLANTS,
  OFF_ZOMBIES,
};
struct pvz_offset {
  const char *name;
  enum off_type type;
  off_t offset;
};
// 辅助地址-0xfd2060
struct pvz_offset pvz_off_tbl[] = {
  { "coins", OFF_MISC, -0xfd2060 },
  { "gargantuar", OFF_ZOMBIES, 0xc4c6c4 },
  { "cabbage", OFF_PLANTS, 0xc44460 },
  { "cannon", OFF_PLANTS, 0xc4467c },
};
off_t getOffset(const char *name) {
  off_t off = -1;
  struct pvz_offset *pf;
  for(unsigned i = 0;i < sizeof(pvz_off_tbl) / sizeof(pvz_off_tbl[0]);++i) {
    pf = &pvz_off_tbl[i];
    if(strcmp(name,pf->name) == 0) {
      off = pf->offset;
      break;
    }
  }
  return off;
}
#endif //__PVZ_OFFSET__H
