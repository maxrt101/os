#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>

#include "kfs_tools.h"

#define KFS_CLI_VERSION     0x0001
#define KFS_CLI_VERSION_STR "0.1"

/*
kfscli create disk.kfs
//kfscli create disk.kfs rootfs/

kfscli setname "KFS TEST IMAGE" disk.kfs

kfscli mkdir /sys disk.kfs
kfcli rm /test/file.txt disk.kfs
kfscli boot boot.img disk.kfs 
kfscli write kernel.bin /sys/kernel disk.kfs

kfscli meta disk.kfs
kfscli read disk.kfs
kfscli read /test.txt disk.kfs
kfscli read /boot/boot.cfg disk.kfs

kfscli 
# open disk.kfs
disk.kfs> read
/
/boot
/boot/boot.cfg
/sys
/sys/kernel
/test.txt
# ...
*/

template <typename T> void print_binary(T n) {
  int numbits = sizeof(T) * 8;
  while(--numbits >= 0)
    printf("%c", (n & ((T)1<<numbits)) ? '1' : '0');
}

static void usage(const char* argv0) {
  printf("kfscli v%s for kfs v%d\n", KFS_CLI_VERSION_STR, KFS_VERSION);
  printf("Usage: %s [OPTIONS] ACTION [ARGS...]\n", argv0);
  printf("Options:\n");
  printf("  -d  -  enabled debug prints\n");
  printf("Actions:\n");
  printf("  help\n");
  printf("  create IMAGE [DIRECTORY]\n");
  printf("  setname NAME IMAGE\n");
  printf("  mkdir DIRECTORY IMAGE\n");
  printf("  boot BOOT_SECTOR_IMAGE IMAGE\n");
  printf("  write FILENAME KFS_FILENAME IMAGE\n");
  printf("  meta IMAGE\n");
  printf("  read [FILENAME] IMAGE\n");
}

static void _cli_create_img(const char* kfsfile, const char* image_name) {
  _kfs_debug("_cli_create_img('%s', '%s')\n", kfsfile, image_name);
  kfs_disk disk;
  disk.magic = KFS_MAGIC;
  disk.version = KFS_VERSION;
  disk.last_id = 0;
  disk.data = new uint8_t[65536];
  disk.boot = disk.data;
  strncpy(disk.name, image_name, 16);
  memset(disk.block_map, 0, KFS_BITMAP_SIZE);
  disk.block_map[0] = 0xE0000000; // first 3 blocks

  disk.root.id = 0;
  disk.root.flags = KFS_NODE_FLAG_DIRECTORY;
  disk.root.size = 0;
  memset(disk.root.name, 0, KFS_NAME_SIZE);
  strcpy(disk.root.name, KFS_ROOT_NODE_NAME);
  disk.root.sub_nodes_count = 0;

  kfs_node boot;
  boot.id = 0;
  boot.flags = KFS_NODE_FLAG_HIDDEN;
  boot.size = 0;
  memset(boot.name, 0, KFS_NAME_SIZE);
  strcpy(boot.name, KFS_BOOT_NODE_NAME);
  boot.sub_nodes_count = 0;

  kfs_add_subnode(&disk, &disk.root, &boot);

  kfs_save_file(&disk, kfsfile);

  delete [] disk.data;
}

static int _cli_read_metadata(const char* kfsfile) {
  _kfs_debug("_cli_read_metadata('%s')\n", kfsfile);
  kfs_disk disk;
  if (kfs_open_file(&disk, kfsfile)) {
    printf("Error reading image\n");
    return -1;
  }

  printf("FILE:    %s\n", kfsfile);
  printf("MAGIC:   %x\n", disk.magic);
  printf("NAME:    %.*s\n", 16, disk.name);
  printf("VERSION: %d\n", disk.version);
  printf("LAST_ID: %d\n", disk.last_id);
  // for (int i = 0; i < KFS_BITMAP_SIZE/4; i++) {
    // print_binary(disk.block_map[i]);
    // if (i && i % 5 == 0)
    //   printf("\n");
    // for (int j = 0; j < 32; j++) {
    //   int bit = 1 << (31-j);
    //   //printf("find_first_free i:%d j:%d map[i]:%x and:%d !and:%d\n",
    //   //  i, j, disk->block_map[i], !!(disk->block_map[i] & bit), !(disk->block_map[i] & bit));
    //   if (!(disk->block_map[i] & bit)) {
    //     return i*4*8+j;
    //   }
    // }
  // }
  // printf("\n");
  // printf("BLOCKS: %d free: %d\n", );

  printf("NODES:\n");
  _kfs_print_node(&disk.root, disk.data, 2);

  kfs_free(&disk);
  return 0;
}

static int _cli_setname(const char* kfsfile, const char* name) {
  _kfs_debug("_cli_setname('%s', '%s')\n", kfsfile, name);
  kfs_disk disk;
  if (kfs_open_file(&disk, kfsfile)) {
    printf("Error reading image\n");
    return -1;
  }

  strncpy(disk.name, name, 16);

  kfs_save_file(&disk, kfsfile);
  kfs_free(&disk);
  return 0;
}

static int _cli_mk_dir(const char* kfsfile, const char* dirname) {
  _kfs_debug("_cli_mk_dir('%s', '%s')\n", kfsfile, dirname);
  kfs_disk disk;
  if (kfs_open_file(&disk, kfsfile)) {
    printf("Error reading image\n");
    return -1;
  }

  if (kfs_mkdir(&disk, dirname) == -1) {
    printf("mkdir failed\n");
    return -1;
  }

  kfs_save_file(&disk, kfsfile);
  kfs_free(&disk);
  return 0;
}

static int _cli_read_tree_node(kfs_disk* disk, kfs_node* node, std::string parent, int level) {
  _kfs_debug("_cli_read_tree_node(...)\n");
  if (node->flags & KFS_NODE_FLAG_HIDDEN) {
    return 1;
  }
  if (parent.back() != '/') {
    parent += "/";
  }
  if (level) {
    parent += node->name;
  }
  printf("%s\n", parent.c_str());
  for (int i = 0; i < node->sub_nodes_count; i++) {
    if (node->flags & KFS_NODE_FLAG_DIRECTORY) {
      kfs_node sub_node;
      kfs_parse_node(&sub_node, disk->data+node->sub_nodes[i]);
      _cli_read_tree_node(disk, &sub_node, parent, level+1);
    }
  }
  return 0;
}

static int _cli_read_tree(const char* kfsfile) {
  _kfs_debug("_cli_read_tree('%s')\n", kfsfile);
  kfs_disk disk;
  if (kfs_open_file(&disk, kfsfile)) {
    printf("Error reading image\n");
    return -1;
  }

  _cli_read_tree_node(&disk, &disk.root, "", 0);
  kfs_free(&disk);
  return 0;
}

static int _cli_read_file(const char* kfsfile, const char* filename) {
  _kfs_debug("_cli_read_file('%s', '%s')\n", kfsfile, filename);
  kfs_disk disk;
  if (kfs_open_file(&disk, kfsfile)) {
    printf("Error reading image\n");
    return -1;
  }

  kfs_node file;
  if (kfs_find(&disk, filename, &file) == -1) {
    printf("No such file or directory\n");
    return -1;
  }

  uint8_t* buffer = new uint8_t[file.size];
  kfs_read(&disk, filename, buffer);

  for (int i = 0; i < file.size; i++) {
    putchar(buffer[i]);
  }

  kfs_free(&disk);
  return 0;
}

static int _cli_load_boot(const char* kfsfile, const char* bootimg) {
  _kfs_debug("_cli_load_boot('%s', '%s')\n", kfsfile, bootimg);
  kfs_disk disk;
  if (kfs_open_file(&disk, kfsfile)) {
    printf("Error reading image\n");
    return -1;
  }

  std::ifstream file(bootimg);
  if (!file.is_open()) {
    printf("No such file or directory: %s\n", bootimg);
    return -1;
  }
  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  uint8_t* buffer = new uint8_t[size];
  file.seekg(0, std::ios::beg); 
  file.read((char*)buffer, size);
  file.close();

  if (size != 512) {
    printf("Bootimg size != 512\n");
    delete [] buffer;
    return -1;
  }

  kfs_write_boot(&disk, buffer);
  kfs_save_file(&disk, kfsfile);
  kfs_free(&disk);
  delete [] buffer;
  return 0;
}

static int _cli_load_atnode(const char* kfsfile, const char* filename, int node) {
  _kfs_debug("_cli_load_atnode('%s', '%s', %d)\n", kfsfile, filename, node);
  kfs_disk disk;
  if (kfs_open_file(&disk, kfsfile)) {
    printf("Error reading image\n");
    return -1;
  }

  std::ifstream file(filename);
  if (!file.is_open()) {
    printf("No such file or directory: %s\n", filename);
    return -1;
  }
  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  uint8_t* buffer = new uint8_t[size];
  file.seekg(0, std::ios::beg); 
  file.read((char*)buffer, size);
  file.close();

  kfs_save_file(&disk, kfsfile);
  kfs_free(&disk);
  delete [] buffer;
  return 0;
}

static int _cli_write_file(const char* kfsfile, const char* filename, const char* kfsfilename) {
  _kfs_debug("_cli_write_file('%s', '%s', '%s')\n", kfsfile, filename, kfsfilename);
  kfs_disk disk;
  if (kfs_open_file(&disk, kfsfile)) {
    printf("Error reading image\n");
    return -1;
  }

  std::ifstream file(filename);
  if (!file.is_open()) {
    printf("No such file or directory: %s\n", filename);
    return -1;
  }
  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  uint8_t* buffer = new uint8_t[size];
  file.seekg(0, std::ios::beg); 
  file.read((char*)buffer, size);
  file.close();

  kfs_write_file(&disk, kfsfilename, buffer, size);
  kfs_save_file(&disk, kfsfile);
  kfs_free(&disk);
  delete [] buffer;
  return 0;
}

int main(int argc, char ** argv) {
  int index = 1;
  if (argc > 1) {
    while (1) {
      if (!strcmp(argv[index], "help")) {
        usage(argv[0]);
        return 1;
      } else if (!strcmp(argv[index], "create")) {
        if (argc-index == 2) {
          _cli_create_img(argv[index+1], "IMAGE");
          return 0;
        } else if (argc-index == 3) {
          _cli_create_img(argv[index+1], "IMAGE");
          /** TODO: from dir */
          _kfs_debug("NOTE: create image from directory is not implemented\n");
          return 0;
        } else {
          break;
        }
      } else if (!strcmp(argv[index], "setname")) {
        if (argc-index == 3) {
          return _cli_setname(argv[index+2], argv[index+1]);
        } else {
          break;
        }
      } else if (!strcmp(argv[index], "mkdir")) {
        if (argc == 4) {
          return _cli_mk_dir(argv[index+2], argv[index+1]);
        } else {
          break;
        }
      } else if (!strcmp(argv[index], "write")) {
        if (argc-index == 4) {
          return _cli_write_file(argv[index+3], argv[index+1], argv[index+2]);
        } else {
          break;
        }
      } else if (!strcmp(argv[index], "boot")) {
        if (argc-index == 3) {
          return _cli_load_boot(argv[index+2], argv[index+1]);
        } else {
          break;
        }
      } else if (!strcmp(argv[index], "meta")) {
        if (argc-index == 2) {
          return _cli_read_metadata(argv[index+1]);
        } else {
          break;
        }
      } else if (!strcmp(argv[index], "read")) {
        if (argc-index == 2) {
          return _cli_read_tree(argv[index+1]);
        } else if (argc-index == 3) {
          return _cli_read_file(argv[index+2], argv[index+1]);
        } else {
          break;
        }
      } else if (!strcmp(argv[index], "-d")) {
        index++;
        _kfs_set_debug(true);
      } else {
        break;
      }
    }
  }

  usage(argv[0]);
  return 1;
}
