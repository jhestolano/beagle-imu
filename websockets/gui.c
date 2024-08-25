#include "gui.h"
#include "raylib.h"
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void gui_init(void) {
  InitWindow(800, 600, "Quaternions!");
  SetTargetFPS(30);
}

void gui_run(void) {
  struct shared_data shared_data;
  do {
    BeginDrawing();
    shared_data = shared_data_read();
    shared_data_write(shared_data);
    ClearBackground(RAYWHITE);
    EndDrawing();
  } while (!WindowShouldClose() && !IsKeyPressed(KEY_Q));
}

void gui_end(void) { CloseWindow(); }
