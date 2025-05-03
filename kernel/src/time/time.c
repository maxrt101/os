#include <time/time.h>
#include <arch.h>

#define TYPE_TUPLE(__t1, __t2) (((uint32_t)(__t1) << 16) | (__t2))

__STATIC_INLINE time_t time_convert_up(time_t time, time_type_t type) {
  if (type - time.type == 1) {
    switch (TYPE_TUPLE(time.type, type)) {
      case TYPE_TUPLE(TIME_MICROSECONDS, TIME_MILLISECONDS):
      case TYPE_TUPLE(TIME_MILLISECONDS, TIME_SECONDS):
        return (time_t){type, time.value / 1000, time.value % 1000};

      case TYPE_TUPLE(TIME_SECONDS, TIME_MINUTES):
      case TYPE_TUPLE(TIME_MINUTES, TIME_HOURS):
        return (time_t){type, time.value / 60, time.value % 60};

      default:
        // TODO: Signal an error
        return time;
    }
  }

  return time_convert_up(time_convert_up(time, time.type + 1), type);
}

__STATIC_INLINE time_t time_convert_down(time_t time, time_type_t type) {
  if (time.type - type == 1) {
    switch (TYPE_TUPLE(time.type, type)) {
      case TYPE_TUPLE(TIME_MILLISECONDS, TIME_MICROSECONDS):
      case TYPE_TUPLE(TIME_SECONDS, TIME_MILLISECONDS):
        return (time_t){type, time.value * 1000 + time.frac / 1000, time.frac % 1000};

      case TYPE_TUPLE(TIME_MINUTES, TIME_SECONDS):
      case TYPE_TUPLE(TIME_HOURS, TIME_MINUTES):
        return (time_t){type, time.value * 60  + time.frac / 60, time.frac % 60};

      default:
        // TODO: Signal an error
          return time;
    }
  }

  return time_convert_down(time_convert_down(time, time.type - 1), type);
}

time_t time_microseconds(uint64_t value) {
  return (time_t) {TIME_MICROSECONDS, value};
}

time_t time_milliseconds(uint64_t value) {
  return (time_t) {TIME_MILLISECONDS, value};
}

time_t time_seconds(uint64_t value) {
  return (time_t) {TIME_SECONDS, value};
}

time_t time_minutes(uint64_t value) {
  return (time_t) {TIME_MINUTES, value};
}

time_t time_hours(uint64_t value) {
  return (time_t) {TIME_HOURS, value};
}

time_t time_convert(time_t time, time_type_t type) {
  if (time.type == type) {
    return time;
  }

  return (time.type < type) ? time_convert_up(time, type) : time_convert_down(time, type);
}

uint64_t time_get_ms() {
  return arch_get_tick() * (1000 / arch_get_tick_duration_hz());
}
