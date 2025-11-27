#pragma once

#include <stdint.h>

typedef int32_t fixed_t;

#define FIXED_SHIFT 16
#define FIXED_MASK 0xFFFF
#define FIXED_ONE (1LL << FIXED_SHIFT)

#define INT2FIXED(__whole) \
  ((fixed_t) __whole << FIXED_SHIFT)

#define FIXED2INT(__fixed) \
  (__fixed >> FIXED_SHIFT)

#define FIXED(__whole, __frac, __scale) \
  (((fixed_t) __whole << FIXED_SHIFT) + (__frac * FIXED_ONE) / __scale)

#define FIXED_WHOLE(__f) \
  (__f >> FIXED_SHIFT)

#define FIXED_FRACTION(__f) \
  (__f & FIXED_MASK)

#define FIXED_ROUND_UP(__f) \
  ((__f + (FIXED_ONE / 2)) >> FIXED_SHIFT)

#define FIXED_CEIL(__f) \
  ((__f + (FIXED_FRACTION(__f) > 0 ? FIXED_ONE : 0)) >> FIXED_SHIFT)

#define FIXED_MUL(__f1, __f2) \
  (fixed_t)(((int64_t) __f1 * __f2) >> FIXED_SHIFT)

#define FIXED_DIV(__f1, __f2) \
  (fixed_t)(((int64_t) __f1 << FIXED_SHIFT) / __f2)

#define FIXED_MUL_INT(__i, __f) \
  FIXED2INT(FIXED_MUL(INT2FIXED(__i), __f))
