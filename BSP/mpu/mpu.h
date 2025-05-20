#ifndef STM32_BSP_MPU_MPU_H_
#define STM32_BSP_MPU_MPU_H_

#include <stdint.h>

#include "i2c/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif


struct BSP_MPU_TypeDef {
  BSP_I2C_E i2c_handle;
  uint16_t addr;
};


typedef struct {
  float x, y, z;
} vec3f_t;


void BSP_MPU_Init(struct BSP_MPU_TypeDef device);
vec3f_t BSP_MPU_GetAccel(struct BSP_MPU_TypeDef device);


#ifdef __cplusplus
}
#endif

#endif // STM32_BSP_MPU_MPU_H_
