#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdarg>
#include <vector>
#include <cstdio>
#include <cmath>

#include "kfs_tools.h"

#define CHAR_ZERO_OR_ONE(x) ((x) ? '1' : '0')

static bool _kfs_debug_enabled = false;

int32_t  _kfs_get_subnode(kfs_node* start_node, uint8_t* data, const char* filename, kfs_node* node);
uint32_t _kfs_find_subnode_offset(kfs_node* start_node, uint8_t* data, uint32_t node_id);
uint32_t _kfs_find_node_offset(kfs_disk* disk, uint32_t node_id);

void _kfs_debug(const char* fmt, ...) {
  if (_kfs_debug_enabled) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }
}

void _kfs_set_debug(bool debug) {
  _kfs_debug_enabled = debug;
}

bool _kfs_get_debug() {
  return _kfs_debug_enabled;
}

void _kfs_print_node(kfs_node* node, uint8_t* data, int32_t identation) {
  static const char* spaces = "                                                                ";
  printf("%.*sID:        %d\n",   identation, spaces, node->id);
  printf("%.*sFLAGS:     directory:%c hidden:%c\n",   identation, spaces,
    CHAR_ZERO_OR_ONE(node->flags & KFS_NODE_FLAG_DIRECTORY),
    CHAR_ZERO_OR_ONE(node->flags & KFS_NODE_FLAG_HIDDEN));
  printf("%.*sSIZE:      %d\n",   identation, spaces, node->size);
  printf("%.*sNAME:      %.*s\n", identation, spaces, 16, node->name);
  printf("%.*sSUB_NODES: %d\n",   identation, spaces, node->sub_nodes_count);
  for (int32_t i = 0; i < node->sub_nodes_count; i++) {
    if (node->flags & KFS_NODE_FLAG_DIRECTORY) {
      kfs_node sub_node;
      kfs_parse_node(&sub_node, data+node->sub_nodes[i]);
      printf("%.*sOFFSET: %d\n", identation+2, spaces, node->sub_nodes[i]);
      _kfs_print_node(&sub_node, data, identation+2);
    } else {
      printf("%.*sDATA_BLOCK: %d\n", identation+2, spaces, node->sub_nodes[i]);
    }
  }
}

int32_t _kfs_get_node(kfs_disk* disk, const char* filename, kfs_node* node) {
  if (!strcmp(filename, "root")) {
    kfs_parse_node(node, disk->data+_kfs_find_node_offset(disk, 0));
    return 0;
  }

  return _kfs_get_subnode(&disk->root, disk->data, filename, node);
}

int32_t _kfs_get_subnode(kfs_node* start_node, uint8_t* data, const char* filename, kfs_node* node) {
  kfs_node current_node;

  for (int32_t i = 0; i < start_node->sub_nodes_count; i++) {
    kfs_parse_node(&current_node, data+start_node->sub_nodes[i]);

    if (!strncmp(current_node.name, filename, 16)) {
      kfs_parse_node(node, data+start_node->sub_nodes[i]);
      return 0;
    } else {
      if (current_node.flags & KFS_NODE_FLAG_DIRECTORY) {
        int32_t rc = _kfs_get_subnode(&current_node, data, filename, node);
        if (rc == 0) {
          return 0;
        }
      }
    }
  }

  return -1;
}

uint32_t _kfs_find_node_offset(kfs_disk* disk, uint32_t node_id) {
  if (disk->root.id == node_id) {
    return 1024; /** FIXME: 'magic' number */
  }

  return _kfs_find_subnode_offset(&disk->root, disk->data, node_id);
}

uint32_t _kfs_find_subnode_offset(kfs_node* start_node, uint8_t* data, uint32_t node_id) {
  kfs_node current_node;

  if (!(start_node->flags & KFS_NODE_FLAG_DIRECTORY)) {
    return 0;
  }

  for (int32_t i = 0; i < start_node->sub_nodes_count; i++) {
    kfs_parse_node(&current_node, data+start_node->sub_nodes[i]);

    if (current_node.id == node_id) {
      return start_node->sub_nodes[i];
    } else {
      if (current_node.flags & KFS_NODE_FLAG_DIRECTORY) {
        int32_t offset = _kfs_find_subnode_offset(&current_node, data, node_id);
        if (offset != 0) {
          return offset;
        }
      }
    }
  }

  return 0;
}

int32_t _kfs_save_node(kfs_disk* disk, kfs_node* node, int32_t offset) {
  _kfs_debug("_kfs_save_node: %.*s %d(%x)\n", 16, node->name, offset, offset);
  
  if (!offset) {
    return -1;
  }

  uint8_t* ptr = disk->data+offset; // new node place

  memcpy(ptr, &node->id, 4); ptr+=4;
  memcpy(ptr, &node->flags, 2); ptr+=2;
  memcpy(ptr, &node->sub_nodes_count, 2); ptr+=2;
  memcpy(ptr, &node->size, 4); ptr+=4;
  memcpy(ptr, &node->name, KFS_NAME_SIZE); ptr+=KFS_NAME_SIZE;
  for (int32_t i = 0; i < KFS_MAX_SUB_NODES; i++) {
    if (i > node->sub_nodes_count) {
      for (int32_t i = 0; i < 4; i++) *ptr++ = 0;
    } else {
       memcpy(ptr, &node->sub_nodes[i], 4); ptr+=4;
    }
  }

  if (node->id == 0) {
    kfs_parse_node(&disk->root, disk->data+_kfs_find_node_offset(disk, 0));
  }

  return 0;
}

uint32_t _kfs_find_first_free_block(kfs_disk* disk) {
  for (int32_t i = 0; i < KFS_BITMAP_SIZE/4; i++) {
    for (int32_t j = 0; j < 32; j++) {
      int32_t bit = 1 << (31-j);
      if (!(disk->block_map[i] & bit)) {
        return i*4*8+j;
      }
    }
  }
  return 0;
}

void _kfs_claim_block(kfs_disk* disk, int32_t block) {
  disk->block_map[block/32] |= (1 << (31 - (block % 32)));
}

void _kfs_free_block(kfs_disk* disk, int32_t block) {
  disk->block_map[block/32] &= ~(1 << (31 - (block % 32)));
}

void _kfs_free_node_blocks(kfs_disk* disk, kfs_node* node) {
  for (int32_t i = 0; i < node->sub_nodes_count; i++) {
    _kfs_free_block(disk, node->sub_nodes[i]/KFS_BLOCK_SIZE);
  }
  node->sub_nodes_count = 0;
}

int32_t kfs_parse_data(kfs_disk* disk, uint8_t* data) {
  disk->data = data;
  disk->boot = data;

  data += KFS_METADATA_OFFSET;

  memcpy(&disk->magic, data, 4); data+=4;

  if (disk->magic != KFS_MAGIC) {
    return -1;
  }

  memcpy(&disk->name, data, KFS_NAME_SIZE); data+=KFS_NAME_SIZE;
  memcpy(&disk->version, data, 4); data+=4;
  memcpy(&disk->last_id, data, 4); data+=4;

  for (int32_t i = 0; i < KFS_BITMAP_SIZE/4; i++) {
    memcpy(&disk->block_map[i], data, 4); data+=4;
  }

  while (data - disk->data != 1024) {
    data++;
  }

  kfs_parse_node(&disk->root, data);
  return 0;
}

int32_t kfs_parse_node(kfs_node* node, uint8_t* data) {
  memcpy(&node->id, data, 4); data+=4;
  memcpy(&node->flags, data, 2); data+=2;
  memcpy(&node->sub_nodes_count, data, 2); data+=2;
  memcpy(&node->size, data, 4); data+=4;
  memcpy(&node->name, data, KFS_NAME_SIZE); data+=KFS_NAME_SIZE;

  for (int32_t i = 0; i < node->sub_nodes_count; i++) {
    memcpy(&node->sub_nodes[i], data, 4); data+=4;
  }

  return 0;
}

int32_t kfs_free(kfs_disk* disk) {
  delete [] disk->boot;
  return 0;
}

int32_t kfs_write_boot(kfs_disk* disk, const uint8_t* boot) {
  for (int32_t i = 0; i < KFS_BOOT_SECTOR_SIZE; i++) {
    disk->boot[i] = boot[i];
  }

  return 0;
}

int32_t kfs_add_node(kfs_disk* disk, kfs_node* node) {
  if (disk->last_id == 0) {
    kfs_add_subnode(disk, &disk->root, node);
  } else {
    kfs_node last_node;
    int32_t offset = _kfs_find_node_offset(disk, disk->last_id);
    if (offset == 0) {
      printf("Cannot find last node(id: %d)\n", disk->last_id);
      return -1;
    }

    kfs_parse_node(&last_node, disk->data+offset);
    kfs_add_subnode(disk, &last_node, node);
  }

  return 0;
}

int32_t kfs_add_subnode(kfs_disk* disk, kfs_node* parent, kfs_node* node) {
  if (!(parent->flags & KFS_NODE_FLAG_DIRECTORY)) {
    printf("File '%.*s' is not a directory\n", 16, parent->name);
    return -1;
  }
  int32_t offset = _kfs_find_first_free_block(disk); // in block_map
  if (!offset) {
    printf("No free blocks\n");
    return -1;
  }
  _kfs_claim_block(disk, offset);
  offset *= KFS_BLOCK_SIZE;
  parent->sub_nodes[parent->sub_nodes_count++] = offset;
  node->id = ++disk->last_id;
  _kfs_save_node(disk, parent, _kfs_find_node_offset(disk, parent->id));
  _kfs_save_node(disk, node, offset);
 return 0;
}

int32_t kfs_mkdir(kfs_disk* disk, const char* dirname) {
  if (dirname[0] != '/') {
    // log error "dirname should start with /"
    return -1;
  }

  const char *last = dirname+1, *current = dirname+1;
  std::vector<std::string> tokens;
  while (*current) {
    if (*current == '/') {
      tokens.push_back(std::string(last, current));
      last = current+1;
    }
    current++;
  }
  tokens.push_back(std::string(last, current));

  kfs_node parent;
  if (_kfs_get_node(disk, "root", &parent)) {
    printf("Can't get root node\n");
    return -1;
  }
  for (int32_t i = 0; i < tokens.size()-1; i++) {
    if (_kfs_get_node(disk, tokens[i].c_str(), &parent) == -1) {
      return -1;
    }
  }

  kfs_node new_node;
  new_node.id = 0;
  new_node.flags = KFS_NODE_FLAG_DIRECTORY;
  new_node.size = 0;
  memset(new_node.name, 0, 16);
  strcpy(new_node.name, tokens.back().c_str());
  new_node.sub_nodes_count = 0;

  if (kfs_add_subnode(disk, &parent, &new_node) == -1) {
    printf("Can't add subnode\n");
    return -1;
  }

  return 0;
}

int32_t kfs_write_file(kfs_disk* disk, const char* filename, const uint8_t* data, int32_t size) {
  if (filename[0] != '/') {
    // log error "Filename should start with /"
    return -1;
  }

  const char *last = filename+1, *current = filename+1;
  std::vector<std::string> tokens;
  while (*current) {
    if (*current == '/') {
      tokens.push_back(std::string(last, current));
      last = current+1;
    }
    current++;
  }
  tokens.push_back(std::string(last, current));

  kfs_node parent;
  if (_kfs_get_node(disk, "root", &parent)) {
    printf("Can't get root node\n");
    return -1;
  }
  for (int32_t i = 0; i < tokens.size()-1; i++) {
    if (_kfs_get_node(disk, tokens[i].c_str(), &parent) == -1) {
      return -1;
    }
  }

  kfs_node file;
  int32_t rc = _kfs_get_node(disk, tokens.back().c_str(), &file);
  if (rc == 0) {
    if (file.size > 0) {
      // rm
    }
    file.size = size;
  } else {
    file.id = 0;
    file.flags = 0;
    file.size = size;
    memset(file.name, 0, 16);
    strcpy(file.name, tokens.back().c_str());
  }

  if (size) {
    int32_t block_count = size / 1024;
    if (size % 1024 != 0) {
      block_count++;
    }
    file.sub_nodes_count = 1; // 1..block_count

    if (block_count == 1) {
      int32_t offset = _kfs_find_first_free_block(disk);
      if (!offset) {
        // log error - cant get free block
        return -1;
      }
      _kfs_claim_block(disk, offset);
      offset *= KFS_BLOCK_SIZE;
      file.sub_nodes[0] = offset;
      memcpy(disk->data+offset, &size, 4);
      memcpy(disk->data+offset+4, data, size);
    } else {
      std::vector<int> blocks;
      for (int32_t i = 0; i < block_count; i++) {
        int32_t offset = _kfs_find_first_free_block(disk);
        if (!offset) {
          // log error - cant get free block
          return -1;
        }
        _kfs_claim_block(disk, offset);
        blocks.push_back(offset);
      }
      int32_t offset = blocks[0] * KFS_BLOCK_SIZE;
      file.sub_nodes[0] = offset;
      memcpy(disk->data+offset, &size, 4);
      memcpy(disk->data+offset+4, data, size);

      /** TODO: FIXME: HANDLE NON CONTINIOUS BLOCKS */
    }
  }

  if (rc == 0) {
    _kfs_save_node(disk, &file, _kfs_find_node_offset(disk, file.id));
  } else {
    kfs_add_subnode(disk, &parent, &file);
  }

  return 0;
}

int32_t kfs_read(kfs_disk* disk, const char* filename, uint8_t* buffer) {
  if (filename[0] != '/') {
    // log error "Filename should start with /"
    return -1;
  }

  const char *last = filename+1, *current = filename+1;
  std::vector<std::string> tokens;
  while (*current) {
    if (*current == '/') {
      tokens.push_back(std::string(last, current));
      last = current+1;
    }
    current++;
  }
  tokens.push_back(std::string(last, current));

  kfs_node file;
  if (_kfs_get_node(disk, "root", &file)) {
    printf("Can't get root node\n");
    return -1;
  }
  for (int32_t i = 0; i < tokens.size(); i++) {
    if (_kfs_get_node(disk, tokens[i].c_str(), &file) == -1) {
      return -1;
    }
  }

  int32_t buffer_offset = 0;
  for (int32_t i = 0; i < file.sub_nodes_count; i++) {
    int32_t offset = file.sub_nodes[i];

    uint8_t* data = disk->data + offset;
    int32_t size = 0;
    memcpy(&size, data, 4);
    data+=4;
    for (int32_t j = 0; j < size; j++) {
      buffer[buffer_offset++] = data[j];
    }
  }

  return buffer_offset - file.size;
}

int32_t kfs_find(kfs_disk* disk, const char* filename, kfs_node* node) {
  if (filename[0] != '/') {
    // log error "Filename should start with /"
    return -1;
  }

  const char *last = filename+1, *current = filename+1;
  std::vector<std::string> tokens;
  while (*current) {
    if (*current == '/') {
      tokens.push_back(std::string(last, current));
      last = current+1;
    }
    current++;
  }
  tokens.push_back(std::string(last, current));

  if (_kfs_get_node(disk, "root", node)) {
    printf("Can't get root node\n");
    return -1;
  }
  for (int32_t i = 0; i < tokens.size(); i++) {
    if (_kfs_get_node(disk, tokens[i].c_str(), node) == -1) {
      return -1;
    }
  }

  return 0;
}

int32_t kfs_rm(kfs_disk*, const char* filename) {
  return 0;
}

int32_t kfs_open_file(kfs_disk* disk, const char* filename) {
  std::ifstream file(filename);
  if (file.fail() || !file.is_open()) {
    printf("No such file or directory: %s\n", filename);
    return -1;
  }
  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  uint8_t* buffer = new uint8_t[size];
  file.seekg(0, std::ios::beg); 
  file.read((char*)buffer, size);
  file.close();

  kfs_parse_data(disk, buffer);
  return 0;
}

int32_t kfs_save_file(kfs_disk* disk, const char* filename) {
  uint8_t* data = new uint8_t[65536];
  uint8_t* ptr = data;

  for (int32_t i = 0; i < KFS_BOOT_SECTOR_SIZE; i++) *ptr++ = disk->boot[i];

  disk->magic = KFS_MAGIC;
  memcpy(ptr, &disk->magic, 4); ptr+=4;
  memcpy(ptr, &disk->name, KFS_NAME_SIZE); ptr+=KFS_NAME_SIZE;
  disk->version = KFS_VERSION;
  memcpy(ptr, &disk->version, 4); ptr+=4;
  memcpy(ptr, &disk->last_id, 4); ptr+=4;

  for (int32_t i = 0; i < KFS_BITMAP_SIZE/4; i++) {
    memcpy(ptr, &disk->block_map[i], 4); ptr+=4;
  }

  while (ptr - data != 1024) {
    *ptr++ = 0;
  }

  _kfs_save_node(disk, &disk->root, _kfs_find_node_offset(disk, disk->root.id));

  int32_t i = 1024;
  while (ptr - data != 65536) {
    *ptr++ = disk->data[i++];
  }

  FILE* file = fopen(filename, "wb");
  fwrite(data, 1, 65536, file);
  fclose(file);

  delete [] data;

  return 0;
}

int32_t kfs_create_from_dir(kfs_disk* disk, const char* datadir) {
  return 0;
}