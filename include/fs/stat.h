#ifndef _FS_STAT_H
#define _FS_STAT_H

#include <stdint.h>

struct stat_t {
  uint32_t dev;
  uint32_t node;
  uint32_t size;
  uint32_t blksize;
};

int stat(const char* path, stat_t* statbuf);

#endif