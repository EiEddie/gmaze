#ifndef STM32_UNITS_MAZE_MAZE_H_
#define STM32_UNITS_MAZE_MAZE_H_


struct maze_t {
  uint32_t cols, rows;
  /// 0: undef; 1: road; -1: wall
  int8_t *grid;
};

void maze_init(struct maze_t *maze, uint32_t cols, uint32_t rows);
void maze_free(struct maze_t* maze);


#endif // STM32_UNITS_MAZE_MAZE_H_
