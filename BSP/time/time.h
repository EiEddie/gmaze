#ifndef STM32_BSP_TIME_TIME_H_
#define STM32_BSP_TIME_TIME_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


uint32_t BSP_TIME_GetMillis();
uint64_t BSP_TIME_GetMicros();
void BSP_TIME_Delay(uint32_t interval);


#ifdef __cplusplus
}
#endif

#endif // STM32_BSP_TIME_TIME_H_
