#include <util/string.h>
#include <util/assert.h>
#include <util/util.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

#define T_toa(__name, __T)                        \
  char * __name(__T num, char * buf, int base) {  \
    size_t i = 0;                                 \
    bool negative = false;                        \
                                                  \
    if (num == 0) {                               \
      buf[i++] = '0';                             \
      buf[i] = '\0';                              \
      return buf+1;                               \
    }                                             \
                                                  \
    if (num < 0 && base == 10) {                  \
      negative = true;                            \
      num = -num;                                 \
    }                                             \
                                                  \
    while (num != 0) {                            \
      int rem = num % base;                       \
      buf[i++] = (rem > 9)                        \
          ? (rem - 10) + 'a'                      \
          : rem + '0';                            \
      num = num / base;                           \
    }                                             \
                                                  \
    if (negative) {                               \
      buf[i++] = '-';                             \
    }                                             \
                                                  \
    buf[i] = '\0';                                \
                                                  \
    str_reverse(buf, i);                          \
                                                  \
    return buf+i;                                 \
  }

#define SNPRINTF_PAD(__size) \
      if (pad_size && __size < pad_size) {                                    \
      for (int i = 0; i < pad_size - __size; ++i) {                           \
        buf[write_index++] = pad_zero ? '0' : ' ';                            \
      }                                                                       \
      pad_size = 0;                                                           \
    }                                                                         \

#define SNPRINTF_NUMBER(__type, __conv, __base)                               \
  {                                                                           \
    char conv_buf[32];                                                        \
    char * end = __conv(va_arg(args, __type), conv_buf, __base);              \
    size_t conv_size = end - conv_buf;                                        \
    SNPRINTF_PAD(conv_size);                                                  \
    memcpy(&buf[write_index], conv_buf, conv_size);                           \
    write_index += conv_size;                                                 \
    ASSERT_RETURN(write_index < size, NULL);                                  \
  }

#define SNPRINTF_NUMBER_CASE(__spec, __type, __conv, __base)                  \
  case __spec: {                                                              \
    SNPRINTF_NUMBER(__type, __conv, __base);                                  \
    break;                                                                    \
  }

#define CHAR_IS_NUM(c) ((c) >= '0' && (c) <= '9')
#define CHAR_IS_NUM_HEX_LOWER(c) ((c) >= 'a' && (c) <= 'f')
#define CHAR_IS_NUM_HEX_UPPER(c) ((c) >= 'A' && (c) <= 'F')
#define CHAR_IS_NUM_HEX(c) (CHAR_IS_NUM_HEX_LOWER(c) || CHAR_IS_NUM_HEX_UPPER(c))

__STATIC_INLINE void str_reverse(char * str, size_t size) {
  size_t start = 0;
  size_t end = size - 1;
  while (start < end) {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    end--;
    start++;
  }
}

void * memcpy(void * dst, const void * src, size_t size) {
  ASSERT_RETURN(dst && src, NULL);

  uint8_t * dst_ptr = dst;
  const uint8_t * src_ptr = src;

  for (size_t i = 0; i < size; ++i) {
    dst_ptr[i] = src_ptr[i];
  }

  return dst;
}

void * memset(void * buf, int value, size_t size) {
  ASSERT_RETURN(buf, NULL);

  uint8_t * buf_ptr = buf;

  for (size_t i = 0; i < size; ++i) {
    buf_ptr[i] = value;
  }

  return buf;
}

volatile void * vmemset(volatile void * buf, int value, size_t size) {
  ASSERT_RETURN(buf, NULL);

  volatile uint8_t * buf_ptr = buf;

  for (size_t i = 0; i < size; ++i) {
    buf_ptr[i] = value;
  }

  return buf;
}

int memcmp(const void * buf1, const void * buf2, size_t size) {
  ASSERT_RETURN(buf1 && buf2, 1); // TODO: Return some error marker

  const uint8_t * p1 = buf1;
  const uint8_t * p2 = buf2;

  for (size_t i = 0; i < size; ++i) {
    if (p1[i] != p2[i]) {
      return p1[i] < p2[i] ? -1 : 1;
    }
  }

  return 0;
}

size_t strlen(const char * str) {
  ASSERT_RETURN(str, 0);

  size_t result = 0;

  while (str[result]) {
    result++;
  }

  return result;
}

char * strcpy(char * dst, const char * src) {
  ASSERT_RETURN(dst && src, NULL); // TODO: Return some error marker

  while (*dst && *src) {
    *dst++ = *src++;
  }

  // TODO: Check what should be returned here
  return dst;
}

int strcmp(char * s1, const char * s2) {
  ASSERT_RETURN(s1 && s2, 1);

  // TODO: Check for behaviour
  while (*s1 && *s2) {
    if (*s1 != *s2) {
      return *s1 < *s2 ? -1 : 1;
    }
    s1++;
    s2++;
  }

  return !(*s1 == '\0' && *s2 == '\0');
}

char * strtok(char ** str, char delimiter) {
  char * start = *str;

  if (**str == '\0') {
    return NULL;
  }

  while (**str != delimiter && **str != '\0') {
    (*str)++;
  }

  **str = '\0';
  (*str)++;

  return start;
}

T_toa(itoa, int);
T_toa(ltoa, long);
T_toa(lltoa, long long);
T_toa(utoa, unsigned);
T_toa(ultoa, unsigned long);
T_toa(ulltoa, unsigned long long);

int atoi_s(const char * str, size_t size, int base) {
  ASSERT_RETURN(str, INT_MAX);

  const char * start = str;

  int sign = 1;
  int result = 0;

  if (*str == '-') {
    sign = -1;
    str++;
  }

  while (CHAR_IS_NUM(*str) || (base == 16 ? CHAR_IS_NUM_HEX(*str) : false)) {
    if (str - start >= size) {
      break;
    }

    result *= base;
    if (CHAR_IS_NUM(*str)) {
      result += *str - '0';
    } else if (CHAR_IS_NUM_HEX_LOWER(*str)) {
      result += *str - 'a';
    } else if (CHAR_IS_NUM_HEX_UPPER(*str)) {
      result += *str - 'A';
    } else {
      return INT_MAX;
    }

    str++;
  }

  return sign * result;
}

int atoi(const char * str, int base) {
  ASSERT_RETURN(str, INT_MAX);

  int sign = 1;
  int result = 0;

  if (*str == '-') {
    sign = -1;
    str++;
  }

  while (CHAR_IS_NUM(*str) || (base == 16 ? CHAR_IS_NUM_HEX(*str) : false)) {
    result *= base;
    if (CHAR_IS_NUM(*str)) {
      result += *str - '0';
    } else if (CHAR_IS_NUM_HEX_LOWER(*str)) {
      result += *str - 'a';
    } else if (CHAR_IS_NUM_HEX_UPPER(*str)) {
      result += *str - 'A';
    } else {
      return INT_MAX;
    }

    str++;
  }

  return sign * result;
}

char * snprintf(char * buf, size_t size, const char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char * res = vsnprintf(buf, size, fmt, args);
  va_end(args);
  return res;
}

char * vsnprintf(char * buf, size_t size, const char * fmt, va_list args) {
  ASSERT_RETURN(buf && fmt, NULL);

  size_t index = 0;
  size_t write_index = 0;

  bool pad_zero = false;
  int pad_size = 0;

  while (fmt[index]) {
    if (fmt[index] == '%') {
      index++;

      size_t pad_start = index;

      while (CHAR_IS_NUM(fmt[index])) {
        index++;
      }

      if (pad_start < index) {
        if (fmt[pad_start] == '0') {
          pad_zero = true;
          pad_start++;
        }

        pad_size = atoi_s(&fmt[pad_start], index - pad_start, 10);
      }

      switch (fmt[index]) {
        case '%':
          buf[write_index++] = '%';
          break;

        case 'c':
          buf[write_index++] = va_arg(args, int);
          break;

        case 's': {
          const char * s = va_arg(args, const char *);
          size_t size = strlen(s);
          SNPRINTF_PAD(size);
          memcpy(&buf[write_index], s, size);
          write_index += size;
          break;
        }

        case 'p': {
          buf[write_index++] = '0';
          buf[write_index++] = 'x';
          SNPRINTF_NUMBER(unsigned long long, ulltoa, 16);
          break;
        }

        SNPRINTF_NUMBER_CASE('d', int, itoa, 10);
        SNPRINTF_NUMBER_CASE('x', int, itoa, 16);

        // TODO: Very ugly, rewrite
        case 'u': {
          if (fmt[index+1] == 'x') {
            index++;
            SNPRINTF_NUMBER(unsigned, utoa, 16);
          } else if (fmt[index+1] == 'l') {
            index++;
            if (fmt[index+1] == 'l') {
              index++;
              if (fmt[index+1] == 'x') {
                index++;
                SNPRINTF_NUMBER(unsigned long long, ulltoa, 16);
              } else if (fmt[index+1] == 'd') {
                index++;
                SNPRINTF_NUMBER(unsigned long long, ulltoa, 10);
              } else {
                SNPRINTF_NUMBER(unsigned long long, ulltoa, 10);
              }
            } else if (fmt[index+1] == 'x') {
              index++;
              SNPRINTF_NUMBER(unsigned long, ulltoa, 16);
            } else {
              SNPRINTF_NUMBER(unsigned long, ultoa, 10);
            }
          } else {
            SNPRINTF_NUMBER(unsigned, utoa, 10);
          }
          break;
        }

        case 'l': {
          if (fmt[index+1] == 'l') {
            index++;
            if (fmt[index+1] == 'x') {
              index++;
              SNPRINTF_NUMBER(long long, lltoa, 16);
            } else if (fmt[index+1] == 'd') {
              index++;
              SNPRINTF_NUMBER(long long, lltoa, 10);
            } else {
              SNPRINTF_NUMBER(long long, lltoa, 10);
            }
          } else if (fmt[index+1] == 'x') {
            index++;
            SNPRINTF_NUMBER(long, ltoa, 16);
          } else if (fmt[index+1] == 'd') {
            index++;
            SNPRINTF_NUMBER(long long, ltoa, 10);
          } else {
            SNPRINTF_NUMBER(long, ltoa, 10);
          }
          break;
        }
      }
    } else {
      buf[write_index++] = fmt[index];
    }

    index++;
    ASSERT_RETURN(write_index < size, NULL);
  }

  buf[write_index] = '\0';

  return buf;
}
