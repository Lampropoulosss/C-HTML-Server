#pragma once

#include <pthread.h>
#include "taskQueue.h"
#include "handleClient.h"

void init_threads();
void *worker_thread();