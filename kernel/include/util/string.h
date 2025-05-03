#pragma once

#include <stddef.h>
#include <stdarg.h>

void * memcpy(void * dst, const void * src, size_t size);
void * memset(void * buf, int value, size_t size);
volatile void * vmemset(volatile void * buf, int value, size_t size);
void * memmove(void * dst, const void * src, size_t size);
int memcmp(const void * buf1, const void * buf2, size_t size);

size_t strlen(const char * str);
char * strcpy(char * dst, const char * src);
int strcmp(char * s1, const char * s2);
char * strtok(char ** str, char delimiter);

char * itoa(int num, char * str, int base);
char * utoa(unsigned num, char * str, int base);
char * ltoa(long num, char * str, int base);
char * ultoa(unsigned long num, char * str, int base);

int atoi(const char * str, int base);
int atoi_s(const char * str, size_t size, int base);
// int atou(const char * str, int base);
// int atol(const char * str, int base);
// int atoll(const char * str, int base);

char * snprintf(char * buf, size_t size, const char * fmt, ...);
char * vsnprintf(char * buf, size_t size, const char * fmt, va_list args);