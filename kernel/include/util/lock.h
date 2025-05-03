#pragma once

#include <stdbool.h>

#ifndef LOCK_LOCKED
#define LOCK_LOCKED true
#endif

#ifndef LOCK_RELEASED
#define LOCK_RELEASED false
#endif

#ifndef LOCK_TYPE
#define LOCK_TYPE _Atomic(bool)
#endif

#ifndef LOCK_GET
#define LOCK_GET(__lock) __lock
#endif

#ifndef LOCK_UPDATE
#define LOCK_UPDATE(__lock, __val) __lock = __val
#endif

typedef LOCK_TYPE lock_t;

#define lock_init(__lock)                           \
  do {                                              \
    LOCK_UPDATE(__lock, LOCK_RELEASED);             \
  } while (0)

#define lock_trylock(__lock)                        \
    ((LOCK_GET(__lock) == LOCK_RELEASED)            \
      ? LOCK_UPDATE(__lock, LOCK_LOCKED), true      \
      : false)

#define lock_waitlock(__lock)                       \
  do {                                              \
    while (LOCK_GET(__lock) == LOCK_LOCKED) {       \
      ;                                             \
    }                                               \
    LOCK_UPDATE(__lock, LOCK_LOCKED);               \
  } while (0)

#define lock_release(__lock)                        \
  do {                                              \
    LOCK_UPDATE(__lock, LOCK_RELEASED);             \
  } while (0)
