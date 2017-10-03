/*
 * File    : patcher.c
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
  if (argc == 2)
    fn = argv[1];
  int fd = open(fn, O_RDWR);
  size_t memsz;
  char *rawHandle = getRawHandle(fd, &memsz);
  Elf32_Ehdr *hdr = (Elf32_Ehdr *)rawHandle;
  Elf32_Shdr *sec = (Elf32_Shdr *)(rawHandle + hdr->e_shoff);
  unsigned shnum = hdr->e_shnum;
  int *sym, *ep = NULL;
  while (shnum != 0) {
    if (sec->sh_type == SHT_PROGBITS) {
      sym = (int *)(rawHandle + sec->sh_offset);
      for (int i = 0; i < sec->sh_size / sizeof(int); ++i) {
        if (sym[i] == 5000 && sym[i - 1] == 500) {
          ep = &sym[i];
        }
      }
    }
    ++sec;
    --shnum;
  }
  if (ep != NULL) {
    for (int i = 0; i < 48; ++i) {
      *ep = 0;
      // *(ep - 1) = 0;
      ep -= 9;
    }
  }
  close(fd);
  munmap(rawHandle, memsz);
  return 0;
}
