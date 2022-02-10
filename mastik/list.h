#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct node_t
{
  struct node_t *next;
  struct node_t *prev;
  int          set;
  size_t       delta;
  char         pad[32]; // up to 64B
} node;

typedef node** list;


int   list_to_array(node *ptr, uint64_t *array);
int   list_length(node *ptr);
node *list_pop(list head);
void  list_push(list head, node *e);
int   list_append(list head, node *e);
int   list_set_first(list head, node *e);
void  list_print(node *ptr);
