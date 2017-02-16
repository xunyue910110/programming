#define RIPC_IMPLEMENTATION

#include "Thread.h"

pthread_key_t CurrentThread::key;

static CurrentThread initializer;
