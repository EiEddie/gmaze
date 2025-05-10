#ifndef STM32_BSP_UART_UART_H_
#define STM32_BSP_UART_UART_H_

#include "stm32f1xx_ll_usart.h"
#include "bspdef.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UART_BUFFER_SIZE 1024


typedef enum {
  E_UART_1,
  E_UART_END,
} BSP_UART_E;

#define UART_ENUM_SIZE (size_t)(E_UART_END)


void BSP_UART_Init(BSP_UART_E device, UART_HandleTypeDef *handle);
void BSP_UART_Send(BSP_UART_E device, uint8_t *data, uint16_t len);
void BSP_UART_Printf(BSP_UART_E device, const char *fmt, ...);
void BSP_UART_SetCallback(BSP_UART_E device, void (*cb)(BSP_UART_E device, uint8_t *data, uint16_t len));


#ifdef __cplusplus
}
#endif

#endif // STM32_BSP_UART_UART_H_
