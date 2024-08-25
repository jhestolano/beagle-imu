#include "../mongoose/mongoose.h"
#include "imu.h"
#include "shared_data.h"
#include <pthread.h>
#include <stddef.h>

static pthread_t gs_imu_thread;

static struct imu_data imu_from_json(struct mg_str json_str);
static void *imu_ws_thread_fn(void *args);
static void imu_ws_fn(struct mg_connection *conn, int ev, void *ev_data);

void imu_start(void) {
  pthread_create(&gs_imu_thread, NULL, imu_ws_thread_fn, NULL);
}

void imu_stop(void) {
  struct shared_data shared_data = shared_data_read();
  shared_data.exit_flag = true;
  shared_data_write(shared_data);
  pthread_join(gs_imu_thread, NULL);
}

static struct imu_data imu_from_json(struct mg_str json_str) {
  /* IMU data is reported as vector for each sensor:
   * "accel": [ax, ay, az]
   * "gyro": [gx, gy, gz]
   * "mag": [mx, my, mz] */
  struct imu_data data = {0};
  mg_json_get_num(json_str, "$.accel[0]", &data.accel.x);
  mg_json_get_num(json_str, "$.accel[1]", &data.accel.y);
  mg_json_get_num(json_str, "$.accel[2]", &data.accel.z);
  mg_json_get_num(json_str, "$.gyro[0]", &data.gyro.x);
  mg_json_get_num(json_str, "$.gyro[1]", &data.gyro.y);
  mg_json_get_num(json_str, "$.gyro[2]", &data.gyro.z);
  mg_json_get_num(json_str, "$.mag[0]", &data.mag.x);
  mg_json_get_num(json_str, "$.mag[1]", &data.mag.y);
  mg_json_get_num(json_str, "$.mag[2]", &data.mag.z);
  return data;
}

static void imu_ws_fn(struct mg_connection *conn, int ev, void *ev_data) {
  struct shared_data shared_data = shared_data_read();
  if (ev == MG_EV_OPEN) {
    conn->is_hexdumping = 1;
    shared_data.conn_status = conn_status_init;
  } else if (ev == MG_EV_WS_OPEN) {
    shared_data.conn_status = conn_status_ok;
  } else if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
    shared_data.imu_data = imu_from_json(wm->data);
    shared_data.conn_status = conn_status_ok;

    switch (shared_data.data_status) {
    case data_status_none:
      shared_data.data_status = data_status_pending;
      break;

    case data_status_pending:
      shared_data.data_status = data_status_overrun;

    case data_status_overrun:
    case data_status_error:
      /* Do nothing. */
      break;
    }

    shared_data.data_status = data_status_pending;
  } else if (ev == MG_EV_ERROR || ev == MG_EV_CLOSE) {
    shared_data.conn_status = conn_status_closed;
    shared_data.exit_flag = true;

  } else {
    /* Do nothing. */
  }
  shared_data_write(shared_data);
}

static void *imu_ws_thread_fn(void *args) {
  printf("imu thread being.\n\r");
  struct mg_mgr mgr;
  struct mg_connection *conn;
  struct shared_data shared_data = {0};

  mg_mgr_init(&mgr);
  mg_log_set(MG_LL_INFO);
  conn = mg_ws_connect(&mgr, "ws://192.168.1.100:8000/", imu_ws_fn, NULL, NULL);
  while (conn && !shared_data.exit_flag) {
    mg_mgr_poll(&mgr, 10);
    shared_data = shared_data_read();
  }
  mg_mgr_free(&mgr);
  printf("imu thread end.\n\r");
  return NULL;
}
