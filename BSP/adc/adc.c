#include <stdint.h>

#include "adc.h"


static uint32_t val;


void BSP_ADC_Init(ADC_HandleTypeDef *device)
{
  HAL_ADCEx_Calibration_Start(device);
  HAL_ADC_Start_DMA(device, &val, 2);
}


uint32_t BSP_ADC_Get()
{
  return val;
}
