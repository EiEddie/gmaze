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


void APP_GAME_Init(struct game_t *game, uint32_t cols, uint32_t rows, uint32_t block)
{
  game->player.vx = 0;
  game->player.vy = 0;
  game->player.x  = block;
  game->player.y  = block;

  game->block = block;
  maze_init(&game->maze, cols, rows);

  game->background = calloc(BSP_OLED_SCR_COLS * BSP_OLED_SCR_PAGES, sizeof(uint8_t));
  APP_DISPLAY_SaveMazeBackground(game->background, game->maze, block, MIN(block / 2 - 1, 2));

  game->player.fig = malloc(block * block);
  memset(game->player.fig, 0xff, block * block);
}


void APP_GAME_InitPreset(struct game_t *game, enum APP_GAME_Preset_E preset)
{
  const struct {
    uint32_t cols, rows, block;
  } presets[E_GAME_END] = {{41, 21, 3}, {31, 15, 4}, {25, 13, 5}, {21, 11, 6}, {17, 9, 7}, {15, 7, 8}};
  APP_GAME_Init(game, presets[preset].cols, presets[preset].rows, presets[preset].block);
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


int8_t APP_GAME_Move(struct game_t *game, float dt)
{
  float *v_s[2]    = {&game->player.vx, &game->player.vy};
  uint32_t *r_s[2] = {&game->player.x, &game->player.y};

#define POS(x, y) ((x) + (y) * game->maze.cols)
  // 在迷宫 grid 中的位置
  // 按 block 计数
  const uint32_t pos = POS(*r_s[0] / game->block, *r_s[1] / game->block);

  static uint8_t dir_prev = 0xff;
  int8_t dir              = 0;
  // 方向对齐则可以移动
  switch (!(*r_s[0] % game->block) * 2 + !(*r_s[1] % game->block)) {
    case 0b10:
      // x 坐标对齐
      // 可以在 y 方向移动
      dir = 1;
      break;
    case 0b01:
      // y 坐标对齐
      // 可以在 x 方向移动
      dir = 0;
      break;
    case 0b11:
      // 两个方向都可以移动
      // 选速度较大的那个方向
      dir = ABS(*v_s[0]) < ABS(*v_s[1]);
      break;
    case 0b00:
      return -1;
  }

  float v;
  v   = *v_s[dir];
  dir = (v < 0) << 1 | dir;
  v   = ABS(v);

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
    int64_t x     = (p % game->maze.cols) * game->block;
    int64_t y     = (p / game->maze.cols) * game->block;
    uint32_t tmp  = ABS((x - *r_s[0]) + (y - *r_s[1]) * unit);
    max           = tmp / unit + tmp % unit;
  }

  dx += v * dt;
  dir_prev = dir;

  // 低于屏幕分辨率
  if (dx < 1)
    return -1;

  // 已经撞到墙
  // 清零对应方向的速度
  if (max <= 0) {
    *v_s[dir & 1] = 0;
    return -1;
  }

  uint32_t delta = MIN((uint32_t)dx, max);
  dx             = fmodf(dx, 1);
  *r_s[dir & 1] += delta * (dir & 0b10 ? -1 : 1);
  max -= delta;
  return dir;
}


void APP_GAME_Update(struct game_t *game, struct BSP_OLED_TypeDef device, float dt)
{
#define PAGE           8
#define BLOCK_ALIGN(x) ((x) / game->block * game->block)
#define SET_0(x, y)    (*(game->background + (y) / PAGE * BSP_OLED_SCR_COLS + (x)) &= ~(1 << (y) % PAGE))

  // 移动前的位置
  uint32_t r_s[2] = {game->player.x, game->player.y};
  int8_t dir      = APP_GAME_Move(game, dt);
  if (dir < 0)
    return;
  // 移动后的位置
  uint32_t rr_s[2] = {game->player.x, game->player.y};

  // 移动方向正负
  // 右下正，左上负
  int64_t sgn = dir & 0b10 ? -1 : 1;

  // 两帧之间必然直线移动
  // 在该方向上移动
  int64_t r_mv_s[2] = {r_s[0], r_s[1]};
  for (; (r_mv_s[dir & 1] - rr_s[dir & 1]) * sgn <= 0; r_mv_s[dir & 1] += sgn * game->block) {
    // 将迷宫中标记未经过的点去除
    SET_0(BLOCK_ALIGN(r_mv_s[0]) + game->block / 2, BLOCK_ALIGN(r_mv_s[1]) + game->block / 2);
  }

  APP_DISPLAY_ShowBlockWithBackground(device, game->background, NULL, game->block, r_s[0], r_s[1]);
  APP_DISPLAY_ShowBlockWithBackground(device, game->background, game->player.fig, game->block, rr_s[0], rr_s[1]);
}
