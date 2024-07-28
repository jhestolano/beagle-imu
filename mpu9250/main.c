#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpu9250.h"

void print_registers(const struct mpu9250_buff* buff) {
  /* Print top. */
  printf("\n\r");
  printf(
      "------------------------------------------------------------------------"
      "-------------\n\r");
  printf(" register dump\n\r");
  printf(
      "------------------------------------------------------------------------"
      "-------------\n\r");
  printf("     ");
  for (int i = 0; i < 0x10; i++) {
    printf(" 0x%02X", i);
  }
  printf("\n\r");

  for (int i = 0; i <= buff->count / 0x10u; i++) {
    printf("0x%02X:", i * 0x10u);
    for (int j = 0; j < 0x10; j++) {
      printf(" 0x%02X", buff->mem[i * 0x10u + j]);
    }
    printf("\n\r");
  }
  printf(
      "------------------------------------------------------------------------"
      "-------------\n\r");
}

int main(void) {
  struct mpu9250_cfg dev_cfg = {
      .dev_name = "/dev/i2c-2",
      .addr = 0x68u,
      .nbus = 2u,
      .gyro_scaling = GYRO_SCALE_1000_DPS,
      .accel_scaling = ACCEL_SCALE_4_G,
  };

  struct mpu9250_buff buff = MPU9250_BUFF(MPU9250_REG_ADDR_NUM);
  struct vec3u raw_vec;
  struct vec3f vec;

  struct mpu9250* dev = mpu9250_init(dev_cfg);
  (void)mpu9250_who_am_i(dev);
  (void)mpu9250_read_all(dev, &buff);

  (void)mpu9250_get_accel_data(dev, &raw_vec);
  (void)mpu9250_get_gyro_data(dev, &raw_vec);
  (void)mpu9250_get_temp_data(dev, &raw_vec.x);

  (void)mpu9250_get_accel(dev, &vec);
  (void)mpu9250_get_gyro(dev, &vec);
  (void)mpu9250_get_temp(dev, &vec.x);

  print_registers(&buff);

  for (int i = 0; i < 30; i++) {
    struct vec3f accel, gyro;
    mpu9250_get_accel(dev, &accel);
    printf("accel[x]: %f\n\r", accel.x);
    printf("accel[y]: %f\n\r", accel.y);
    printf("accel[z]: %f\n\r", accel.z);

    mpu9250_get_gyro(dev, &gyro);
    printf("gyro[x]: %f\n\r", gyro.x);
    printf("gyro[y]: %f\n\r", gyro.y);
    printf("gyro[z]: %f\n\r", gyro.z);
    sleep(1);
  }

  (void)mpu9250_deinit(dev);
  return 0;
}
