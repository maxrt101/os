#pragma once

#include <stdint.h>

typedef enum {
  TIME_MICROSECONDS,
  TIME_MILLISECONDS,
  TIME_SECONDS,
  TIME_MINUTES,
  TIME_HOURS,
  // TODO: days, months, years
} time_type_t;

typedef struct {
  time_type_t type;
  uint64_t    value;

  // Fraction of type-1, used in up conversions
  uint64_t    frac;
} time_t;

time_t time_microseconds(uint64_t value);
time_t time_milliseconds(uint64_t value);
time_t time_seconds(uint64_t value);
time_t time_minutes(uint64_t value);
time_t time_hours(uint64_t value);

time_t time_convert(time_t time, time_type_t type);

uint64_t time_get_ms();
