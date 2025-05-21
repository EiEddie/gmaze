#include "stm32f1xx_hal.h"

#include "time.h"


uint32_t BSP_TIME_GetMillis()
{
  return HAL_GetTick();
}


uint64_t BSP_TIME_GetMicros()
{
  uint32_t m0        = HAL_GetTick();
  __IO uint32_t u0   = SysTick->VAL;
  uint32_t m1        = HAL_GetTick();
  __IO uint32_t u1   = SysTick->VAL;
  const uint32_t tms = SysTick->LOAD + 1;

  if (m1 != m0) {
    return (m1 * 1000 + ((tms - u1) * 1000) / tms);
  } else {
    return (m0 * 1000 + ((tms - u0) * 1000) / tms);
  }
}


void BSP_TIME_Delay(uint32_t interval)
{
  HAL_Delay(interval);
}
