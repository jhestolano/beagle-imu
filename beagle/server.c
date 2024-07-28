#include "mongoose.h"
#include <stdio.h>

// static const char *s_listen_on = "ws://192.168.1.23:8000";
static const char *s_listen_on = "ws://0.0.0.0:8000";
static const char *s_web_root = ".";

static void ws_fn(struct mg_connection *conn, int ev, void *ev_data)
{
    if (ev == MG_EV_OPEN) {
        // conn->is_hexdumping = 1;
    }
    else if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        if (mg_match(hm->uri, mg_str("/websocket"), NULL)) {
            // Upgrade to websocket. From now on, a connection is a full-duplex
            // Websocket connection, which will receive MG_EV_WS_MSG events.
            mg_ws_upgrade(conn, hm, NULL);
        }
        else if (mg_match(hm->uri, mg_str("/rest"), NULL)) {
            // Serve REST response
            mg_http_reply(conn, 200, "", "{\"result\": %d}\n", 123);
        }
        else {
            // Serve static files
            struct mg_http_serve_opts opts = {.root_dir = s_web_root};
            mg_http_serve_dir(conn, ev_data, &opts);
        }
    }
    else if (ev == MG_EV_WS_MSG) {
        // Got websocket frame. Received data is wm->data. Echo it back!
        struct mg_ws_message *msg = (struct mg_ws_message *)ev_data;
        printf("Got message: %.*s\n\r", (int)msg->data.len, msg->data.buf);
        mg_ws_send(conn, msg->data.buf, msg->data.len, WEBSOCKET_OP_TEXT);
    }
}

int main(void)
{
    struct mg_mgr mgr; // Event manager
    mg_mgr_init(&mgr); // Initialise event manager
    printf("Starting WS listener on %s/websocket\n", s_listen_on);
    mg_http_listen(&mgr, s_listen_on, ws_fn, NULL); // Create HTTP listener
    for (;;)
        mg_mgr_poll(&mgr, 1000); // Infinite event loop
    mg_mgr_free(&mgr);
    return 0;
}
