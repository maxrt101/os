#pragma once

#include <stdint.h>

typedef uint32_t event_t;

typedef void (*evnet_handler_fn_t)(void *);

typedef struct evnet_handler_t {
  struct evnet_handler_t * next;
  evnet_handler_fn_t fn;
  void * ctx;
} evnet_handler_t;

typedef struct event_ctx_t {
  struct event_ctx_t * next;
  event_t evt;
  evnet_handler_t * head;
} event_ctx_t;

void event_register(event_t evt, event_ctx_t * ctx);
void event_subscribe(event_t evt, evnet_handler_t * handler);
void event_dispatch(event_t evt);
