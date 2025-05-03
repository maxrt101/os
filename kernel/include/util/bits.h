#pragma once

/**
 * Sets bit
 */
#define UTIL_BIT_SET(val, bit) (val | ((uint64_t) 1 << bit))

/**
 * Clears bit
 */
#define UTIL_BIT_CLEAR(val, bit) (val & ~((uint64_t) 1 << bit))

/**
 * Toggles bit
 */
#define UTIL_BIT_TOGGLE(val, bit) (val ^ ((uint64_t) 1 << bit))

/**
 * Gets bit
 */
#define UTIL_BIT_GET(val, bit) ((val & ((uint64_t) 1 << bit)) ? 1 : 0)
