/*
 * File    : elfutils.h
 * Project : PVZ
 * Author  : ze00
 * Email   : zerozakiGeek@gmail.com
 * Date    : 2017-08-15
 * Module  :
 * License : MIT
 */
#ifndef __ELFUTILS__H
#define __ELFUTILS__H
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/sysconf.h>
#include <sys/mman.h>
size_t padding(size_t size) {
  unsigned long pagesize = sysconf(_SC_PAGESIZE);
  return size + (pagesize - (size & pagesize - 1) & pagesize - 1);
}
const Elf32_Shdr *getSectionTableHeader(Elf32_Ehdr *hdr) {
  return (Elf32_Shdr *)((const char *)hdr + hdr->e_shstrndx * hdr->e_shentsize +
                        hdr->e_shoff);
}
const char *getSectionName(const char *base, Elf32_Shdr *sec) {
  return base + sec->sh_name;
}
int is(const char *stbl, Elf32_Shdr *sec, const char *name) {
  return strcmp(getSectionName(stbl, sec), name) == 0;
}
void *getRawHandle(int fd, size_t *memsz) {
  struct stat st;
  fstat(fd, &st);
  *memsz = padding(st.st_size);
  return mmap(NULL, *memsz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}
void dumpAt(char *rawHandle, Elf32_Shdr *sec, const char *stbl, int num) {
  char *sym = rawHandle + sec->sh_offset;
  size_t i = sec->sh_size;
  while (i) {
    if (*(int *)sym == num) {
      printf("found at offset %p (%p,%s)\n", (void *)(sym - rawHandle),
             (void *)sec->sh_offset, getSectionName(stbl, sec));
    }
    ++sym;
    --i;
  }
}
void reportVaddr(const char *stbl, Elf32_Shdr *sec) {
  printf("%s at Vaddr:%p\n", getSectionName(stbl, sec), (void *)sec->sh_addr);
}
#endif //__ELFUTILS__H
