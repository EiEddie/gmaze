#include <stdint.h>
#include <string.h>

#include "display.h"
#include "maze/maze.h"
#include "oled/oled.h"


#define BLOCK 5
#define PAGE  8


uint8_t _get_page_from_bg(struct maze_t maze, uint32_t page, uint32_t col)
{
  uint32_t bx     = col / BLOCK;
  uint32_t by_st  = page * PAGE / BLOCK;
  uint32_t by_ed  = (page * PAGE + PAGE - 1) / BLOCK;
  uint32_t offset = BLOCK * (by_st + 1) - PAGE * page;

  uint8_t res = 0;
  for (int i = 0; by_st <= by_ed; i++) {
    uint8_t is_wall = maze.grid[POS(bx, by_st)] > 0 ? 0 : 1;
    if (bx >= COLS || by_st >= ROWS)
      is_wall = 0;
    uint16_t tmp = (1 << BLOCK) - 1;
    tmp *= is_wall;
    tmp <<= BLOCK * i;
    tmp >>= BLOCK - offset;
    res |= tmp;
    by_st++;
  }
  return res;
}


void APP_DISPLAY_ShowMaze(struct BSP_OLED_TypeDef device, struct maze_t maze)
{
  for (int p = 0; p < (BLOCK * ROWS - 1) / BSP_OLED_SCR_PAGES + 1 && p < BSP_OLED_SCR_PAGES; p++) {
    for (int c = 0; c < COLS; c++) {
      if (c * BLOCK + BLOCK - 1 >= BSP_OLED_SCR_COLS)
        break;
      uint8_t buf[BLOCK] = {_get_page_from_bg(maze, p, c * BLOCK)};
      memset(buf, *buf, BLOCK);
      BSP_OLED_PageDisplay(device, p, c * BLOCK, buf, BLOCK);
    }
  }
}
