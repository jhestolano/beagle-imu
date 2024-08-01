#include "mongoose.h"
#include "mpu9250.h"
#include <stdio.h>

static const char *s_listen_on = "ws://0.0.0.0:8000";
static const char *s_web_root = ".";

static void ws_fn(struct mg_connection *conn, int ev, void *ev_data) {
  if (ev == MG_EV_OPEN) {
    puts("Connection started!");

  } else if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    if (mg_match(hm->uri, mg_str("/websocket"), NULL)) {
      mg_ws_upgrade(conn, hm, NULL);
    } else if (mg_match(hm->uri, mg_str("/imu"), NULL)) {
      struct mpu9250 *dev = conn->fn_data;
      /* Get sensor data. */
      struct vec3f accel;
      (void)mpu9250_get_accel(dev, &accel);
      mg_http_reply(conn, 200, "", "{\"accel_z\": %f}\n", accel.z);
    } else {
      struct mg_http_serve_opts opts = {.root_dir = s_web_root};
      mg_http_serve_dir(conn, ev_data, &opts);
    }

  } else if (ev == MG_EV_WS_MSG) {
    // Got websocket frame. Received data is wm->data. Echo it back!
    struct mg_ws_message *msg = (struct mg_ws_message *)ev_data;
    printf("Got message: %.*s\n\r", (int)msg->data.len, msg->data.buf);
    mg_ws_send(conn, msg->data.buf, msg->data.len, WEBSOCKET_OP_TEXT);
  } else {
    /* Does nothing. */
  }
}

int main(void) {

  struct mpu9250 *dev = mpu9250_init(MPU9250_DEFAULT_CONFIG);

  if (!dev) {
    /* No point to continue. Just die. */
    return 1;
  }

  struct mg_mgr mgr; // Event manager
  mg_mgr_init(&mgr); // Initialise event manager
  printf("Starting WS listener on %s/websocket\n", s_listen_on);
  mg_http_listen(&mgr, s_listen_on, ws_fn, (void *)dev); // Create HTTP listener
  for (;;)
    mg_mgr_poll(&mgr, 1000); // Infinite event loop
  mg_mgr_free(&mgr);
  mpu9250_deinit(dev);
  return 0;
}
