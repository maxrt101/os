#include <string.h>
#include <stdlib.h>
#include <math.h>


#define DEFINE_CONV_FUNCTION(name, type)\
  char* name (type value, char *buffer, int base) {\
    if (base < 2 || base > 32) return buffer;\
    type n = abs(value);\
    int i = 0;\
    while (n) {\
      int r = n % base;\
      if (r >= 10) buffer[i++] = 65 + (r - 10);\
      else buffer[i++] = 48 + r;\
      n = n / base;\
    }\
    if (i == 0) buffer[i++] = '0';\
    if (value < 0 && base == 10) buffer[i++] = '-';\
    buffer[i] = '\0';\
    return reverse_char_buf(buffer, 0, i-1);\
  }

// Helper functions

static char* reverse_char_buf(char* buffer, int i, int j) {
  if (!buffer)
    return 0;
  while (i < j) {
    swap(&buffer[i++], &buffer[j--]);
  }
  return buffer;
}

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

char* strcat(char *dest, const char *src) {
  if (!dest || !src)
    return 0;
  int dest_ptr = 0;
  for (int i = 0; dest[i]; i++) {
    dest_ptr = i;
  }
  for (int i = 0; src[i]; i++) {
    dest_ptr++;
    dest[dest_ptr] = src[i];
  }
  return dest;
}

char* strchr(char *str, int ch) {
  for (size_t i = 0; i < strlen(str); i++) {
    if (str[i] == ch)
      return str+i;
  }
  return NULL;
}

char* strcpy(char *dest, const char *src) {
  if (!dest)
    return 0;
  for (int i = 0; src[i]; i++) {
    dest[i] = src[i];
  }
  return dest;
}

int strsplit(const char *str, char **buffer, char delim) {
  if (!str || !buffer)
    return 0;
  int si = 0, bi = 0, bei = 0;
  while (str[si]) {
    if (str[si] == delim) {
      si++;
      bi++;
      bei = 0;
    } else {
      buffer[bi][bei++] = str[si++];
    }
  }
  return bi;
}

int atoi(const char* str) {
  int res = 0;
  for (int i = 0; str[i]; ++i)
    res = res * 10 + str[i] - '0';
  return res;
}

double atof(const char* str) {
  double res = 0.0;
  return res;
}

long atol(const char* str) {
  return 0;
}

DEFINE_CONV_FUNCTION(itoa, int);
DEFINE_CONV_FUNCTION(utoa, unsigned int);
DEFINE_CONV_FUNCTION(lltoa, long long);
DEFINE_CONV_FUNCTION(ulltoa, unsigned long long);


int memcmp(const void* lhs, const void* rhs, size_t count) {
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
}

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

void* memmove(void *dest, const void *src, size_t count) {
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
}

}