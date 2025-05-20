#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "display.h"
#include "maze/maze.h"
#include "oled/oled.h"


#define PAGE       8

#define MIN(x, y)  ((x) > (y) ? (y) : (x))
#define MAX(x, y)  ((x) > (y) ? (x) : (y))
#define CEIL(x, y) (((x) - 1) / (y) + 1)
#define ABS(x)     ((x) > 0 ? (x) : -(x))


uint8_t _get_page_from_bg(struct maze_t maze, uint32_t page, uint32_t col, uint16_t block, uint16_t erode)
{
  // TODO: 整理逻辑, 添加注释

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


void _get_pages_from_bg(struct maze_t maze, uint32_t page, uint8_t *buf, uint32_t col, uint16_t block, uint16_t erode)
{
  // TODO: 整理逻辑, 添加注释

  uint32_t block_ed = MIN(col / block, maze.cols - 1);

  // 将未经过横向侵蚀的数据放入缓冲区
  uint8_t *offset = buf;
  for (uint32_t b = 0; b <= block_ed; b++) {
    *offset = _get_page_from_bg(maze, page, b * block, block, erode);
    memset(offset, *offset, MIN(block, col - b * block));
    offset += block;
  }

  // 横向侵蚀
  uint8_t before = 0xff;
  uint8_t *prev  = &before;
  for (int _ = 0; _ < erode; _++) {
    // TODO: 检查边界是否越界
    for (int i = 0; i <= (block_ed + 1) * block - 1; i++) {
      uint8_t tmp = buf[i];
      buf[i] &= before;
      *prev &= tmp;
      prev   = buf + i;
      before = tmp;
    }
  }
}


void APP_DISPLAY_SaveMazeBackground(uint8_t *buf, struct maze_t maze, uint16_t block, uint16_t erode)
{
  for (int p = 0; p < MIN(CEIL(block * maze.rows, BSP_OLED_SCR_PAGES), BSP_OLED_SCR_PAGES); p++) {
    // TODO: 更好的写法; 避免使用屏幕数据
    _get_pages_from_bg(maze, p, buf + p * BSP_OLED_SCR_COLS, BSP_OLED_SCR_COLS, block, erode);
  }
}


void APP_DISPLAY_ShowBlockWithBackground(struct BSP_OLED_TypeDef device, uint8_t *background, uint8_t *fig,
                                         uint16_t block, uint32_t x, uint32_t y)
{
  uint32_t offset = y % PAGE;
  uint32_t pages  = CEIL(offset + block, PAGE);

  if (fig == NULL)
    goto empty;

  // 将二维数组的图案重组成 BITSET 所用的缓冲区
  // 缓冲区是宽 block, 高 pages * 8 的二维数组, 以线性组织
  uint8_t *buf = calloc(block * pages * PAGE, sizeof(uint8_t));
  memcpy(buf, fig, block * block);

  // 将 fig 中存储的图像原样转换为 BITSET
  for (int c = 0; c < block; c++) {
    for (int p = 0; p < pages; p++) {
      uint8_t pdata = 0;
      for (int i = 0; i < 8; i++) {
        // 先在竖向按页遍历, 再在每一页中按点遍历
        pdata |= (1 << i) * (!!buf[(p * PAGE + i) * block + c]);
      }
      buf[c + p * block] = pdata;
    }

    // 将 BITSET 向下移动 offset 个单位
    // 为了使 BITSET 恰好在传入的坐标位置
    for (int64_t p = pages - 1; p >= 0; p--) {
      // 将每个页的数据移位 offset
      // 超出 uint8 的部分存入下方一个 page 中
      buf[c + p * block] <<= offset;
      if (p != 0)
        buf[c + p * block] |= buf[c + (p - 1) * block] >> (PAGE - offset);
    }
  }

  // 刷新对应页
  // 最小刷新量: 只会刷新 fig 占用的空间
  // 即 block 宽, pages 个页
  for (int p = 0; p < pages; p++) {
    for (int c = 0; c < block; c++)
      // 将背景叠加上去
      buf[c + p * block] |= *(background + (p + y / PAGE) * BSP_OLED_SCR_COLS + (x + c));
    BSP_OLED_PageDisplay(device, p + y / PAGE, x, buf + p * block, block);
  }

  free(buf);
  return;
empty:;

  for (int p = 0; p < pages; p++) {
    // 只打印背景
    BSP_OLED_PageDisplay(device, p + y / PAGE, x, background + (p + y / PAGE) * BSP_OLED_SCR_COLS + x, block);
  }
}
