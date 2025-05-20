#include "i2c.h"


#define TIMEOUT 100

static I2C_HandleTypeDef *_handles[I2C_ENUM_SIZE] = {NULL};


void BSP_I2C_Init(BSP_I2C_E device, I2C_HandleTypeDef *handle)
{
  BSP_ASSERT(device < E_I2C_END);
  BSP_ASSERT(_handles[device] == NULL);
  _handles[device] = handle;
}


uint8_t BSP_I2C_IsReady(BSP_I2C_E device)
{
  return _handles[device]->State == HAL_I2C_STATE_READY;
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
