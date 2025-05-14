#include <stdint.h>
#include <string.h>

#include "display.h"
#include "maze/maze.h"
#include "oled/oled.h"


#define PAGE       8

#define MIN(x, y)  ((x) > (y) ? (y) : (x))
#define CEIL(x, y) (((x) - 1) / (y) + 1)
#define ABS(x)     ((x) > 0 ? (x) : -(x))


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


void _get_pages_from_bg(struct maze_t maze, uint32_t page, uint8_t *buf, uint32_t col_bg, uint32_t col_ed,
                        uint16_t block, uint16_t erode)
{
  uint8_t before = 0xff;
  if (col_bg != 0)
    before = _get_page_from_bg(maze, page, col_bg - 1, block, erode);

  *buf            = _get_page_from_bg(maze, page, col_bg, block, erode);
  uint8_t *offset = buf + block - col_bg % block;
  memset(buf, *buf, offset - buf);

  for (int c = col_bg + block; c <= col_ed; c += block) {
    *offset = _get_page_from_bg(maze, page, c, block, erode);
    memset(offset, *offset, MIN(block, col_ed - c + 1));
    offset += block;
  }

  uint8_t *prev = &before;
  for (int i = 0; i < MIN(col_ed - col_bg + 1, maze.cols * block); i++) {
    uint8_t tmp = buf[i];
    buf[i] &= before;
    *prev &= tmp;
    prev   = buf + i;
    before = tmp;
  }
}


void APP_DISPLAY_ShowMaze(struct BSP_OLED_TypeDef device, struct maze_t maze, uint16_t block, uint16_t erode)
{
  uint8_t buf[BSP_OLED_SCR_COLS];
  for (int p = 0; p < MIN(CEIL(block * maze.rows, BSP_OLED_SCR_PAGES), BSP_OLED_SCR_PAGES); p++) {
    _get_pages_from_bg(maze, p, buf, 0, BSP_OLED_SCR_COLS - 1, block, erode);
    BSP_OLED_PageDisplay(device, p, 0, buf, MIN(block * maze.cols, BSP_OLED_SCR_COLS));
  }
}


void _draw(uint8_t *buf, uint32_t a)
{
  memset(buf, 1, a * a);
}


void APP_DISPLAY_ShowCircle(struct BSP_OLED_TypeDef device, struct maze_t maze, uint16_t block, uint16_t erode,
                            uint32_t x, uint32_t y)
{
  uint8_t buf[128];
  _draw(buf, block);

  uint32_t pages  = CEIL(block, PAGE) + 1;
  uint32_t offset = y % PAGE;
  for (int c = 0; c < block; c++) {
    for (int p = 0; p < pages - 1; p++) {
      uint8_t pdata = 0;
      for (int i = 0; i < 8; i++) {
        if (p * PAGE + i >= block)
          break;
        pdata |= (1 << i) * (1 & buf[c + (p * PAGE + i) * block]);
      }
      buf[c + p * block] = pdata;
    }

    buf[c + (pages - 1) * block] = 0;
    for (int p = pages - 1; p >= 0; p--) {
      buf[c + p * block] <<= offset;
      if (p != 0)
        buf[c + p * block] |= buf[c + (p - 1) * block] >> (PAGE - offset);
    }
  }

  uint8_t bg[128];
  for (int p = 0; p < pages; p++) {
    _get_pages_from_bg(maze, p + y / PAGE, bg, x, x + block, block, erode);
    for (int c = 0; c < block; c++)
      buf[c + p * block] |= bg[c];
    BSP_OLED_PageDisplay(device, p + y / PAGE, x, buf + p * block, block);
  }
}
