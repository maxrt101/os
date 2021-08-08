#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdarg.h>

void putc(char c);
void puts(const char* s);

int printf(const char* fmt, ...);
int vprintf(const char* fmt, va_list args);

#endif