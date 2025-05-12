#include <stdint.h>
#include <string.h>

#include "display.h"
#include "maze/maze.h"
#include "oled/oled.h"


#define PAGE 8


uint8_t _get_page_from_bg(struct maze_t maze, uint32_t page, uint32_t col, uint16_t block)
{
  uint32_t bx     = col / block;
  uint32_t by_st  = page * PAGE / block;
  uint32_t by_ed  = (page * PAGE + PAGE - 1) / block;
  uint32_t offset = block * (by_st + 1) - PAGE * page;

  uint8_t res = 0;
  for (int i = 0; by_st <= by_ed; i++) {
    uint8_t is_wall = maze.grid[bx + by_st * maze.cols] > 0 ? 0 : 1;
    if (bx >= maze.cols || by_st >= maze.rows)
      is_wall = 0;
    uint16_t tmp = (1 << block) - 1;
    tmp *= is_wall;
    tmp <<= block * i;
    tmp >>= block - offset;
    res |= tmp;
    by_st++;
  }
  return res;
}


void APP_DISPLAY_ShowMaze(struct BSP_OLED_TypeDef device, struct maze_t maze, uint16_t block)
{
  uint8_t buf[BSP_OLED_SCR_COLS];
  for (int p = 0; p < (block * maze.rows - 1) / BSP_OLED_SCR_PAGES + 1 && p < BSP_OLED_SCR_PAGES; p++) {
    for (int c = 0; c < maze.cols; c++) {
      if (c * block + block - 1 >= BSP_OLED_SCR_COLS)
        break;
      memset(buf, _get_page_from_bg(maze, p, c * block, block), block);
      BSP_OLED_PageDisplay(device, p, c * block, buf, block);
    }
  }
}
