#ifndef _KFS_TOOLS_H
#define _KFS_TOOLS_H

#include "fs/kfs.h"

#define KFS_ROOT_NODE_NAME "root"
#define KFS_BOOT_NODE_NAME "boot"

#ifdef __cplusplus
extern "C" {
#endif

void _kfs_print_node(kfs_node* node, uint8_t* data, int identation);

void _kfs_debug(const char* fmt, ...);
void _kfs_set_debug(bool debug);
bool _kfs_get_debug();

#ifdef __cplusplus
}
#endif

#endif
