#ifndef STM32_BSP_I2C_I2C_H_
#define STM32_BSP_I2C_I2C_H_

#include "stm32f1xx_ll_i2c.h"
#include "bspdef.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
  E_I2C_1,
  E_I2C_2,
  E_I2C_END,
} BSP_I2C_E;

#define I2C_ENUM_SIZE (size_t)(E_I2C_END)


void BSP_I2C_Init(BSP_I2C_E device, I2C_HandleTypeDef *handle);
uint8_t BSP_I2C_IsReady(BSP_I2C_E device);
void BSP_I2C_MasterSend(BSP_I2C_E device, uint16_t dev_addr, uint8_t *data, uint16_t len);
void BSP_I2C_MemWrite(BSP_I2C_E device, uint16_t dev_addr, uint16_t data_addr, uint8_t *data, uint16_t len);
void BSP_I2C_MemRead(BSP_I2C_E device, uint16_t dev_addr, uint16_t data_addr, uint8_t *buf, uint16_t len);


#ifdef __cplusplus
}
#endif

#endif // STM32_BSP_I2C_I2C_H_
