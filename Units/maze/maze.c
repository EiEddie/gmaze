#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "maze.h"
#include "list.h"


/// @brief 0: RIGHT; 1: DOWN; 2: LEFT; 3: UP
///
/// 0bXY X: 0=POSITIVE   1=NEGATIVE (DOWN and RIGHT is POSITIVE) \n
///      Y: 0=HORIZONTAL 1=VERTICAL
uint32_t _move(uint32_t pos, uint8_t dir, uint32_t step)
{
  int64_t res = pos + (dir & 1 ? COLS : 1) * (dir & 2 ? -1 : 1) * step;
  if (ROW(pos) != ROW(res) && !(dir & 1))
    return pos;
  if (0 > res || res > COLS * ROWS - 1)
    return pos;
  return res;
}


uint8_t _rand_dirs()
{
  uint8_t res = 0b11100100;
  for (int i = 0; i < 4; i++) {
    int j = random() % 4;
    i *= 2;
    j *= 2;
    uint8_t tmp = 0b11;
    tmp &= (res >> i) ^ (res >> j);
    res ^= tmp << i;
    res ^= tmp << j;
  }
  return res;
}


struct node_t {
  uint32_t pos;
  struct list_head_t list;
};


void maze_init(struct maze_t *maze)
{
  // FIXME
  srandom(0);
  maze->size = COLS * ROWS;
  memset(maze->grid, 0xff, maze->size * sizeof(int8_t));

  int8_t grid_tmp[COLS * ROWS] = {0};
  struct list_head_t walls;
  list_init(&walls);
  struct node_t *rnode = malloc(sizeof(struct node_t));
  rnode->pos           = POS(1, 1);
  list_add(&walls, &rnode->list);

  while (!list_empty(&walls)) {
    struct node_t *nptr = container_of(
        list_pop(&walls), struct node_t, list);
    uint32_t road = nptr->pos;
    free(nptr);
    maze->grid[road] = 1;

    uint8_t dirs = _rand_dirs();
    for (int i = 0; i < 4; i++) {
      uint8_t dir       = dirs >> (2 * i) & 0b11;
      uint32_t road_tmp = _move(road, dir, 2);
      if (road_tmp == road)
        continue;
      if (grid_tmp[road_tmp] != 1)
        continue;
      road_tmp             = (road + road_tmp) / 2;
      grid_tmp[road_tmp]   = 1;
      maze->grid[road_tmp] = 1;
      break;
    }

    grid_tmp[road] = 1;

    for (int d = 0; d < 4; d++) {
      uint32_t wall = _move(road, d, 2);
      if (wall == road)
        continue;
      if (grid_tmp[wall] != 0)
        continue;
      grid_tmp[wall] = -1;
      struct node_t *wnode =
          malloc(sizeof(struct node_t));
      wnode->pos = wall;
      list_add(&walls, &wnode->list);
    }
  }
}
