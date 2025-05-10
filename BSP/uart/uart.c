#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "uart.h"


static uint8_t _uart_tx_buf[UART_BUFFER_SIZE];
static uint8_t _uart_rx_buf[UART_ENUM_SIZE][UART_BUFFER_SIZE];
static UART_HandleTypeDef *_handles[UART_ENUM_SIZE] = {NULL};
static void (*_callbacks[UART_ENUM_SIZE])(BSP_UART_E, uint8_t *, uint16_t);


void BSP_UART_Init(BSP_UART_E device, UART_HandleTypeDef *handle)
{
  BSP_ASSERT(device < E_UART_END);
  BSP_ASSERT(_handles[device] == NULL);
  _handles[device] = handle;
}


void BSP_UART_Send(BSP_UART_E device, uint8_t *data, uint16_t len)
{
  BSP_ASSERT(_handles[device]);
  HAL_UART_Transmit(_handles[device], data, len, HAL_MAX_DELAY);
}


void BSP_UART_Printf(BSP_UART_E device, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  uint16_t len = vsnprintf((char *)_uart_tx_buf, UART_BUFFER_SIZE, fmt, ap);
  va_end(ap);
  BSP_UART_Send(device, _uart_tx_buf, len);
}


void BSP_UART_SetCallback(BSP_UART_E device, void (*cb)(BSP_UART_E, uint8_t *, uint16_t))
{
  BSP_ASSERT(_callbacks[device] == NULL);
  BSP_ASSERT(_handles[device]);
  _callbacks[device] = cb;
  HAL_UARTEx_ReceiveToIdle_DMA(_handles[device], _uart_rx_buf[device], UART_BUFFER_SIZE);
  __HAL_DMA_DISABLE_IT(_handles[device]->hdmarx, DMA_IT_HT);
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *handle, uint16_t len)
{
  for (int i = 0; i < UART_ENUM_SIZE; i++) {
    if (handle != _handles[i])
      continue;
    if (_callbacks[i])
      _callbacks[i](i, _uart_rx_buf[i], len);
    memset(_uart_rx_buf[i], 0, sizeof(uint8_t) * len);
    HAL_UARTEx_ReceiveToIdle_DMA(handle, _uart_rx_buf[i], UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(handle->hdmarx, DMA_IT_HT);
  }
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *handle)
{
  for (int i = 0; i < UART_ENUM_SIZE; i++) {
    if (_handles[i] == NULL)
      continue;
    if (handle != _handles[i])
      continue;
    HAL_UARTEx_ReceiveToIdle_DMA(handle, _uart_rx_buf[i], UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(handle->hdmarx, DMA_IT_HT);
    break;
  }
}
