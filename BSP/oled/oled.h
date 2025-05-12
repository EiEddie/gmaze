#ifndef STM32_BSP_OLED_OLED_H_
#define STM32_BSP_OLED_OLED_H_

#include <stdint.h>

#include "i2c/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif


#define BSP_OLED_SCR_PAGES 8
#define BSP_OLED_SCR_COLS  128
#define BSP_OLED_SCR_ROWS  64

struct BSP_OLED_TypeDef {
  BSP_I2C_E i2c_handle;
  uint16_t addr;
};


void BSP_OLED_Init(struct BSP_OLED_TypeDef device);
void BSP_OLED_TurnOn(struct BSP_OLED_TypeDef device);
void BSP_OLED_TurnOff(struct BSP_OLED_TypeDef device);
void BSP_OLED_Clean(struct BSP_OLED_TypeDef device);
void BSP_OLED_PageDisplay(struct BSP_OLED_TypeDef device, uint32_t page, uint32_t start, uint8_t *data, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif // STM32_BSP_OLED_OLED_H_
