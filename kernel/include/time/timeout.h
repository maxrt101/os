#pragma once

#include <time/time.h>
#include <stdbool.h>

typedef struct {
  uint64_t start;
  uint64_t ticks;
} timeout_t;

void timeout_init(timeout_t * timeout, time_t duration);
void timeout_restart(timeout_t * timeout);
void timeout_expire(timeout_t * timeout);
bool timeout_is_expired(timeout_t * timeout);
uint64_t timeout_elapsed(timeout_t * timeout);
uint64_t timeout_remaining(timeout_t * timeout);

