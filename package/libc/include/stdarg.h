#ifndef _STDARG_H_
#define _STDARG_H_

#define STACKITEM int

typedef unsigned char* va_list;

#define VA_SIZE(TYPE) (( sizeof(TYPE) + sizeof(STACKITEM) - 1) & ~(sizeof(STACKITEM) - 1 ))
#define va_start(AP, LASTARG) ( AP = ((va_list) & (LASTARG) + VA_SIZE(LASTARG)) )
#define va_end(AP)
#define va_arg(AP, TYPE) ( AP += VA_SIZE(TYPE), *((TYPE*)(AP - VA_SIZE(TYPE))) )

#endif