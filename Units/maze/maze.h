#ifndef STM32_UNITS_MAZE_MAZE_H_
#define STM32_UNITS_MAZE_MAZE_H_


struct maze_t {
  uint32_t cols, rows;
  /// 0: undef; 1: road; -1: wall
  int8_t *grid;
};

/// @brief 0: RIGHT; 1: DOWN; 2: LEFT; 3: UP
///
/// 0bXY X: 0=POSITIVE   1=NEGATIVE (DOWN and RIGHT is POSITIVE) \n
///      Y: 0=HORIZONTAL 1=VERTICAL
uint32_t maze_move(struct maze_t*maze, uint32_t pos, uint8_t dir, uint32_t step);

void maze_init(struct maze_t *maze, uint32_t cols, uint32_t rows);

void maze_free(struct maze_t *maze);


#endif // STM32_UNITS_MAZE_MAZE_H_
