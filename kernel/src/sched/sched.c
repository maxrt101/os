#include <sched/sched.h>
#include <memory/memory.h>
#include <util/string.h>
#include <kernel.h>

#include "util/assert.h"

void sched_init(sched_t * sched) {
  task_t * idle = kmalloc(sizeof(task_t));

  idle->id   = 0;
  idle->next = idle;
  memset(&idle->ctx, 0, sizeof(idle->ctx));

  sched->current = idle;
  sched->head = idle;

  kprintf("sched: Scheduler initialized\n");
}

void sched_add(sched_t * sched, task_entry_t entry, void * arg) {
  task_t * task = kmalloc(sizeof(task_t));

  void * stack_ptr = kmalloc(4096);
  void * stack_top = stack_ptr + 4096;

  task->id  = ++sched->next_id;
  task->ctx = arch_create_task_ctx(entry, arg, stack_top);

  task->next        = sched->head->next;
  sched->head->next = task;

  kprintf("sched: New task #%lu (%p)\n", task->id, entry);
}

arch_task_ctx_t sched_switch(sched_t * sched, arch_task_ctx_t current) {
  ASSERT_RETURN(sched && sched->current, current);

  sched->current->ctx = current;

  sched->current = sched->current->next;

  return sched->current->ctx;
}