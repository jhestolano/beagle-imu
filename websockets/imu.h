#ifndef IMU_H
#define IMU_H

struct vec3f {
  double x;
  double y;
  double z;
};

struct imu_data {
  struct vec3f gyro;
  struct vec3f accel;
  struct vec3f mag;
};

struct quaternion {
  double w;
  struct vec3f v;
};

void imu_start(void);

void imu_stop(void);

#endif
