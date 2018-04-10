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
#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#ifdef __GNUC__
#define EXPECT(x, y) __builtin_expect(x, y)
#else
#define EXPECT(x, y) x
#endif

// dirty hack for FreeBSD
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#define PTRACE_ATTACH PT_ATTACH
#define PTRACE_DETACH PT_DETACH
#define PTRACE_PEEKDATA PT_READ_D
#define PTRACE_POKEDATA PT_WRITE_D
#endif

#include "scanmem.h"
/* progress handling */
#define NUM_DOTS (10)
#define NUM_SAMPLES (100)
#define MAX_PROGRESS (1.0) /* 100% */
#if (!NUM_DOTS || !NUM_SAMPLES || NUM_SAMPLES % NUM_DOTS != 0)
#error Invalid NUM_DOTS to NUM_SAMPLES proportion!
#endif
#define SAMPLES_PER_DOT (NUM_SAMPLES / NUM_DOTS)
#define PROGRESS_PER_SAMPLE (MAX_PROGRESS / NUM_SAMPLES)

/* ptrace peek buffer, used by peekdata() */
/* make it larger in order to reduce shift */
/* #define MAX_PEEKBUF_SIZE (4*sizeof(int64_t)) */
#define MAX_PEEKBUF_SIZE (1024)
static struct {
  uint8_t cache[MAX_PEEKBUF_SIZE]; /* read from ptrace()  */
  unsigned size;                   /* number of entries (in bytes) */
  const char *base;                /* base address of cached region */
  pid_t pid;                       /* what pid this applies to */
} peekbuf;

#define show_error printf
#define show_info printf
bool sm_attach(pid_t target) {
  int status;

  /* attach to the target application, which should cause a SIGSTOP */
  if (ptrace(PTRACE_ATTACH, target, NULL, NULL) == -1L) {
    show_error("failed to attach to %d, %s\n", target, strerror(errno));
    return false;
  }

  /* wait for the SIGSTOP to take place. */
  if (waitpid(target, &status, 0) == -1 || !WIFSTOPPED(status)) {
    show_error("there was an error waiting for the target to stop.\n");
    show_info("%s\n", strerror(errno));
    return false;
  }

  /* flush the peek buffer */
  memset(&peekbuf, 0x00, sizeof(peekbuf));

  /* everything looks okay */
  return true;
}

bool sm_detach(pid_t target) {
  // addr is ignored on Linux, but should be 1 on FreeBSD in order to let the
  // child process continue execution where it had been interrupted
  return ptrace(PTRACE_DETACH, target, 1, 0) == 0;
}

/* read region using /proc/pid/mem */
ssize_t readregion(pid_t target, void *buf, size_t count,
                   unsigned long offset) {
  char mem[32];
  int fd;
  ssize_t len;

  /* print the path to mem file */
  snprintf(mem, sizeof(mem), "/proc/%d/mem", target);

  /* attempt to open the file */
  if ((fd = open(mem, O_RDONLY)) == -1) {
    show_error("unable to open %s.\n", mem);
    return -1;
  }

  /* try to honor the request */
  len = pread64(fd, buf, count, offset);

  /* clean up */
  close(fd);

  return len;
}

bool sm_read_array(pid_t target, const void *addr, char *buf, int len) {
  if (sm_attach(target) == false) {
    return false;
  }

#if HAVE_PROCMEM
  unsigned nread = 0;
  ssize_t tmpl;
  while (nread < len) {
    if ((tmpl = readregion(target, buf + nread, len - nread,
                           (unsigned long)(addr + nread))) == -1) {
      /* no, continue with whatever data was read */
      break;
    } else {
      /* some data was read */
      nread += tmpl;
    }
  }

  if (nread < len) {
    sm_detach(target);
    return false;
  }

  return sm_detach(target);
#else
  int i;
  /* here we just read long by long, this should be ok for most of time */
  /* partial hit is not handled */
  for (i = 0; i < len; i += sizeof(long)) {
    errno = 0;
    *((long *)(buf + i)) = ptrace(PTRACE_PEEKDATA, target, addr + i, NULL);
    if (EXPECT((*((long *)(buf + i)) == -1L) && (errno != 0), false)) {
      sm_detach(target);
      return false;
    }
  }
  return sm_detach(target);
#endif
}

/* TODO: may use /proc/<pid>/mem here */
bool sm_write_array(pid_t target, void *addr, const void *data, int len) {
  int i, j;
  long peek_value;

  if (sm_attach(target) == false) {
    return false;
  }

  for (i = 0; i + sizeof(long) < len; i += sizeof(long)) {
    if (ptrace(PTRACE_POKEDATA, target, addr + i, *(long *)(data + i)) == -1L) {
      return false;
    }
  }

  if (len - i > 0) /* something left (shorter than a long) */
  {
    if (len > sizeof(long)) /* rewrite last sizeof(long) bytes of the buffer */
    {
      if (ptrace(PTRACE_POKEDATA, target, addr + len - sizeof(long),
                 *(long *)(data + len - sizeof(long))) == -1L) {
        return false;
      }
    } else /* we have to play with bits... */
    {
      /* try all possible shifting read and write */
      for (j = 0; j <= sizeof(long) - (len - i); ++j) {
        errno = 0;
        if (((peek_value = ptrace(PTRACE_PEEKDATA, target, addr - j, NULL)) ==
             -1L) &&
            (errno != 0)) {
          if (errno == EIO || errno == EFAULT) /* may try next shift */
            continue;
          else {
            show_error("%s failed.\n", __func__);
            return false;
          }
        } else /* peek success */
        {
          /* write back */
          memcpy(((int8_t *)&peek_value) + j, data + i, len - i);

          if (ptrace(PTRACE_POKEDATA, target, addr - j, peek_value) == -1L) {
            show_error("%s failed.\n", __func__);
            return false;
          }

          break;
        }
      }
    }
  }

  return sm_detach(target);
}
