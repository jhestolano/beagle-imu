#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "mpu9250.h"

#define WHO_AM_I (0x71u)

#define ACCEL_2G_SCALING (16384.0f)
#define GYRO_250_DPS_SCALING (131.072f)

static const float accel_scaling_map[] = {
    32767.0f / 2.0f,   /* 2G scaling */
    32767.0f / 4.0f,   /* 4G scaling */
    32767.0f / 8.0f,   /* 8G scaling */
    32767.0f / 16.0f,  /* 16G scaling */
    99999999999999.0f, /* Invalid scaling */
};

static const float gyro_scaling_map[] = {
    32767.0f / 250.0f,  /* 250dps scaling */
    32767.0f / 500.0f,  /* 500dps scaling */
    32767.0f / 1000.0f, /* 1000dps scaling */
    32767.0f / 2000.0f, /* 2000dps scaling */
    99999999999999.0f,  /* Invalid scaling */
};

#define CATCH_NULL_PTR(x)                      \
  {                                            \
    if ((x) == NULL) {                         \
      fprintf(stderr, "Invalid null ptr\n\r"); \
      return DEV_NULL_PTR;                     \
    }                                          \
  }

#define TX_BUFF_SIZE (8u)
#define RX_BUFF_SIZE (MPU9250_REG_ADDR_NUM)

#define BYTES_TO_WORD(x, y) \
  (unsigned int)(((unsigned char)x) << 8u) | (unsigned int)((unsigned char)y)

struct mpu9250 {
  int fhandle;
  struct mpu9250_cfg cfg;
  struct mpu9250_buff rx_buff;
  struct mpu9250_buff tx_buff;
};

struct mpu9250* mpu9250_init(struct mpu9250_cfg cfg) {
  struct mpu9250* dev = calloc(1u, sizeof(struct mpu9250));

  if (!dev) {
    fprintf(stderr, "could not allocate memory\n\r");
    return NULL;
  }

  dev->rx_buff = (struct mpu9250_buff){0u};
  dev->tx_buff = (struct mpu9250_buff){0u};

  dev->rx_buff.mem = calloc(MPU9250_REG_ADDR_NUM, sizeof(char));
  dev->tx_buff.mem = calloc(8u, sizeof(char));

  if (!dev->rx_buff.mem || !dev->tx_buff.mem) {
    fprintf(stderr, "could not allocate memory\n\r");
    mpu9250_deinit(dev);
    return NULL;
  }

  dev->rx_buff.cap = RX_BUFF_SIZE;
  dev->tx_buff.cap = TX_BUFF_SIZE;

  dev->fhandle = open(cfg.dev_name, O_RDWR);
  if (dev->fhandle < 0) {
    fprintf(stderr, "could not open %s\n\r", cfg.dev_name);
    mpu9250_deinit(dev);
    return NULL;
  }

  if (ioctl(dev->fhandle, I2C_SLAVE, cfg.addr) < 0) {
    fprintf(stderr, "could not configure device %u\n\r", cfg.addr);
    return NULL;
  }

  if (mpu9250_who_am_i(dev) != WHO_AM_I) {
    fprintf(stderr, "whoami register not correct. comm error?\n\r");
    return NULL;
  }

  /* Write accelerometer and gyro scaling. */
  char config_reg = (char)((unsigned int)cfg.accel_scaling << 3u);
  mpu9250_write_reg(dev, MPU9250_REG_ACCEL_CONFIG, config_reg);

  config_reg = (char)((unsigned int)cfg.gyro_scaling << 3u);
  mpu9250_write_reg(dev, MPU9250_REG_GYRO_CONFIG, 0u);

  dev->cfg = cfg;
  fprintf(stdout, "init ok\n\r");
  return dev;
}

int mpu9250_deinit(struct mpu9250* dev) {
  CATCH_NULL_PTR(dev);

  close(dev->fhandle);
  free(dev->tx_buff.mem);
  free(dev->rx_buff.mem);
  dev->fhandle = 0;
  dev->rx_buff = (struct mpu9250_buff){0u};
  dev->tx_buff = (struct mpu9250_buff){0u};
  return DEV_OK;
}

int mpu9250_read_reg(struct mpu9250* dev, char reg, char* data) {
  CATCH_NULL_PTR(dev);
  CATCH_NULL_PTR(data);

  if (dev->fhandle == 0) {
    fprintf(stderr, "device not open!\n\r");
    return DEV_OPEN_ERR;
  }

  char buff[1];
  buff[0] = reg;
  fprintf(stdout, "reading register 0x%X\n\r", buff[0]);

  int n_bytes = write(dev->fhandle, buff, 1u);
  if (n_bytes != 1u) {
    fprintf(stderr, "error sending read command!\n\r");
    fprintf(stderr, "wrote %d bytes\n\r", n_bytes);
    return DEV_TX_ERR;
  }
  dev->rx_buff.count = read(dev->fhandle, data, 1u);
  fprintf(stdout, "received: 0x%X\n\r", *data);
  return DEV_OK;
}

int mpu9250_write_reg(struct mpu9250* dev, char reg, char data) {
  CATCH_NULL_PTR(dev);
  if (dev->fhandle == 0) {
    fprintf(stderr, "device not open!\n\r");
    return DEV_OPEN_ERR;
  }

  char buff[] = {reg, data};
  fprintf(stdout, "writing register 0x%X with value: 0x%X\n\r", reg, data);
  int n_bytes = write(dev->fhandle, buff, 2u);
  if (n_bytes != 2u) {
    fprintf(stderr, "error sending write command!\n\r");
    fprintf(stderr, "wrote %d bytes\n\r", n_bytes);
    return DEV_TX_ERR;
  }
  return DEV_OK;
}

char mpu9250_who_am_i(struct mpu9250* dev) {
  CATCH_NULL_PTR(dev);
  char reg_data;
  if (DEV_OK != mpu9250_read_reg(dev, MPU9250_REG_WHO_AM_I, &reg_data)) {
    fprintf(stderr, "who_am_i call failed\n\r");
    return 0xFFu;
  }
  fprintf(stdout, "whoami: 0x%X\n\r", reg_data);
  return reg_data;
}

int mpu9250_read_all(struct mpu9250* dev, struct mpu9250_buff* buff) {
  CATCH_NULL_PTR(dev);
  CATCH_NULL_PTR(buff);

  if (buff->cap < MPU9250_REG_ADDR_NUM) {
    fprintf(stderr, "output buffer not big enough\n\r");
    return DEV_PARAM_ERR;
  }

  /* Start from first register */
  dev->rx_buff.count = 0u;
  for (int i = 0; i < MPU9250_REG_ADDR_NUM; i++) {
    if (write(dev->fhandle, &i, 1u) < 0) {
      fprintf(stderr, "error sending read all command!\n\r");
      return DEV_TX_ERR;
    }
    dev->rx_buff.count += read(dev->fhandle, &dev->rx_buff.mem[i], 1u);
  }

  /* Copy into output buffer. */
  memcpy(buff->mem, dev->rx_buff.mem, MPU9250_REG_ADDR_NUM);
  buff->count = MPU9250_REG_ADDR_NUM;

  // fprintf(stdout, "read 0x%X bytes.\n\r", dev->rx_buff.count);
  // fprintf(stdout, "0x75 reg: 0x%X\n\r", dev->rx_buff.mem[0x75u]);
  return DEV_OK;
}

int mpu9250_get_accel_data(struct mpu9250* dev, struct vec3u* accel) {
  CATCH_NULL_PTR(dev);
  CATCH_NULL_PTR(accel);

  int status = DEV_OK;
  char reg_data[2u] = {0u};

  status |= mpu9250_read_reg(dev, MPU9250_REG_ACCEL_XOUT_H, &reg_data[0u]);
  status |= mpu9250_read_reg(dev, MPU9250_REG_ACCEL_XOUT_L, &reg_data[1u]);
  accel->x = BYTES_TO_WORD(reg_data[0u], reg_data[1u]);

  status |= mpu9250_read_reg(dev, MPU9250_REG_ACCEL_YOUT_H, &reg_data[0u]);
  status |= mpu9250_read_reg(dev, MPU9250_REG_ACCEL_YOUT_L, &reg_data[1u]);
  accel->y = BYTES_TO_WORD(reg_data[0u], reg_data[1u]);

  status |= mpu9250_read_reg(dev, MPU9250_REG_ACCEL_ZOUT_H, &reg_data[0u]);
  status |= mpu9250_read_reg(dev, MPU9250_REG_ACCEL_ZOUT_L, &reg_data[1u]);
  accel->z = BYTES_TO_WORD(reg_data[0u], reg_data[1u]);

  if (DEV_OK != status) {
    fprintf(stderr, "accel register read failed!\n\r");
    return DEV_DATA_READ_ERR;
  }

  fprintf(stdout, "raw_accel[x] = 0x%X\n\r", accel->x);
  fprintf(stdout, "raw_accel[y] = 0x%X\n\r", accel->y);
  fprintf(stdout, "raw_accel[z] = 0x%X\n\r", accel->z);

  return (status == DEV_OK) ? DEV_OK : DEV_NOT_OK;
}

int mpu9250_get_gyro_data(struct mpu9250* dev, struct vec3u* gyro) {
  CATCH_NULL_PTR(dev);
  CATCH_NULL_PTR(gyro);

  int status = DEV_OK;
  char reg_data[2u] = {0u};

  status |= mpu9250_read_reg(dev, MPU9250_REG_GYRO_XOUT_H, &reg_data[0u]);
  status |= mpu9250_read_reg(dev, MPU9250_REG_GYRO_XOUT_L, &reg_data[1u]);
  gyro->x = BYTES_TO_WORD(reg_data[0u], reg_data[1u]);

  status |= mpu9250_read_reg(dev, MPU9250_REG_GYRO_YOUT_H, &reg_data[0u]);
  status |= mpu9250_read_reg(dev, MPU9250_REG_GYRO_YOUT_L, &reg_data[1u]);
  gyro->y = BYTES_TO_WORD(reg_data[0u], reg_data[1u]);

  status |= mpu9250_read_reg(dev, MPU9250_REG_GYRO_ZOUT_H, &reg_data[0u]);
  status |= mpu9250_read_reg(dev, MPU9250_REG_GYRO_ZOUT_L, &reg_data[1u]);
  gyro->z = BYTES_TO_WORD(reg_data[0u], reg_data[1u]);

  if (DEV_OK != status) {
    fprintf(stderr, "gyro register read failed!\n\r");
    return DEV_DATA_READ_ERR;
  }

  fprintf(stdout, "raw_gyro[x] = 0x%X\n\r", gyro->x);
  fprintf(stdout, "raw_gyro[y] = 0x%X\n\r", gyro->y);
  fprintf(stdout, "raw_gyro[z] = 0x%X\n\r", gyro->z);

  return (status == DEV_OK) ? DEV_OK : DEV_NOT_OK;
}

int mpu9250_get_temp_data(struct mpu9250* dev, unsigned int* temp) {
  CATCH_NULL_PTR(dev);
  CATCH_NULL_PTR(temp);

  int status = DEV_OK;
  char reg_data[2u] = {0u};

  status |= mpu9250_read_reg(dev, MPU9250_REG_TEMP_OUT_H, &reg_data[0u]);
  status |= mpu9250_read_reg(dev, MPU9250_REG_TEMP_OUT_L, &reg_data[1u]);
  *temp = BYTES_TO_WORD(reg_data[0u], reg_data[1u]);

  if (DEV_OK != status) {
    fprintf(stderr, "temp register read failed!\n\r");
    return DEV_DATA_READ_ERR;
  }

  fprintf(stdout, "raw_temp = 0x%X\n\r", *temp);
  return (status == DEV_OK) ? DEV_OK : DEV_NOT_OK;
}

int mpu9250_get_accel(struct mpu9250* dev, struct vec3f* accel) {
  CATCH_NULL_PTR(dev);
  CATCH_NULL_PTR(accel);

  struct vec3u raw_accel;
  if (DEV_OK != mpu9250_get_accel_data(dev, &raw_accel)) {
    return DEV_DATA_READ_ERR;
  }

  /* Assumes 2g configuration */
  accel->x =
      ((float)(short)raw_accel.x) / accel_scaling_map[dev->cfg.accel_scaling];
  accel->y =
      ((float)(short)raw_accel.y) / accel_scaling_map[dev->cfg.accel_scaling];
  accel->z =
      ((float)(short)raw_accel.z) / accel_scaling_map[dev->cfg.accel_scaling];

  return DEV_OK;
}

int mpu9250_get_gyro(struct mpu9250* dev, struct vec3f* gyro) {
  CATCH_NULL_PTR(dev);
  CATCH_NULL_PTR(gyro);

  struct vec3u raw_gyro;
  if (DEV_OK != mpu9250_get_gyro_data(dev, &raw_gyro)) {
    return DEV_DATA_READ_ERR;
  }

  /* Assumes 250 deg/s default configuration */
  gyro->x =
      ((float)(short)raw_gyro.x) / gyro_scaling_map[dev->cfg.gyro_scaling];
  gyro->y =
      ((float)(short)raw_gyro.y) / gyro_scaling_map[dev->cfg.gyro_scaling];
  gyro->z =
      ((float)(short)raw_gyro.z) / gyro_scaling_map[dev->cfg.gyro_scaling];

  return DEV_OK;
}

int mpu9250_get_temp(struct mpu9250* dev, float* temp) {
  return DEV_OK;
}
