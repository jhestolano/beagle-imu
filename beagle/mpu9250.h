#ifndef MPU9250_H
#define MPU9250_H

#include <stdlib.h>

#define DEV_OK (0)
#define DEV_NOT_OK (-1)
#define DEV_NULL_PTR (-2)
#define DEV_TX_ERR (-3)
#define DEV_OPEN_ERR (-4)
#define DEV_PARAM_ERR (-5)
#define DEV_DATA_READ_ERR (-6)

#define MPU9250_REG_SELF_TEST_X_GYRO (0x00u)
#define MPU9250_REG_SELF_TEST_Y_GYRO (0x01u)
#define MPU9250_REG_SELF_TEST_Z_GYRO (0x02u)
#define MPU9250_REG_SELF_TEST_X_ACCEL (0x0Du)
#define MPU9250_REG_SELF_TEST_Y_ACCEL (0x0Eu)
#define MPU9250_REG_SELF_TEST_Z_ACCEL (0x0Fu)
#define MPU9250_REG_XG_OFFSET_H (0x13u)
#define MPU9250_REG_XG_OFFSET_L (0x14u)
#define MPU9250_REG_YG_OFFSET_H (0x15u)
#define MPU9250_REG_YG_OFFSET_L (0x16u)
#define MPU9250_REG_ZG_OFFSET_H (0x17u)
#define MPU9250_REG_ZG_OFFSET_L (0x18u)
#define MPU9250_REG_SMPRT_DIV (0x19u)
#define MPU9250_REG_CONFIG (0x1Au)
#define MPU9250_REG_GYRO_CONFIG (0x1Bu)
#define MPU9250_REG_ACCEL_CONFIG (0x1Cu)
#define MPU9250_REG_ACCEL_CONFIG2 (0x1Du)
#define MPU9250_REG_LP_ACCEL_ODR (0x1Eu)
#define MPU9250_REG_WOM_THR (0x1Fu)
#define MPU9250_REG_MOTION_DURATION (0x20u)
#define MPU9250_REG_FIFO_EN (0x23u)
#define MPU9250_REG_I2C_MST_CTRL (0x24u)
#define MPU9250_REG_I2C_MST_STATUS (0x36u)
#define MPU9250_REG_I2C_MST_DELAY_CTRL (0x67u)
#define MPU9250_REG_I2C_SLV0_ADDR (0x25u)
#define MPU9250_REG_I2C_SLV0_REG (0x26u)
#define MPU9250_REG_I2C_SLV0_CTRL (0x27u)
#define MPU9250_REG_I2C_SLV0_DO (0x63u)
#define MPU9250_REG_I2C_SLV1_ADDR (0x28u)
#define MPU9250_REG_I2C_SLV1_REG (0x29u)
#define MPU9250_REG_I2C_SLV1_CTRL (0x2Au)
#define MPU9250_REG_I2C_SLV1_DO (0x64u)
#define MPU9250_REG_I2C_SLV2_ADDR (0x2Bu)
#define MPU9250_REG_I2C_SLV2_REG (0x2Cu)
#define MPU9250_REG_I2C_SLV2_CTRL (0x2Du)
#define MPU9250_REG_I2C_SLV2_DO (0x65u)
#define MPU9250_REG_I2C_SLV3_ADDR (0x2Eu)
#define MPU9250_REG_I2C_SLV3_REG (0x2Fu)
#define MPU9250_REG_I2C_SLV3_CTRL (0x30u)
#define MPU9250_REG_I2C_SLV3_DO (0x66u)
#define MPU9250_REG_I2C_SLV4_ADDR (0x31u)
#define MPU9250_REG_I2C_SLV4_REG (0x32u)
#define MPU9250_REG_I2C_SLV4_CTRL (0x34u)
#define MPU9250_REG_I2C_SLV4_DO (0x33u)
#define MPU9250_REG_I2C_SLV4_DI (0x35u)
#define MPU9250_REG_EXT_SENS_DATA_00 (0x49u)
#define MPU9250_REG_INT_PIN_CFG (0x37u)
#define MPU9250_REG_INT_ENABLE (0x38u)
#define MPU9250_REG_INT_STATUS (0x3Au)
#define MPU9250_REG_ACCEL_XOUT_H (0x3Bu)
#define MPU9250_REG_ACCEL_XOUT_L (0x3Cu)
#define MPU9250_REG_ACCEL_YOUT_H (0x3Du)
#define MPU9250_REG_ACCEL_YOUT_L (0x3Eu)
#define MPU9250_REG_ACCEL_ZOUT_H (0x3Fu)
#define MPU9250_REG_ACCEL_ZOUT_L (0x40u)
#define MPU9250_REG_TEMP_OUT_H (0x41u)
#define MPU9250_REG_TEMP_OUT_L (0x42u)
#define MPU9250_REG_GYRO_XOUT_H (0x43u)
#define MPU9250_REG_GYRO_XOUT_L (0x44u)
#define MPU9250_REG_GYRO_YOUT_H (0x45u)
#define MPU9250_REG_GYRO_YOUT_L (0x46u)
#define MPU9250_REG_GYRO_ZOUT_H (0x47u)
#define MPU9250_REG_GYRO_ZOUT_L (0x48u)
#define MPU9250_REG_SIGNAL_PATH_RESET (0x68u)
#define MPU9250_REG_MOT_DETECT_CTRL (0x69u)
#define MPU9250_REG_USER_CTRL (0x6Au)
#define MPU9250_REG_PWR_MGMT_1 (0x6Bu)
#define MPU9250_REG_PWR_MGMT_2 (0x6Cu)
#define MPU9250_REG_BANK_SEL (0x6Du)
#define MPU9250_REG_MEM (0x6Fu)
#define MPU9250_REG_PROGRAM_START (0x70u)
#define MPU9250_REG_FIFO_COUNTH (0x72u)
#define MPU9250_REG_FIFO_COUNTL (0x73u)
#define MPU9250_REG_R_W (0x74u)
#define MPU9250_REG_WHO_AM_I (0x75u)
#define MPU9250_REG_XA_OFFSET_H (0x77u)
#define MPU9250_REG_XA_OFFSET_L (0x78u)
#define MPU9250_REG_YA_OFFSET_H (0x7Au)
#define MPU9250_REG_YA_OFFSET_L (0x7Bu)
#define MPU9250_REG_ZA_OFFSET_H (0x7Du)
#define MPU9250_REG_ZA_OFFSET_L (0x7Eu)
#define MPU9250_REG_ADDR_NUM (0x7Du)

#define MPU9250_BUFF(size)                                                     \
  (struct mpu9250_buff) {                                                      \
    .mem = calloc((size), 1u), .cap = (size), .count = 0u,                     \
  }

#define MPU9250_DEFAULT_CONFIG                                                 \
  (struct mpu9250_cfg) {                                                       \
    .dev_name = "/dev/i2c-2", .addr = 0x68u, .nbus = 2u,                       \
    .gyro_scaling = GYRO_SCALE_1000_DPS, .accel_scaling = ACCEL_SCALE_4_G,     \
  }

struct mpu9250_buff {
  char *mem;
  int cap;
  int count;
};

struct vec3u {
  unsigned int x, y, z;
};

struct vec3f {
  float x, y, z;
};

enum accel_scale {
  ACCEL_SCALE_2_G = 0u,
  ACCEL_SCALE_4_G,
  ACCEL_SCALE_8_G,
  ACCEL_SCALE_16_G,
  ACCEL_SCALE_INVALID,
};

enum gyro_scale {
  GYRO_SCALE_250_DPS = 0u,
  GYRO_SCALE_500_DPS,
  GYRO_SCALE_1000_DPS,
  GYRO_SCALE_2000_DPS,
  GYRO_SCALE_INVALID,
};

struct mpu9250_cfg {
  const char *dev_name;
  unsigned int addr;
  unsigned int nbus;
  enum gyro_scale gyro_scaling;
  enum accel_scale accel_scaling;
};

struct mpu9250;

struct mpu9250 *mpu9250_init(struct mpu9250_cfg cfg);

int mpu9250_deinit(struct mpu9250 *dev);

int mpu9250_read_reg(struct mpu9250 *dev, char reg, char *data);

int mpu9250_write_reg(struct mpu9250 *dev, char reg, char data);

char mpu9250_who_am_i(struct mpu9250 *dev);

int mpu9250_read_all(struct mpu9250 *dev, struct mpu9250_buff *buff);

int mpu9250_get_accel_data(struct mpu9250 *dev, struct vec3u *accel);

int mpu9250_get_gyro_data(struct mpu9250 *dev, struct vec3u *gyro);

int mpu9250_get_temp_data(struct mpu9250 *dev, unsigned int *temp);

int mpu9250_get_accel(struct mpu9250 *dev, struct vec3f *accel);

int mpu9250_get_gyro(struct mpu9250 *dev, struct vec3f *gyro);

int mpu9250_get_temp(struct mpu9250 *dev, float *temp);

#endif
