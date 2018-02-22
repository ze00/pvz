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
  node->next = NULL;
  if (*target == NULL) {
    *target = node;
  } else {
    (*target)->next = node;
  }
}
void destroy(__task **node) {
  __task *helper;
  while (*node != NULL) {
    helper = (*node)->next;
    free(*node);
    *node = helper;
  }
  *node = NULL;
}
void pop(__task **target) {
  __task *helper = (*target)->next;
  free(*target);
  *target = helper;
}
int has(__task *target, int row, int col) {
  while (target != NULL) {
    if (target->row == row && target->col == col)
      return 1;
    target = target->next;
  }
  return 0;
}
void insert_images(__images **target, int attack, void *remote) {
  __images *node = malloc(sizeof(__images));
  node->attack = attack;
  node->remote = remote;
  node->next = NULL;
  if (*target == NULL) {
    *target = node;
  } else {
    (*target)->next = node;
  }
}
void destroy_images(__images **node) {
  __images *helper;
  while (*node != NULL) {
    helper = (*node)->next;
    free(*node);
    *node = helper;
  }
  *node = NULL;
}
void recover_images(__images *node) {
  while (node != NULL) {
    extern void pvz_write(void *, void *, size_t);
    pvz_write(node->remote, &node->attack, sizeof(node->attack));
    node = node->next;
  }
}
void parseRowAndCol(const char *buf, __task **head, __task **helper) {
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
    insert(helper, row, col);
    if (*head == NULL)
      *head = *helper;
    else
      *helper = (*helper)->next;
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
