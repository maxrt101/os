#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

#ifdef __CODE16__
typedef signed char         i8;
typedef signed int          i16;
typedef signed long int     i32;
typedef signed long long    i64;

typedef unsigned char       u8;
typedef unsigned int        u16;
typedef unsigned long int   u32;
typedef unsigned long long  u64;
#else
typedef signed char         i8;
typedef signed short        i16;
typedef signed int          i32;
typedef signed long long    i64;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

// typedef unsigned long long  size_t;
#endif

typedef i8                  byte;
typedef i16                 word;
typedef i32                 dword;
typedef i64                 qword;

#endif