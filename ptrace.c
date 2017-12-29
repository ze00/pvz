/*
    Functions to access the memory of the target process.
 
    Copyright (C) 2006,2007,2009 Tavis Ormandy <taviso@sdf.lonestar.org>
    Copyright (C) 2009           Eli Dupree <elidupree@charter.net>
    Copyright (C) 2009,2010      WANG Lu <coolwanglu@gmail.com>
    Copyright (C) 2015           Sebastian Parschauer <s.parschauer@gmx.de>
 
    This file is part of libscanmem.

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

/* for pread */
# ifdef _XOPEN_SOURCE
#  undef _XOPEN_SOURCE
# endif
# define _XOPEN_SOURCE 500

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>
#include <fcntl.h>

#ifdef __GNUC__
# define EXPECT(x,y) __builtin_expect(x, y)
#else
# define EXPECT(x,y) x
#endif

// dirty hack for FreeBSD
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#define PTRACE_ATTACH PT_ATTACH
#define PTRACE_DETACH PT_DETACH
#define PTRACE_PEEKDATA PT_READ_D
#define PTRACE_POKEDATA PT_WRITE_D
#endif

bool sm_attach(pid_t target)
{
  int status;

  /* attach to the target application, which should cause a SIGSTOP */
  if (ptrace(PTRACE_ATTACH, target, NULL, NULL) == -1L) {
    return false;
  }

  /* wait for the SIGSTOP to take place. */
  if (waitpid(target, &status, 0) == -1 || !WIFSTOPPED(status)) {
    return false;
  }

  /* everything looks okay */
  return true;

}

bool sm_detach(pid_t target)
{
  // addr is ignored on Linux, but should be 1 on FreeBSD in order to let the child process continue execution where it had been interrupted
  return ptrace(PTRACE_DETACH, target, 1, 0) == 0;
}

bool sm_read_array(pid_t target, const void *addr, char *buf, int len)
{
  if (sm_attach(target) == false) {
    return false;
  }

  int i;
  /* here we just read long by long, this should be ok for most of time */
  /* partial hit is not handled */
  for(i = 0; i < len; i += sizeof(long))
  {
    errno = 0;
    *((long *)(buf+i)) = ptrace(PTRACE_PEEKDATA, target, addr+i, NULL);
    if (EXPECT((*((long *)(buf+i)) == -1L) && (errno != 0), false)) {
      sm_detach(target);
      return false;
    }
  }
  return sm_detach(target);
}

/* TODO: may use /proc/<pid>/mem here */
bool sm_write_array(pid_t target, void *addr, const void *data, int len)
{
  int i,j;
  long peek_value;

  if (sm_attach(target) == false) {
    return false;
  }

  for (i = 0; i + sizeof(long) < len; i += sizeof(long))
  {
    if (ptrace(PTRACE_POKEDATA, target, addr + i, *(long *)(data + i)) == -1L) {
      return false;
    }
  }

  if (len - i > 0) /* something left (shorter than a long) */
  {
    if (len > sizeof(long)) /* rewrite last sizeof(long) bytes of the buffer */
    {
      if (ptrace(PTRACE_POKEDATA, target, addr + len - sizeof(long), *(long *)(data + len - sizeof(long))) == -1L) {
        return false;
      }
    }
    else /* we have to play with bits... */
    {
      /* try all possible shifting read and write */
      for(j = 0; j <= sizeof(long) - (len - i); ++j)
      {
        errno = 0;
        if(((peek_value = ptrace(PTRACE_PEEKDATA, target, addr - j, NULL)) == -1L) && (errno != 0))
        {
          if (errno == EIO || errno == EFAULT) /* may try next shift */
            continue;
          else
          {
            return false;
          }
        }
        else /* peek success */
        {
          /* write back */
          memcpy(((int8_t*)&peek_value)+j, data+i, len-i);        

          if (ptrace(PTRACE_POKEDATA, target, addr - j, peek_value) == -1L)
          {
            return false;
          }

          break;
        }
      }
    }
  }

  return sm_detach(target);
}
