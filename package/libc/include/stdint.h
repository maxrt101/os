#ifndef _STDINT_H_
#define _STDINT_H_

#ifdef __CODE16__
typedef unsigned long long  uint64_t;
typedef unsigned long int   uint32_t;
typedef unsigned short      uint16_t;
typedef unsigned char       uint8_t;

typedef long long           int64_t;
typedef long int            int32_t;
typedef short               int16_t;
typedef char                int8_t;
#else
typedef unsigned long long  uint64_t;
typedef unsigned int        uint32_t;
typedef unsigned short      uint16_t;
typedef unsigned char       uint8_t;

typedef long long           int64_t;
typedef int                 int32_t;
typedef short               int16_t;
typedef char                int8_t;
#endif

#endif