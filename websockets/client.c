#include "mongoose.h"
#include <stdio.h>
static const char *s_url = "ws://localhost:8000/websocket";

void comm_init(void)
{
}

static void ws_fn(struct mg_connection *conn, int ev, void *ev_data)
{
    if (ev == MG_EV_OPEN) {
        conn->is_hexdumping = 1;
    }
    else if (ev == MG_EV_ERROR) {
        MG_ERROR(("%p %s", conn->fd, (char *)ev_data));
    }
    else if (ev == MG_EV_WS_OPEN) {
        mg_ws_send(conn, "hello", 5, WEBSOCKET_OP_TEXT);
    }
    else if (ev == MG_EV_WS_MSG) {
        struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
        printf("GOT ECHO REPLY: [%.*s]\n", (int)wm->data.len, wm->data.buf);
    }

    if (ev == MG_EV_ERROR || ev == MG_EV_CLOSE || ev == MG_EV_WS_MSG) {
        *(bool *)conn->fn_data = true; // Signal that we're done
    }
}

int main(void)
{
    struct mg_mgr mgr;       // Event manager
    bool done = false;       // Event handler flips it to true
    struct mg_connection *c; // Client connection
    mg_mgr_init(&mgr);       // Initialise event manager
    mg_log_set(MG_LL_DEBUG); // Set log level
    c = mg_ws_connect(&mgr, s_url, ws_fn, &done, NULL); // Create client
    while (c && done == false)
        mg_mgr_poll(&mgr, 1000); // Wait for echo
    mg_mgr_free(&mgr);           // Deallocate resources
    return 0;
}
