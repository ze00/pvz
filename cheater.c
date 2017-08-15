 /*
  * File    : cheater.c
  * Project :
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
#include "pvz.h"
#include "pvz_offset.h"
#include "cheater.h"
int main(int argc,char **argv) {
  checkRootState();
  int *base = getDynamicBase();
  int v = 0;
  // printf("%d\n",sm_write_array(pid,(char *)base + getOffset("cannon"),&v,4));
  return 0;
}
