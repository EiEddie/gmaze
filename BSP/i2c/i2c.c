#include "i2c.h"


#define TIMEOUT 100

static I2C_HandleTypeDef *_handles[I2C_ENUM_SIZE] = {NULL};


void BSP_I2C_Init(BSP_I2C_E device, I2C_HandleTypeDef *handle)
{
  BSP_ASSERT(device < E_I2C_END);
  BSP_ASSERT(_handles[device] == NULL);
  _handles[device] = handle;
}


uint8_t BSP_I2C_IsReady(BSP_I2C_E device, uint16_t dev_addr)
{
  BSP_ASSERT(_handles[device]);
  uint8_t is_ready = 1;
  is_ready &= HAL_I2C_GetState(_handles[device]) == HAL_I2C_STATE_READY;
  // FIXME: 返回始终为 HAL_ERROR
  // is_ready &= HAL_I2C_IsDeviceReady(_handles[device], dev_addr << 1, 3, TIMEOUT) == HAL_OK;
  return is_ready;
}


void BSP_I2C_MasterSend(BSP_I2C_E device, uint16_t dev_addr, uint8_t *data, uint16_t len)
{
  BSP_ASSERT(_handles[device]);
  HAL_I2C_Master_Transmit(_handles[device], dev_addr, data, len, TIMEOUT);
}


void BSP_I2C_MemWrite(BSP_I2C_E device, uint16_t dev_addr, uint16_t data_addr, uint8_t *data, uint16_t len)
{
  BSP_ASSERT(_handles[device]);
  HAL_I2C_Mem_Write(_handles[device], dev_addr, data_addr, I2C_MEMADD_SIZE_8BIT, data, len, TIMEOUT);
}


void BSP_I2C_MemRead(BSP_I2C_E device, uint16_t dev_addr, uint16_t data_addr, uint8_t *buf, uint16_t len)
{
  BSP_ASSERT(_handles[device]);
  HAL_I2C_Mem_Read(_handles[device], dev_addr, data_addr, I2C_MEMADD_SIZE_8BIT, buf, len, TIMEOUT);
}
