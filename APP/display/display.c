#include <stdint.h>
#include <string.h>

#include "display.h"
#include "maze/maze.h"
#include "oled/oled.h"


#define PAGE 8


uint8_t _get_page_from_bg(struct maze_t maze, uint32_t page, uint32_t col, uint16_t block, uint16_t erode)
{
  uint32_t bx     = col / block;
  uint32_t by_st  = page * PAGE / block;
  uint32_t by_ed  = (page * PAGE + PAGE - 1) / block;
  uint32_t offset = block * (by_st + 1) - PAGE * page;

  uint8_t before = 1;
  if (page > 0) {
    uint32_t tmp = (page * PAGE - 1) / block;
    before       = maze.grid[bx + tmp * maze.cols] < 0;
  }
  uint8_t after = 1;
  if ((page + 1) * PAGE / block < maze.rows) {
    uint32_t tmp = (page + 1) * PAGE / block;
    after        = maze.grid[bx + tmp * maze.cols] < 0;
  }

  uint8_t res = 0;
  for (int i = 0; by_st <= by_ed; i++) {
    uint8_t is_wall;
    if (bx >= maze.cols || by_st >= maze.rows)
      is_wall = 0;
    else
      is_wall = maze.grid[bx + by_st * maze.cols] < 0;
    uint16_t tmp = (1 << block) - 1;
    tmp *= is_wall;
    tmp <<= block * i;
    tmp >>= block - offset;
    res |= tmp;
    by_st++;
  }

  for (int _ = 0; _ < erode; _++) {
    uint8_t tmp = res;
    res &= tmp << 1 | before;
    if (by_ed >= maze.rows)
      continue;
    res &= tmp >> 1 | after << 7;
  }

  return res;
}


void APP_DISPLAY_ShowMaze(struct BSP_OLED_TypeDef device, struct maze_t maze, uint16_t block, uint16_t erode)
{
  uint8_t buf[BSP_OLED_SCR_COLS];
  for (int p = 0; p < (block * maze.rows - 1) / BSP_OLED_SCR_PAGES + 1 && p < BSP_OLED_SCR_PAGES; p++) {
    uint8_t before = _get_page_from_bg(maze, p, 0, block, erode);
    uint8_t now    = before;

    for (int c = 0; c < maze.cols; c++) {
      if (c * block + block - 1 >= BSP_OLED_SCR_COLS)
        break;

      uint8_t after = _get_page_from_bg(maze, p, (c + 1) * block, block, erode);
      if (c + 1 >= maze.cols)
        after = now;
      memset(buf, now, block);
      buf[0] &= before;
      buf[block - 1] &= after;
      for (int i = 0; i < erode - 1; i++) {
        buf[i + 1]         = buf[i];
        buf[block - i - 2] = buf[block - i - 1];
      }

      BSP_OLED_PageDisplay(device, p, c * block, buf, block);
      before = buf[block - 1];
      now    = after;
    }
  }
}
