// #include "code16gcc.h"
#include "boot.h"

#include <sys/asm.h>
#include <fs/kfs.h>
#include <string.h>

ASM_SECTION(".latetext32");

struct Token {
  const char* begin;
  const char* end;
};

uint32_t _kfs_find_node_offset(kfs_disk* disk, uint32_t node_id);
int32_t _kfs_get_subnode(kfs_node* start_node, uint8_t* data, const char* filename, kfs_node* node);
uint32_t _kfs_find_subnode_offset(kfs_node* start_node, uint8_t* data, uint32_t node_id);


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

  for (int i = 0; i < start_node->sub_nodes_count; i++) {
    kfs_parse_node(&current_node, data+start_node->sub_nodes[i]);

    if (current_node.id == node_id) {
      return start_node->sub_nodes[i];
    } else {
      if (current_node.flags & KFS_NODE_FLAG_DIRECTORY) {
        int offset = _kfs_find_subnode_offset(&current_node, data, node_id);
        if (offset != 0) {
          return offset;
        }
      }
    }
  }

  return 0;
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

  for (int i = 0; i < KFS_BITMAP_SIZE/4; i++) {
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

int32_t kfs_read(kfs_disk* disk, const char* filename, uint8_t* buffer) {
  if (filename[0] != '/') {
    // log error "Filename should start with /"
    return -1;
  }

  const char *last = filename+1, *current = filename+1;
  // std::vector<std::string> tokens;
  Token tokens[16];
  Token* tokens_ptr = tokens;
  while (*current) {
    if (*current == '/') {
      // tokens.push_back(std::string(last, current));
      tokens_ptr->begin = last;
      tokens_ptr->end = current;
      tokens_ptr++;
      last = current+1;
    }
    current++;
  }
  // tokens.push_back(std::string(last, current));
  tokens_ptr->begin = last;
  tokens_ptr->end = current;
  tokens_ptr++;

  kfs_node file;
  if (_kfs_get_node(disk, "root", &file)) {
    // printf("Can't get root node\n");
    return -1;
  }

  for (int32_t i = 0; i < tokens_ptr - tokens; i++) {
    char tmp_buf[32];
    int j = 0;
    for (const char* it = tokens[i].begin; it != tokens[i].end; it++) {
      tmp_buf[j++] = *it;
    }
    if (_kfs_get_node(disk, tmp_buf, &file) == -1) {
      // puts("Can't get node\n");
      return -1;
    }
  }

  int32_t buffer_offset = 0;
  for (int i = 0; i < file.sub_nodes_count; i++) {
    int32_t offset = file.sub_nodes[i];

    uint8_t* data = disk->data + offset;
    int32_t size = 0;
    memcpy(&size, data, 4);
    for (int32_t j = 0; j < size; j++) {
      buffer[buffer_offset++] = data[j+4];
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
  // std::vector<std::string> tokens;
  Token tokens[16];
  Token* tokens_ptr = tokens;
  while (*current) {
    if (*current == '/') {
      // tokens.push_back(std::string(last, current));
      tokens_ptr->begin = last;
      tokens_ptr->end = current;
      tokens_ptr++;
      last = current+1;
    }
    current++;
  }
  // tokens.push_back(std::string(last, current));
  tokens_ptr->begin = last;
  tokens_ptr->end = current;
  tokens_ptr++;

  if (_kfs_get_node(disk, "root", node)) {
    // puts("Can't get root node\n");
    return -1;
  }
  for (int32_t i = 0; i < tokens_ptr - tokens; i++) {
    char tmp_buf[32];
    int32_t j = 0;
    for (const char* it = tokens[i].begin; it != tokens[i].end; it++) {
      tmp_buf[j++] = *it;
    }
    // puts(tmp_buf);
    // puts("\n");
    if (_kfs_get_node(disk, tmp_buf, node) == -1) {
      // puts("Can't get node\n");
      return -1;
    }
  }

  return 0;
}
