#include "i2c/i2c.h"

#include "oled.h"


void _cmd(struct BSP_OLED_TypeDef device, uint8_t cmd)
{
  BSP_I2C_MemWrite(device.i2c_handle, device.addr, 0x00, &cmd, 1);
}


void _data(struct BSP_OLED_TypeDef device, uint8_t *data, size_t len)
{
  BSP_I2C_MemWrite(device.i2c_handle, device.addr, 0x40, data, len);
}


void BSP_OLED_Init(struct BSP_OLED_TypeDef device)
{
  while (!BSP_I2C_IsReady(device.i2c_handle))
    /* nothing */;

  _cmd(device, 0xAE); // 显示关闭
  _cmd(device, 0x20); // 设置内存寻址模式
  _cmd(device, 0x10); // 00: 水平寻址模式
                      // 01: 垂直寻址模式
                      // 10: 页寻址模式 (复位)
                      // 11: 无效
  _cmd(device, 0xb0); // 设置页寻址模式的页起始地址, 0-7
  _cmd(device, 0xc8); // 设置COM输出扫描方向
  _cmd(device, 0x00); // 设置低列地址
  _cmd(device, 0x10); // 设置高列地址
  _cmd(device, 0x40); // 设置起始行地址
  _cmd(device, 0x81); // 设置对比度控制寄存器
  _cmd(device, 0xff); // 亮度调节 0x00 ~ 0xff
  _cmd(device, 0xa1); // 设置段重新映射 0 到 127
  _cmd(device, 0xa6); // 设置正常显示
  _cmd(device, 0xa8); // 设置复用比例 (1到64)
  _cmd(device, 0x3F); //
  _cmd(device, 0xa4); // 0xa4: 输出遵循RAM内容
                      // 0xa5: 输出忽略RAM内容
  _cmd(device, 0xd3); // 设置显示偏移
  _cmd(device, 0x00); // 不偏移
  _cmd(device, 0xd5); // 设置显示时钟分频比/振荡器频率
  _cmd(device, 0xf0); // 设置分频比
  _cmd(device, 0xd9); // 设置预充电期
  _cmd(device, 0x22); //
  _cmd(device, 0xda); // 设置 COM 引脚硬件配置
  _cmd(device, 0x12); //
  _cmd(device, 0xdb); // 设置 vcomh
  _cmd(device, 0x20); // 0x20: 0.77xVcc
  _cmd(device, 0x8d); // DC-DC 使能
  _cmd(device, 0x14); //
  _cmd(device, 0xaf); // 开启 OLED

  BSP_OLED_Clean(device);
}


void BSP_OLED_TurnOn(struct BSP_OLED_TypeDef device)
{
  _cmd(device, 0X8D); // 设置电荷泵
  _cmd(device, 0X14); // 开启电荷泵
  _cmd(device, 0XAF); // 唤醒 OLED
}


void BSP_OLED_TurnOff(struct BSP_OLED_TypeDef device)
{
  _cmd(device, 0X8D); // 设置电荷泵
  _cmd(device, 0X10); // 关闭电荷泵
  _cmd(device, 0XAE); // OLED 休眠
}


void BSP_OLED_Clean(struct BSP_OLED_TypeDef device)
{
  uint8_t buf[BSP_OLED_SCR_COLS] = {0};
  for (uint32_t page = 0; page < BSP_OLED_SCR_PAGES; page++)
    BSP_OLED_PageDisplay(device, page, 0, buf, BSP_OLED_SCR_COLS);
}


void BSP_OLED_PageDisplay(struct BSP_OLED_TypeDef device, uint32_t page, uint32_t start, uint8_t *data, uint32_t len)
{
  _cmd(device, 0xb0 | page);                  // 换页
  _cmd(device, 0x00 | ((start & 0xf0) >> 4)); // 低位列起始地址
  _cmd(device, 0x10 | (start & 0x0f));        // 高位列起始地址
  _data(device, data, len);
}
