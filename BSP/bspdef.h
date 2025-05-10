#ifndef STM32_BSP_BSPDEF_H_
#define STM32_BSP_BSPDEF_H_

#include <stdint.h>
#include <stddef.h>

#include "stm32f1xx_hal.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ERROR_Port LED_GPIO_Port
#define ERROR_Pin  LED_Pin

__attribute__((unused)) static void BSP_Error(const char *file, uint32_t line)
{
  UNUSED(file);
  UNUSED(line);
  HAL_GPIO_TogglePin(ERROR_Port, ERROR_Pin);
  while (1) __NOP();
}

#define BSP_ASSERT(arg)              \
  do {                               \
    if (!(arg))                      \
      BSP_Error(__FILE__, __LINE__); \
  } while (0)


#ifdef __cplusplus
}
#endif

#endif // STM32_BSP_BSPDEF_H_
