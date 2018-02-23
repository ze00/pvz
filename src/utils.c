/*
 * File    : src/utils.c
 * Project :
 * Author  : ze00
 * Email   : zerozakiGeek@gmail.com
 * Date    : 2018-01-19
 * Module  :
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include "defs.h"
#include "utils.h"
void insert(__task **target, int row, int col) {
  __task *node = malloc(sizeof(__task));
  node->row = row;
  node->col = col;
  next(node) = NULL;
  if (*target == NULL) {
    *target = node;
    real(*target) = node;
  } else {
    next((__task *)real(*target)) = node;
    real(*target) = node;
  }
}
void destroy(void **rawnode, void (*op)(void *)) {
  __list **node = (__list **)rawnode;
  __list *helper;
  while (*node != NULL) {
    helper = (*node)->next;
    if (op != NULL)
      op(*node);
    free(*node);
    *node = helper;
  }
  if (*node != NULL)
    (*node)->real = NULL;
  *node = NULL;
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
void insert_images(__images **target, int attack, void *remote) {
  __images *node = malloc(sizeof(__images));
  node->attack = attack;
  node->remote = remote;
  next(node) = NULL;
  if (*target == NULL) {
    *target = node;
    real(*target) = node;
  } else {
    next((__images *)real(*target)) = node;
    real(*target) = node;
  }
}
void recover_images(__images *node) {
  while (node != NULL) {
    extern void pvz_write(void *, void *, size_t);
    pvz_write(node->remote, &node->attack, sizeof(node->attack));
    node = next(node);
  }
}
void insert_heaps(__heaps **heap, char *base, char *end) {
  __heaps *node = malloc(sizeof(__heaps));
  node->base = base;
  node->end = end;
  next(node) = NULL;
  node->heap_size = end - base;
  printf("Found heap %p ... %p\n", base, end);
  node->buf = malloc(node->heap_size);
  if (node->buf == NULL) {
    printf("Cannt allocate '%zu' byte memory...\n", node->heap_size);
  }
  if (*heap == NULL) {
    *heap = node;
    real(*heap) = node;
  } else {
    next((__heaps *)real(*heap)) = node;
    real(*heap) = node;
  }
}
void free_buf(__heaps *heap) { free(heap->buf); }
void destroy_heaps(__heaps **node) {
  destroy((void **)node, (void (*)(void *))free_buf);
}
void parseRowAndCol(const char *buf, __task **head) {
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
    insert(head, row, col);
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
