#ifndef STM32_UNITS_LIST_LIST_H_
#define STM32_UNITS_LIST_LIST_H_

#include <stddef.h>
#include <stdint.h>


#undef offsetof
#define offsetof(type, member) \
  ((size_t)&((type *)0)->member)


#undef container_of
#define container_of(ptr, type, member)               \
  ({                                                  \
    const typeof(((type *)0)->member) *_mptr = (ptr); \
    (type *)((char *)_mptr - offsetof(type, member)); \
  })


struct list_head_t {
  struct list_head_t *next;
};


void list_init(struct list_head_t *list);
uint8_t list_empty(struct list_head_t *list);
void list_add(struct list_head_t *list,
              struct list_head_t *src);
struct list_head_t *list_pop(struct list_head_t *list);


#endif // STM32_UNITS_LIST_LIST_H_
