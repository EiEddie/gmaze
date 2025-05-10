#ifndef STM32_UNITS_MAZE_MAZE_H_
#define STM32_UNITS_MAZE_MAZE_H_


#define COLS      17
#define ROWS      17

#define COL(p)    ((p) % COLS)
#define ROW(p)    ((p) / COLS)
#define POS(x, y) ((x) + (y) * COLS)

struct maze_t {
  uint32_t size;
  /// 0: undef; 1: road; -1: wall
  int8_t grid[COLS * ROWS];
};

void maze_init(struct maze_t *maze);


#endif // STM32_UNITS_MAZE_MAZE_H_
