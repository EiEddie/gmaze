#include <stdint.h>

#include "list.h"


void list_init(struct list_head_t *list)
{
  list->next = NULL;
}


uint8_t list_empty(struct list_head_t *list)
{
  return list->next == NULL;
}


void list_add(struct list_head_t *list,
              struct list_head_t *src)
{
  src->next  = list->next;
  list->next = src;
}


struct list_head_t *list_pop(struct list_head_t *list)
{
  struct list_head_t *tmp = list->next;
  list->next              = tmp->next;
  return tmp;
}
