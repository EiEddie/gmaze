#ifndef STM32_APP_DISPLAY_DISPLAY_H_
#define STM32_APP_DISPLAY_DISPLAY_H_

#include "oled/oled.h"
#include "maze/maze.h"

#ifdef __cplusplus
extern "C" {
#endif


void APP_DISPLAY_ShowMaze(struct BSP_OLED_TypeDef device, struct maze_t maze, uint16_t block);


#ifdef __cplusplus
}
#endif

#endif // STM32_APP_DISPLAY_DISPLAY_H_
