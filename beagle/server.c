#include "mongoose.h"
#include "mpu9250.h"
#include <stdio.h>

static const char *gs_listen_on = "ws://0.0.0.0:8000";

struct websock_data {
  struct mg_mgr mgr;
  struct mg_timer imu_timer;
  struct mpu9250 *imu_dev;
};

static void tm_fn(void *data) {
  static int cnt = 0;
  struct websock_data *ws_data = (struct websock_data *)data;
  (void)ws_data;
  puts("triggered!");
  if (cnt++ > 20) {
    mg_timer_free(&ws_data->mgr.timers, &ws_data->imu_timer);
    cnt = 0;
    puts("done!");
  }
}

static void ws_fn(struct mg_connection *conn, int ev, void *ev_data) {
  struct websock_data *ws_data = (struct websock_data *)conn->fn_data;
  (void)ws_data;

  if (ev == MG_EV_OPEN) {
    puts("Connection started!");
  } else if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    if (mg_match(hm->uri, mg_str("/"), NULL)) {
      mg_ws_upgrade(conn, hm, NULL);
      puts("Connection upgraded to websockets");
      mg_timer_init(&ws_data->mgr.timers, &ws_data->imu_timer, 100u,
                    MG_TIMER_REPEAT, tm_fn, ws_data);
    }
  } else if (ev == MG_EV_WS_MSG) {
    // Got websocket frame. Received data is wm->data. Echo it back!
    struct mg_ws_message *msg = (struct mg_ws_message *)ev_data;
    printf("Got message: %.*s\n\r", (int)msg->data.len, msg->data.buf);
    mg_ws_send(conn, msg->data.buf, msg->data.len, WEBSOCKET_OP_TEXT);
  } else if (ev == MG_EV_CLOSE) {
    // mg_timer_free(&ws_data->mgr.timers, &ws_data->imu_timer);
    puts("Connection closed!");
  } else {
    /* Does nothing. */
  }
}

int main(void) {
  struct websock_data ws_data = {
      .imu_dev = mpu9250_init(MPU9250_DEFAULT_CONFIG),
  };

  if (!ws_data.imu_dev) {
    /* No point to continue. Just die. */
    return 1;
  }

  mg_mgr_init(&ws_data.mgr);

  printf("Starting WS listener on %s/websocket\n", gs_listen_on);

  mg_http_listen(&ws_data.mgr, gs_listen_on, ws_fn, (void *)&ws_data);
  for (;;)
    mg_mgr_poll(&ws_data.mgr, 10u);

  mg_mgr_free(&ws_data.mgr);
  mpu9250_deinit(ws_data.imu_dev);

  return 0;
}
