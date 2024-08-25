#include "gui.h"
#include "imu.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
  imu_start();
  gui_init();
  gui_run();
  gui_end();
  imu_stop();
  return 0;
}
