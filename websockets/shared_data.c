#include "shared_data.h"
#include <pthread.h>

static pthread_mutex_t mutex_shared_data = PTHREAD_MUTEX_INITIALIZER;
static struct shared_data gs_shared_data = {0};

struct shared_data shared_data_read(void) {
  struct shared_data shared_data;
  pthread_mutex_lock(&mutex_shared_data);
  shared_data = gs_shared_data;
  pthread_mutex_unlock(&mutex_shared_data);
  return shared_data;
}

void shared_data_write(struct shared_data data) {
  pthread_mutex_lock(&mutex_shared_data);
  gs_shared_data = data;
  pthread_mutex_unlock(&mutex_shared_data);
}
