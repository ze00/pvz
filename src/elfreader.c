/*
 * File    : elfreader.c
 * Project : PVZ
 * Author  : ze00
 * Email   : zerozakiGeek@gmail.com
 * Date    : 2017-08-14
 * LICENSE : MIT
 * Module  :
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "elfutils.h"
int main(int argc, char **argv) {
  const char *fn = "libpvz.so";
  int num = -1;
  switch (argc) {
  case 3:
    num = atoi(argv[2]);
  case 2:
    fn = argv[1];
  }
  int fd = open(fn, O_RDWR);
  size_t memsz;
  char *rawHandle = getRawHandle(fd, &memsz);
  Elf32_Ehdr *hdr = (Elf32_Ehdr *)rawHandle;
  Elf32_Shdr *sec = (Elf32_Shdr *)(rawHandle + hdr->e_shoff);
  unsigned shnum = hdr->e_shnum;
  const char *stbl = rawHandle + getSectionTableHeader(hdr)->sh_offset;
  while (shnum != 0) {
    if (sec->sh_type == SHT_PROGBITS) {
      if (num != -1) {
        dumpAt(rawHandle, sec, stbl, num);
      } else {
        reportVaddr(stbl, sec);
      }
    }
    ++sec;
    --shnum;
  }
  close(fd);
  munmap(rawHandle, memsz);
  return 0;
}
