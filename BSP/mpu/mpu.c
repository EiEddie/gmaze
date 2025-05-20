#include "mpu.h"
#include "i2c/i2c.h"


#define MPU6050_REG_PWR_MGMT1    0x6B
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_ACCEL_CONFIG 0x1C

#define MPU6050_ACCEL_FS_2G      (0 << 3)
#define MPU6050_ACCEL_SCALE_2G   (1.0f / 16384.0f)


void BSP_MPU_Init(struct BSP_MPU_TypeDef device)
{
  uint8_t data;

  // 退出休眠
  data = 0x00;
  BSP_I2C_MemWrite(device.i2c_handle, device.addr, MPU6050_REG_PWR_MGMT1, &data, 1);

  // 配置加速度量程 ±2g
  data = MPU6050_ACCEL_FS_2G;
  BSP_I2C_MemWrite(device.i2c_handle, device.addr, MPU6050_REG_PWR_MGMT1, &data, 1);
}


vec3f_t BSP_MPU_GetAccel(struct BSP_MPU_TypeDef device)
{
  uint8_t buf[6];
  vec3f_t ret;

  BSP_I2C_MemRead(device.i2c_handle, device.addr, MPU6050_REG_ACCEL_XOUT_H, buf, 6);

  for (int i = 0; i < 3; i++) {
    int16_t raw          = (int16_t)((buf[2 * i] << 8) | buf[2 * i + 1]);
    *((float *)&ret + i) = MPU6050_ACCEL_SCALE_2G * (float)raw;
  }

  return ret;
}
