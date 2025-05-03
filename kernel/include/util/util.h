#pragma once

#include "util/compiler.h"
#include "util/vargs.h"
#include "util/bits.h"

/**
 * Supress 'unused variable' and 'unused parameter' warnings
 */
#define UTIL_UNUSED(x) (void)(x)

/**
 * Concatenates 2 tokens
 */
#define UTIL_CAT_RAW(a, b) a ## b

/**
 * Concatenates 2 tokens, deferred
 */
#define UTIL_CAT(a, b) UTIL_CAT_RAW(a, b)

/**
 * Determines smallest number from the 2
 */
#define UTIL_MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * Determines biggest number from the 2
 */
#define UTIL_MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * Absolute difference between 2 numbers
 */
#define UTIL_ABS(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))

/**
 * Calculates size of an array
 */
#define UTIL_ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

/**
 * Caps x between min and max
 */
#define UTIL_CAP(x, min, max) \
    ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

/**
 * Maps value of range [min1, max1] to range[min2, max2]
 */
#define UTIL_MAP_RANGE(value, min1, max1, min2, max2)                           \
  (((value) * ((max2) - (min2))) / ((max1) - (min1)))

/**
 * Helper macro for UTIL_IF_1
 */
#define UTIL_IF1_1 __X__,

/**
 * Expanded to if_true if condition is 1, __VA_ARGS__ otherwise
 *
 * Works by concatenating value 'condition' with UTIL_IF1_, if 'condition'
 * is 1 - it will produce UTIL_IF1_1, which will produce '__X__,' and
 * second argument will be if_true, if 'condition' is anything but 1
 * (let's say 0), UTIL_IF1_0 is empty, and doesn't contain a comma, so
 * second argument will be __VA_ARGS__, and it will get called
 */
#define UTIL_IF_1_IMPL(condition, if_true, ...)                                 \
    UTIL_GET_ARG2_OBSTRUCT(UTIL_CAT_RAW(UTIL_IF1_, condition) if_true, __VA_ARGS__)

/**
 * Expanded to if_true if condition is 1, __VA_ARGS__ otherwise
 *
 * @note condition must be literally 1
 */
#define UTIL_IF_1(condition, if_true, ...)                                      \
    UTIL_IF_1_IMPL(condition, if_true, __VA_ARGS__)

/**
 * Helper macro for UTIL_IF_0
 */
#define UTIL_IF0_0 __X__,

/**
 * Expanded to if_true if condition is 0, __VA_ARGS__ otherwise
 *
 * Works by concatenating value 'condition' with UTIL_IF0_, if 'condition'
 * is 0 - it will produce UTIL_IF0_0, which will produce '__X__,' and
 * second argument will be if_true, if 'condition' is anything but 0
 * (let's say 1), UTIL_IF0_1 is empty, and doesn't contain a comma, so
 * second argument will be __VA_ARGS__, and it will get called
 */
#define UTIL_IF_0_IMPL(condition, if_true, ...)                                 \
    UTIL_GET_ARG2_OBSTRUCT(UTIL_CAT_RAW(UTIL_IF0_, condition) if_true, __VA_ARGS__)

/**
 * Expanded to if_true if condition is 0, __VA_ARGS__ otherwise
 *
 * @note condition must be literally 0
 */
#define UTIL_IF_0(condition, if_true, ...)                                      \
    UTIL_IF_0_IMPL(condition, if_true, __VA_ARGS__)

/**
 * Helper macro for UTIL_IF_NON0
 */
#define UTIL_IF_NON0_0 __X__,

/**
 * Expanded to if_true if condition is anything but 0, if_false otherwise
 *
 * Works by concatenating value 'condition' with UTIL_COND_NON0_, if
 * 'condition' is 0 - it will produce UTIL_COND_NON0_0, which will
 * produce '__X__,' and second argument will be if_false, if 'condition'
 * is anything but 0 (let's say 1), UTIL_COND_NON0_1 is empty, and
 * doesn't contain a comma, so second argument will be if_true, and
 * it will get called
 */
#define UTIL_IF_NON0(condition, if_true, if_false)                              \
    UTIL_GET_ARG2_OBSTRUCT(UTIL_CAT_RAW(UTIL_IF_NON0_, condition) if_false, if_true)

/**
 * Helper macro for UTIL_IF_EMPTY
 */
#define UTIL_IF_EMPTY_ __X__,

/**
 * Expanded to if_true if condition is empty, if_false otherwise
 *
 * Works by concatenating value 'condition' with UTIL_IF_EMPTY_, if
 * 'condition' is empty - it will produce UTIL_IF_EMPTY_, which will
 * produce '__X__,' and second argument will be if_true, if 'condition'
 * is anything else (let's say 1), UTIL_IF_EMPTY_1 is empty, and doesn't
 * contain a comma, so second argument will be if_false, and it will get
 * called
 */
#define UTIL_IF_EMPTY(condition, if_true, if_false)                             \
    UTIL_GET_ARG2_OBSTRUCT(UTIL_CAT_RAW(UTIL_IF_EMPTY_, condition) if_true, if_false)
