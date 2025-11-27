#pragma once

#include <stdint.h>
#include <util/util.h>
#include <util/vargs.h>

// #define FIXED(__w, __d) (fixed_t){__w, __d}
//
// typedef struct {
//   uint32_t whole;
//   uint32_t decimal;
// } fixed_t;

// fixed_t fixed_new(uint32_t whole, uint32_t decimal);
// uint32_t fixed_multiply_int(uint32_t i, fixed_t f);
// uint32_t fixed_divide_int(uint32_t i, fixed_t f);

typedef int32_t fixed_t;

// fixed_t create_fixed(int whole, int frac, int scale) {
//   // 1. Convert the whole part
//   fixed_t result = int_to_fixed(whole);
//
//   // 2. Calculate the fixed-point fractional delta
//   // Math: (frac / scale) * 65536
//   // Implementation: (frac * 65536) / scale
//   // We cast to int64_t to prevent overflow during the multiply
//   fixed_t frac_fixed = (fixed_t)(((int64_t)frac * FIX_ONE) / scale);
//
//   // 3. Combine
//   // Handle signs: If whole is negative (e.g., -5.25), we subtract the fraction
//   if (whole >= 0) {
//     result += frac_fixed;
//   } else {
//     result -= frac_fixed;
//   }
//
//   return result;
// }

#define FIXED_SHIFT 16
#define FIXED_MASK 0xFFFF
#define FIXED_ONE (1LL << FIXED_SHIFT)

#define INT2FIXED(__whole) \
  ((fixed_t) __whole << FIXED_SHIFT)

#define FIXED2INT(__fixed) \
  (__fixed >> FIXED_SHIFT)

#define FIXED(__whole, __frac, __scale) \
  (((fixed_t) __whole << FIXED_SHIFT) + (__frac * FIXED_ONE) / __scale)

#define FIXED_WHOLE(__f) (__f >> FIXED_SHIFT)
#define FIXED_FRACTION(__f) (__f & FIXED_MASK)

#define FIXED_ROUND_UP(__f) \
  ((__f + (FIXED_ONE / 2)) >> FIXED_SHIFT)

#define FIXED_CEIL(__f) \
  ((__f + (FIXED_FRACTION(__f) > 0 ? FIXED_ONE : 0)) >> FIXED_SHIFT)

#define FIXED_MUL(__f1, __f2) \
  (fixed_t)(((int64_t) __f1 * __f2) >> FIXED_SHIFT)

#define FIXED_DIV(__f1, __f2) \
  (fixed_t)(((int64_t) __f1 << FIXED_SHIFT) / __f2)

#define FIXED_MUL_INT(__i, __f) \
  FIXED2INT((fixed_t)((((int64_t) __i << FIXED_SHIFT) * __f) >> FIXED_SHIFT))
