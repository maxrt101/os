#pragma once

void kpanic(const char * fmt, ...);

/**
 * Asserts expression, if failed - panics
 */
#define ASSERT(expr)                                                            \
    do {                                                                        \
      if (!(expr)) {                                                            \
        kpanic("Assertion failed at %s:%d\n", __FILE__, __LINE__);              \
      }                                                                         \
    } while (0)

/**
 * Asserts expression, if failed executes user code
 */
#define ASSERT_OR_ELSE(expr, ...)                                               \
    do {                                                                        \
      if (!(expr)) {                                                            \
        __VA_ARGS__;                                                            \
      }                                                                         \
    } while (0)

/**
 * Asserts expression, if failed returns
 */
#define ASSERT_RETURN(expr, ...)                                                \
    do {                                                                        \
      if (!(expr)) {                                                            \
        return __VA_ARGS__;                                                     \
      }                                                                         \
    } while (0)
