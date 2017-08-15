 /*
  * File    : pvz.c
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
#include <sys/sysconf.h>
#include <sys/mman.h>
size_t padding(size_t size) {
  unsigned long pagesize = sysconf(_SC_PAGESIZE);
  return size + (pagesize - (size & pagesize - 1) & pagesize - 1);
}
const Elf32_Shdr *getSectionTableHeader(Elf32_Ehdr *hdr) {
  return (Elf32_Shdr *)((const char*)hdr + hdr->e_shstrndx * hdr->e_shentsize + hdr->e_shoff);
}
#if 0
const char *getSectionName(const char *base,Elf32_Shdr *sec) {
  return base + sec->sh_name;
}
#endif
int main(int argc,char **argv) {
  const char *fn = "libpvz.so";
  if(argc == 2)
    fn = argv[1];
  int fd = open(fn,O_RDWR);
  struct stat st;
  stat(fn,&st);
  size_t memsz = padding(st.st_size);
  char *rawHandle = mmap(NULL,memsz,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
  Elf32_Ehdr *hdr = (Elf32_Ehdr *)rawHandle;
  Elf32_Shdr *sec = (Elf32_Shdr *)(rawHandle + hdr->e_shoff);
  unsigned shnum = hdr->e_shnum;
  // const char *stbl = rawHandle + getSectionTableHeader(hdr)->sh_offset;
  int *sym,*ep = NULL;
  while(shnum != 0) {
    // if(strcmp(getSectionName(stbl,sec),".bss") == 0) {
    if(sec->sh_type == SHT_PROGBITS) {
      sym = (int *)(rawHandle + sec->sh_offset);
      for(int i = 0;i < sec->sh_size / sizeof(int);++i) {
        if(sym[i] == 5000 && sym[i-1] == 500) {
          ep = &sym[i];
        }
      }
    }
    ++sec;
    --shnum;
  }
  if(ep != NULL) {
    for(int i = 0;i < 48;++i) {
      *ep = 0;
      // *(ep - 1) = 0;
      ep -= 9;
    }
  }
  close(fd);
  munmap(rawHandle,memsz);
  return 0;
}
