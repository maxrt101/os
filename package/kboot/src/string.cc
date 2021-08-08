// #include "code16gcc.h"
#include <sys/asm.h>
#include <string.h>

ASM_SECTION(".latetext");

extern "C" {

size_t strlen(const char* str) {
  if (!str)
    return 0;
  size_t length = 0;
  while (str[length])
    length++;
  return length;
}

int strcmp(const char* str1, const char* str2) {
  if (!str1 || !str2)
    return 0;
  for (int i = 0; ; i++) {
    if (str1[i] != str2[i]) {
      return str1[i] < str2[i] ? -1 : 1;
    }
    if (!str1[i]) {
      return 0;
    }
  }
}

int strncmp(const char* s1, const char* s2, int n) {
  while (n && *s1 && (*s1 == *s2)) {
    ++s1;
    ++s2;
    --n;
  }
  if (n == 0) {
      return 0;
  } else {
      return *(unsigned char *)s1 - *(unsigned char *)s2;
  }
}

/*int memcmp(const void* lhs, const void* rhs, size_t count) {
  if (!lhs || !rhs)
    return 0;
  unsigned char* p1 = (unsigned char*)lhs;
  unsigned char* p2 = (unsigned char*)rhs;
  int similatity = 0;
  for (size_t i = 0; i < count; i++) {
    if (p1[i] == p2[i]) {
      similatity++;
    } else {
      return similatity;
    }
  }
  return 0;
}*/

void* memset(void *dest, int c, size_t count) {
  unsigned char* p = (unsigned char*)dest;
  while(count--) {
    *p++ = (unsigned char)c;
  }
  return dest;
}

void* memcpy(void *dest, const void *src, size_t count) {
  if (!dest || !src) {
    return 0;
  }
  unsigned char* d = (unsigned char*)dest;
  unsigned char* s = (unsigned char*)src;
  for (size_t i = 0; i < count; i++) {
    d[i] = s[i];
  }
  return dest;
}

/*void* memmove(void *dest, const void *src, size_t count) {
  if (!dest || !src)
    return 0;
  unsigned char* d = (unsigned char*)dest;
  unsigned char* s = (unsigned char*)src;
  if (d < s) {
    for (size_t i = 0; i < count; i++) {
      d[i] = s[i];
    }
  } else {
    for (size_t i = count; i != 0; i--) {
      d[i-1] = s[i-1];
    }
  }
  return dest;
}*/


}