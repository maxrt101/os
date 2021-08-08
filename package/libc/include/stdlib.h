#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stddef.h>

#ifdef __cplusplus

template <typename T> inline void min(T a, T b) {
    return a < b ? a : b;
}

template <typename T> inline void max(T a, T b) {
    return a > b ? a : b;
}

template <typename T> inline void swap(T* a, T* b) {
    T tmp = *a;
    *a = *b;
    *b = tmp;
}

#endif

#endif