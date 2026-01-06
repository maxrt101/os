#pragma once

#include <stdint.h>
#include <arch.h>

typedef uint64_t task_id_t;

typedef void (*task_entry_t)(void*);

typedef struct task_t {
  task_id_t       id;
  arch_task_ctx_t ctx;
  struct task_t * next;
} task_t;

typedef struct {
  task_t *  current;
  task_t *  head;
  task_id_t next_id;
} sched_t;

void sched_init(sched_t * sched);
void sched_add(sched_t * sched, task_entry_t entry, void * arg);
arch_task_ctx_t sched_switch(sched_t * sched, arch_task_ctx_t current);
