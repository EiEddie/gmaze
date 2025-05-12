#ifndef STM32_BSP_ADC_ADC_H_
#define STM32_BSP_ADC_ADC_H_

#include <stdint.h>

#include "stm32f1xx_ll_adc.h"

#ifdef __cplusplus
extern "C" {
#endif


void BSP_ADC_Init(ADC_HandleTypeDef *device);
uint32_t BSP_ADC_Get();


#ifdef __cplusplus
}
#endif

#endif // STM32_BSP_ADC_ADC_H_
