#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <stdbool.h>

#include "imu.h"

/* Enums. */
enum conn_status {
  conn_status_init,
  conn_status_ok,
  conn_status_err,
  conn_status_closed,
};

enum data_status {
  data_status_none,
  data_status_pending,
  data_status_overrun,
  data_status_error,
};

/* Structs. */
struct shared_data {
  enum conn_status conn_status;
  enum data_status data_status;
  bool exit_flag;
  struct imu_data imu_data;
};

struct shared_data shared_data_read(void);

void shared_data_write(struct shared_data data);

#endif // !SHARED_DATA_H
