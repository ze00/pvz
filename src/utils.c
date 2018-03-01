/*
 * File    : src/utils.c
 * Project :
 * Author  : ze00
 * Email   : zerozakiGeek@gmail.com
 * Date    : 2018-01-19
 * Module  :
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>
#include <unistd.h>
#include "defs.h"
#include "scanmem.h"
#include "pvz.h"
#include "utils.h"
void *insert(__list **target, size_t len) {
  __list *node = malloc(len);
  node->next = NULL;
  if (*target == NULL) {
    *target = node;
  } else {
    // real是当前链表的尾节点
    ((__list *)(*target)->real)->next = node;
  }
  (*target)->real = node;
  return node;
}
void destroy(__list **node, void (*op)(void *)) {
  __list *helper;
  while (*node != NULL) {
    helper = (*node)->next;
    if (op != NULL)
      op(*node);
    free(*node);
    *node = helper;
  }
  *node = NULL;
}
void insert_task(__task **target, int row, int col) {
  __task *node = insert((__list **)target, sizeof(__task));
  node->row = row;
  node->col = col;
}
void pop(__task **target) {
  __task *helper = next(*target);
  free(*target);
  *target = helper;
}
int has(__task *target, int row, int col) {
  while (target != NULL) {
    if (target->row == row && target->col == col)
      return 1;
    target = next(target);
  }
  return 0;
}
void insert_images(__images **target, int value, void *remote) {
  __images *node = insert((__list **)target, sizeof(__images));
  node->value = value;
  node->remote = remote;
}
void recover_images(__images *node) {
  while (node != NULL) {
    extern void pvz_write(void *, void *, size_t);
    pvz_write(node->remote, &node->value, sizeof(node->value));
    node = next(node);
  }
}
void insert_heaps(__heaps **target, char *base, char *end) {
  __heaps *node = insert((__list **)target, sizeof(__heaps));
  node->base = base;
  node->end = end;
  node->heap_size = end - base;
  printf("Found heap %p ... %p\n", base, end);
  // 这里的malloc做一下检查
  // 因为这里malloc的内存都比较大
  // insert那里malloc基本不可能失败
  node->buf = malloc(node->heap_size);
  if (node->buf == NULL) {
    printf("Cannot allocate '%zu' byte memory...this may is a bug!please contract developer.\n", node->heap_size);
    exit(1);
  }
}
void free_buf(__heaps *heap) { free(heap->buf); }
void destroy_heaps(__heaps **node) {
  destroy((__list **)node, (void (*)(void *))free_buf);
}
void insert_thread_ids(thread_ids **ids, pthread_t id) {
  thread_ids *node = insert((__list **)ids, sizeof(thread_ids));
  node->id = id;
}
void destroy_thread_ids(thread_ids **ids) {
  thread_ids *id = *ids;
  while (id != NULL) {
    // 等待线程退出
    // 或许也必须pthread_join
    // 应该再所有线程都退出后再回到游戏
    // 否则ptrace时回到游戏
    // 游戏已经被暂停了
    pthread_join(id->id, NULL);
    id = next(id);
  }
  destroy((__list **)ids, NULL);
}
void parseRowAndCol(const char *buf, __task **task) {
  const char *val = buf;
  int row, col;
  enum statusMachine {
    NEED_DOT,
    NEED_COMMA,
    NEED_ROW,
    NEED_COL,
  } status = NEED_ROW;
parse:
#define CHECK(stmt)                                                            \
  if (!(stmt)) {                                                               \
    printf("%s", buf);                                                         \
    printf("%*s\n", (int)((val - buf) + 1), "^");                              \
    goto panic;                                                                \
  }
#define DIGIT() (*val - '0')
  if (*val == '\n') {
    if (status != NEED_COMMA) {
      --val;
      CHECK(0);
    }
    goto out;
  }
  while (isspace(*val))
    ++val;
  switch (status) {
  case NEED_ROW:
    CHECK(isdigit(*val) && IN_RANGE(DIGIT(), 1, 6));
    row = DIGIT();
    status = NEED_DOT;
    val++;
    goto parse;
  case NEED_COL:
    CHECK(isdigit(*val) && IN_RANGE(DIGIT(), 1, 9));
    col = DIGIT();
    status = NEED_COMMA;
    val++;
    // 不允许重复
    if (!has(*task, row, col))
      insert_task(task, row, col);
    goto parse;
  case NEED_DOT:
    CHECK('.' == *val);
    status = NEED_COL;
    val++;
    goto parse;
  case NEED_COMMA:
    CHECK(',' == *val);
    status = NEED_ROW;
    val++;
    goto parse;
  }
panic:
  PANIC;
out:
  return;
}
#undef CHECK
#undef DIGIT
void checkRootState() {
  if (getuid() != 0 || getgid() != 0) {
    printf("must run me under root mode\n");
    exit(-1);
  }
}
const char *readline(const Path file) {
  static BufferType buf;
  FILE *fp = fopen(file, "r");
  fgets(buf, BUFSIZE, fp);
  fclose(fp);
  return buf;
}
pid_t findPVZProcess() {
  DIR *dp = opendir("/proc");
  struct dirent *dirHandle;
  pid_t pid = -1;
  Path packageNameProvider;
  while ((dirHandle = readdir(dp))) {
    // 文件名第一个字符是数字
    if (dirHandle->d_type & DT_DIR && isdigit(*dirHandle->d_name)) {
      sprintf(packageNameProvider, "/proc/%s/cmdline", dirHandle->d_name);
      if (strncmp(readline(packageNameProvider), SPECIFIC_PACKAGE,
                  strlen(SPECIFIC_PACKAGE)) == 0) {
        pid = atoi(dirHandle->d_name);
        break;
      }
    }
  }
  closedir(dp);
  return pid;
}
