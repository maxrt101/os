#include <event/event.h>
#include <util/assert.h>
#include <stddef.h>
#include <kernel.h>

static event_ctx_t * events = NULL;

void event_register(event_t evt, event_ctx_t * ctx) {
  ASSERT_RETURN(ctx);

  ctx->evt = evt;
  ctx->head = NULL;

  if (events) {
    ctx->next = events;
    events = ctx;
  } else {
    ctx->next = NULL;
    events = ctx;
  }
}

void event_subscribe(event_t evt, evnet_handler_t * handler) {
  ASSERT_RETURN(handler);

  event_ctx_t * ctx = events;

  while (ctx) {
    if (ctx->evt == evt) {
      break;
    }
    ctx = ctx->next;
  }

  ASSERT_RETURN(ctx);

  handler->next = ctx->head;
  ctx->head = handler;
}

void event_dispatch(event_t evt) {
  event_ctx_t * ctx = events;

  while (ctx) {
    if (ctx->evt == evt) {
      break;
    }
    ctx = ctx->next;
  }

  ASSERT_RETURN(ctx);

  evnet_handler_t * handler = ctx->head;

  while (handler) {
    handler->fn(handler->ctx);
    handler = handler->next;
  }
}
