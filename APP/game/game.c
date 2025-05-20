#include <math.h>
#include <malloc.h>
#include <string.h>

#include "game.h"
#include "display/display.h"
#include "maze/maze.h"


#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define ABS(x)    ((x) > 0 ? (x) : -(x))


void APP_GAME_SetPlayerVelocity(struct game_t *game, float vx, float vy)
{
  game->player.vx = vx;
  game->player.vy = vy;
}


void APP_GAME_AddPlayerVelocity(struct game_t *game, float vx, float vy)
{
  game->player.vx += vx;
  game->player.vy += vy;
}


void APP_GAME_Init(struct game_t *game, uint32_t cols, uint32_t rows, uint32_t block, uint32_t erode)
{
  game->player.vx = 0;
  game->player.vy = 0;
  game->player.x  = block;
  game->player.y  = block;

  game->block = block;
  maze_init(&game->maze, cols, rows);

  game->background = calloc(BSP_OLED_SCR_COLS * BSP_OLED_SCR_PAGES, sizeof(uint8_t));
  APP_DISPLAY_SaveMazeBackground(game->background, game->maze, block, erode);

  game->player.fig = malloc(block * block);
  memset(game->player.fig, 0xff, block * block);
}


void APP_GAME_Free(struct game_t *game)
{
  maze_free(&game->maze);

  free(game->player.fig);
  game->player.fig = NULL;

  free(game->background);
  game->background = NULL;
}


void APP_GAME_Flush(struct game_t *game, struct BSP_OLED_TypeDef device)
{
  for (int p = 0; p < BSP_OLED_SCR_PAGES; p++) {
    BSP_OLED_PageDisplay(device, p, 0, game->background + p * BSP_OLED_SCR_COLS, BSP_OLED_SCR_COLS);
  }

  APP_DISPLAY_ShowBlockWithBackground(device, game->background, game->player.fig, game->block, game->player.x,
                                      game->player.y);
}


uint8_t APP_GAME_Move(struct game_t *game, float dt)
{
#define POS(x, y) ((x) + (y) * game->maze.cols)
  static uint8_t dir_prev = 0xff;
  uint8_t dir             = 0;
  // 在迷宫 grid 中的位置
  // 按 block 计数
  const uint32_t pos = POS(game->player.x / game->block, game->player.y / game->block);
  float v            = NAN;

  uint8_t is_move = 0;

  if (game->player.y % game->block == 0 && game->player.vx != 0) {
    // y 坐标对齐
    // 可以在 x 方向移动
    is_move++;
    v   = game->player.vx;
    dir = (v < 0) << 1 | 0;
    v   = ABS(v);
  }

  if (game->player.x % game->block == 0 && game->player.vy != 0) {
    is_move++;
    v   = game->player.vy;
    dir = (v < 0) << 1 | 1;
    v   = ABS(v);
  }

  // 两个方向都不能移动
  if (!is_move)
    return 0;

  // 两个方向都可以移动
  // 选速度较大的那个方向
  if (is_move == 2) {
    const float v_s[2] = {game->player.vx, game->player.vy};
    dir                = ABS(game->player.vx) > ABS(game->player.vy) ? 0 : 1;
    v                  = v_s[dir];
    dir                = (v < 0) << 1 | dir;
    v                  = ABS(v);
  }

  // 在这里, 人物一定可以运动
  //
  // 在之前的移动中积累的, 没有被显示出来的位移
  // 没有被显示出来是因为低于屏幕分辨率
  static float dx = 0;
  // 找到 dir 方向能移动的最远距离
  // 当方向改变, 此距离也需重新计算
  static int64_t max = 0;
  // 当移动方向改变, 重新计算距离, 重置累积位移
  if (dir_prev == 0xff || dir_prev != dir) {
    dx         = 0;
    uint32_t p = pos;
    // 找到最远能移动的距离
    while (1) {
      uint32_t tmp = maze_move(&game->maze, p, dir, 1);
      if (tmp == p || game->maze.grid[tmp] < 0)
        break;
      p = tmp;
    }
    uint32_t unit = game->maze.cols * game->block;
    int64_t x = (p % game->maze.cols) * game->block, y = (p / game->maze.cols) * game->block;
    uint32_t tmp = ABS((x - game->player.x) + (y - game->player.y) * unit);
    max          = tmp / unit + tmp % unit;
  }

  dx += v * dt;
  dir_prev = dir;
  // 低于屏幕分辨率
  if (dx < 1)
    return 0;

  // 已经撞到墙
  // 清零对应方向的速度
  if (max <= 0) {
    float *v_s[2] = {&game->player.vx, &game->player.vy};
    *v_s[dir & 1] = 0;
    return 0;
  }

  uint32_t *coord[2] = {&game->player.x, &game->player.y};
  uint32_t delta     = MIN((uint32_t)dx, max);
  dx                 = fmodf(dx, 1);
  *(coord[dir & 1]) += delta * (dir & 0b10 ? -1 : 1);
  max -= delta;
  return 1;
}


void APP_GAME_Update(struct game_t *game, struct BSP_OLED_TypeDef device, float dt)
{
  uint32_t x = game->player.x, y = game->player.y;
  if (!APP_GAME_Move(game, dt))
    return;
  APP_DISPLAY_ShowBlockWithBackground(device, game->background, NULL, game->block, x, y);
  APP_DISPLAY_ShowBlockWithBackground(device, game->background, game->player.fig, game->block, game->player.x,
                                      game->player.y);
}
