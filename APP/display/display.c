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


void _save_raw_maze(struct maze_t maze, uint8_t *buf, uint32_t cols, uint32_t pages, uint16_t block)
{
#define POS(x, y) ((x) + (y) * maze.cols)

  for (uint32_t p = 0; p < pages; p++) {
    for (uint32_t gc = 0; gc < cols / block; gc += 1) {
      // page 起点所在的 block
      uint32_t gr_bg = p * PAGE / block;
      // page 终点 (仍在 page 内) 所在的 block
      uint32_t gr_ed = ((p + 1) * PAGE - 1) / block;
      // page 起点在 block 中的偏移量
      // 因为 page 起点有可能在 block 的中间
      uint32_t offset = p * PAGE % block;

      // 当前 block 起点在 buf 中的位置
      uint8_t *pptr  = buf + p * BSP_OLED_SCR_COLS + gc * block;
      uint16_t pdata = 0;

      for (uint32_t gr = gr_bg; gr <= gr_ed; gr++) {
        if (gr >= maze.rows)
          break;

        // block 个 1 代表一个墙点
        uint8_t tmp = (1 << block) - 1;
        tmp *= maze.grid[POS(gc, gr)] < 0;
        pdata |= tmp << (gr - gr_bg) * block >> offset;
      }

      memset(pptr, (uint8_t)pdata, MIN(block, BSP_OLED_SCR_COLS - p * block));
    }
  }
}


void _erode_horizontal(uint8_t *buf, uint32_t cols, uint32_t pages, uint16_t erode)
{
  for (uint32_t p = 0; p < pages; p++) {
    uint8_t *pptr = buf + p * BSP_OLED_SCR_COLS;
    // 侵蚀 erode 次
    for (uint16_t _ = 0; _ < erode; _++) {
      uint8_t before = 0xff;
      uint8_t *prev  = &before;
      // 将每一列与前一列比较
      // 当有 bit 不一样时, 全部置为 0
      // 最后一列除外
      for (uint32_t c = 0; c < cols; c++) {
        uint8_t tmp = *(pptr + c);
        *(pptr + c) &= before;
        *prev &= tmp;
        prev   = pptr + c;
        before = tmp;
      }
    }
  }
}


void _erode_vertical(uint8_t *buf, uint32_t cols, uint32_t rows, uint32_t pages, uint16_t erode)
{
  for (uint32_t c = 0; c < cols; c++) {
    // 临时缓冲区
    // 长度应 >= pages
    // pages 最大值为 8
    uint8_t tbuf[8];
#define pptr(p) (buf + (p) * BSP_OLED_SCR_COLS + (c))

    for (uint32_t p = 0; p < pages; p++) {
      // 将前一页, 当前页与后一页放在 16 位容器中操作
      // 前一页 4bits, 当前页 8bits, 后一页 4bits
      // 所以侵蚀宽度最大为 4
      uint16_t tmp    = 0;
      uint32_t offset = (rows - 1) % PAGE + 1;

      // 当 page 为 0 时
      // 前一页 page-1 不存在
      // 特殊处理, 全部设为 1
      if (p == 0)
        tmp |= (1 << 4) - 1;
      else
        // 前一页只取高 4 位
        tmp |= *pptr(p - 1) >> 4;

      tmp |= (uint16_t)*pptr(p) << 4;

      // 最后一页单独处理
      // 因为可能高位有空白
      if (p == pages - 1)
        tmp |= ~0 >> (offset + 4) << (offset + 4);
      else
        // 后一页只取低 4 位
        tmp |= (uint16_t)*pptr(p + 1) << 12;

      // 侵蚀 erode 次
      for (uint16_t _ = 0; _ < erode; _++) {
        tmp &= tmp << 1 & tmp;
        tmp &= tmp >> 1 & tmp;
      }

      if (p == pages - 1)
        // 删除侵蚀时多余的 1
        tbuf[p] = (tmp >> 4) & ((1 << offset) - 1);
      else
        tbuf[p] = (tmp >> 4) & (uint8_t)(~0);
    }

    for (uint32_t p = 0; p < pages; p++)
      *pptr(p) = tbuf[p];
  }
}


void APP_DISPLAY_SaveMazeBackground(uint8_t *buf, struct maze_t maze, uint16_t block, uint16_t erode)
{
  uint32_t pages = MIN(CEIL(maze.rows * block, PAGE), BSP_OLED_SCR_PAGES);
  uint32_t cols  = MIN(maze.cols * block, BSP_OLED_SCR_COLS);
  uint32_t rows  = MIN(maze.rows * block, BSP_OLED_SCR_ROWS);
  _save_raw_maze(maze, buf, cols, pages, block);
  _erode_horizontal(buf, cols, pages, erode);
  _erode_vertical(buf, cols, rows, pages, erode);
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
