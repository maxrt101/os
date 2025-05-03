#include <time/timeout.h>
#include <time/time.h>
#include <util/assert.h>
// #include <arch.h>

void timeout_init(timeout_t * timeout, time_t duration) {
  ASSERT_RETURN(timeout);

  timeout->start = time_get_ms() * 1000;
  timeout->ticks = time_convert(duration, TIME_MICROSECONDS).value;
}

void timeout_restart(timeout_t * timeout) {
  ASSERT_RETURN(timeout);

  timeout->start = time_get_ms() * 1000;
}

void timeout_expire(timeout_t * timeout) {
  ASSERT_RETURN(timeout);

  timeout->start = 0;
}

bool timeout_is_expired(timeout_t * timeout) {
  ASSERT_RETURN(timeout, true);

  return (time_get_ms() * 1000 - timeout->start) >= timeout->ticks;
}

uint64_t timeout_elapsed(timeout_t * timeout) {
  ASSERT_RETURN(timeout, 0);

  return time_get_ms() * 1000 - timeout->start;
}

uint64_t timeout_remaining(timeout_t * timeout) {
  ASSERT_RETURN(timeout, 0);

  return timeout->ticks - (time_get_ms() * 1000 - timeout->start);
}
