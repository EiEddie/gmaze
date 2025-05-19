#ifndef STM32_APP_DISPLAY_DISPLAY_H_
#define STM32_APP_DISPLAY_DISPLAY_H_

#include "oled/oled.h"
#include "maze/maze.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief 将迷宫的图案存入缓冲区
 *
 * \param block 迷宫中一个路点或墙点在屏幕上显示的像素块边长.
 * \param erode 侵蚀宽度, 在路两旁侵蚀这个宽度的墙壁.
 * 例如原本 5 格宽的路, 经过 1 宽度的侵蚀后, 两边各增加 1 宽度, 变为 7 宽.
 *
 * \note 被侵蚀后的墙壁宽度必须 >= 2, 否则会在跨页时产生问题. 函数中未进行判断.
 */
void APP_DISPLAY_SaveMazeBackground(uint8_t *buf, struct maze_t maze, uint16_t block, uint16_t erode);

void APP_DISPLAY_Show(struct BSP_OLED_TypeDef device, uint8_t *buf);

void APP_DISPLAY_ShowBlockWithBackground(struct BSP_OLED_TypeDef device, uint8_t *background, uint8_t *fig,
                                         uint16_t block, uint32_t x, uint32_t y);


#ifdef __cplusplus
}
#endif

#endif // STM32_APP_DISPLAY_DISPLAY_H_
