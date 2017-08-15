 /*
  * File    : pvz_offset.h
  * Project :
  * Author  : ze00
  * Email   : zerozakiGeek@gmail.com
  * Date    : 2017-08-15
  * Module  : 
  * License : MIT
  */
#ifndef __PVZ_OFFSET__H
#define __PVZ_OFFSET__H
#include <sys/cdefs.h>
enum off_type {
  OFF_MISC,
  OFF_PLANTS,
};
struct pvz_offset {
  const char *name;
  enum off_type type;
  off_t offset;
};
struct pvz_offset pvz_off_tbl[] = {
  { "coins" , OFF_MISC, -8551532 },
  { "cannon", OFF_PLANTS, 12859004 },
};
off_t getOffset(const char *name) {
  off_t off = -1;
  struct pvz_offset *pf;
  for(unsigned i = 0;i < sizeof(pvz_off_tbl) / sizeof(pvz_off_tbl[0]);++i) {
    pf = &pvz_off_tbl[i];
    if(strcmp(name,pf->name) == 0) {
      off = pf->offset;
      if(pf->type == OFF_PLANTS)
        off += 4096;
      break;
    }
  }
  return off;
}
#endif //__PVZ_OFFSET__H
