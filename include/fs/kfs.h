#ifndef _FS_KFS_H_
#define _FS_KFS_H_

#include <stdint.h>

/*
0   -> 512 => boot sector
512 -> 8K  => boot data (basically bootloader)
8K  -> 9K  => disk metadata
9K  -> 10K => root node
10K -> END => free to use
*/

/*
What if, bootloader is allways at kfs_node 1, at /kboot
So, root node's first sub-node is kboot node
Then we can get block offset for /kboot in assembly, just by calculating metadata offsets
*/

#define KFS_MAX_SUB_NODES       230
#define KFS_NAME_SIZE           16
#define KFS_BOOT_SECTOR_SIZE    512
#define KFS_METADATA_OFFSET     512

#define KFS_BLOCK_SIZE          1024
// #define KFS_DISK_SIZE           1310720
// #define KFS_DISK_SIZE           65536
#define KFS_BITMAP_SIZE         160
#define KFS_DISK_METADATA_SIZE  28+(KFS_BITMAP_SIZE)
#define KFS_NODE_SIZE           24+(KFS_MAX_SUBNODES*4)

#define KFS_MAGIC               0x1A2B3C4D
#define KFS_VERSION             0x1

#define KFS_NODE_FLAG_DIRECTORY (1 << 0)
#define KFS_NODE_FLAG_HIDDEN    (1 << 1)

struct kfs_node {
  uint32_t id;                            // ID
  uint16_t flags;                         // Node Flags
  uint16_t sub_nodes_count;               // Count of sub-nodes
  uint32_t size;                          // Node Data Size
  char     name[KFS_NAME_SIZE];           // Node Name
  uint32_t sub_nodes[KFS_MAX_SUB_NODES];  // If node is a directory - these are pointers to it's contents,
                                          // If node is a file - these are pointers(offsets) to data blocks
};

struct kfs_disk {
  uint8_t* data;                          // Raw Disk Data
  uint8_t* boot;                          // Boot sector offset in data (boot==data)
  uint32_t magic;                         // Magic Number
  char     name[KFS_NAME_SIZE];           // Disk Name
  uint32_t version;                       // KFS version
  uint32_t last_id;                       // ID of last known kfs_node
  uint32_t block_map[KFS_BITMAP_SIZE/4];  // Bitmap of blocks - 1 bit - 1K block
  kfs_node root;                          // Root Node (/)
};

/* Prototypes for KFS functions */

int32_t kfs_parse_data(kfs_disk* disk, uint8_t* data);
int32_t kfs_parse_node(kfs_node* node, uint8_t* data);
int32_t kfs_free(kfs_disk* disk);

int32_t kfs_write_boot(kfs_disk* disk, const uint8_t* boot);
int32_t kfs_write_file(kfs_disk* disk, const char* filename, const uint8_t* data, int size);
// int32_t kfs_write_node(kfs_disk* disk, kfs_node* node, int32_t id);
int32_t kfs_add_node(kfs_disk* disk, kfs_node* node);
int32_t kfs_add_subnode(kfs_disk* disk, kfs_node* parent, kfs_node* node);
int32_t kfs_mkdir(kfs_disk* disk, const char* dirnmae);
int32_t kfs_rm(kfs_disk*, const char* filename);

int32_t kfs_read(kfs_disk* disk, const char* filename, uint8_t* buffer);
int32_t kfs_find(kfs_disk* disk, const char* filename, kfs_node* node);

int32_t kfs_open_file(kfs_disk* disk, const char* filename);
int32_t kfs_save_file(kfs_disk* disk, const char* filename);

int32_t kfs_create_from_dir(kfs_disk* disk, const char* datadir);

#endif
