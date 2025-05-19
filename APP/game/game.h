#ifndef STM32_APP_GAME_GAME_H_
#define STM32_APP_GAME_GAME_H_

#include <stdint.h>

#include "maze/maze.h"
#include "oled/oled.h"

#ifdef __cplusplus
extern "C" {
#endif


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

void APP_GAME_Init(struct game_t *game, uint32_t cols, uint32_t rows, uint32_t block, uint32_t erode);
void APP_GAME_Free(struct game_t *game);
uint8_t APP_GAME_Move(struct game_t *game, float dt);
void APP_GAME_Flush(struct game_t *game, struct BSP_OLED_TypeDef device);
void APP_GAME_Update(struct game_t *game, struct BSP_OLED_TypeDef device, float dt);


#ifdef __cplusplus
}
#endif

#endif // STM32_APP_GAME_GAME_H_
