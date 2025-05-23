#ifndef STM32_APP_GAME_GAME_H_
#define STM32_APP_GAME_GAME_H_

#include <stdint.h>

#include "maze/maze.h"
#include "oled/oled.h"

#ifdef __cplusplus
extern "C" {
#endif


enum APP_GAME_Preset_E {
  E_GAME_3x = 0,
  E_GAME_4x,
  E_GAME_5x,
  E_GAME_6x,
  E_GAME_7x,
  E_GAME_8x,
  E_GAME_END,
};


struct player_t {
  uint8_t *fig;
  uint32_t x, y;
  float vx, vy;
};


struct game_t {
  struct maze_t maze;
  uint8_t *background;
  uint16_t block;

  struct player_t player;
};


void APP_GAME_SetPlayerVelocity(struct game_t *game, float vx, float vy);
void APP_GAME_AddPlayerVelocity(struct game_t *game, float vx, float vy);

void APP_GAME_Init(struct game_t *game, uint32_t cols, uint32_t rows, uint32_t block);
void APP_GAME_InitPreset(struct game_t *game, enum APP_GAME_Preset_E preset);
void APP_GAME_Free(struct game_t *game);

/**
 * \brief 按速度移动人物
 *
 * \return 是否移动, 以及移动方向
 * \retval -1 未移动
 * \retval 0 [0b00] RIGHT
 * \retval 1 [0b01] DOWN
 * \retval 2 [0b10] LEFT
 * \retval 3 [0b11] UP
 */
int8_t APP_GAME_Move(struct game_t *game, float dt);

void APP_GAME_Flush(struct game_t *game, struct BSP_OLED_TypeDef device);
void APP_GAME_Update(struct game_t *game, struct BSP_OLED_TypeDef device, float dt);


#ifdef __cplusplus
}
#endif

#endif // STM32_APP_GAME_GAME_H_
