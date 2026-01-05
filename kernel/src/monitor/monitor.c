#include <monitor/monitor.h>
#include <drivers/tty/ansi.h>
#include <util/string.h>
#include <util/fixed.h>
#include <time/sleep.h>
#include <time/time.h>
#include <kernel.h>

#include "memory/memory.h"

#define MONITOR_ARGV_MAX 16
#define MONITOR_DEBUG_SC  0
#define MONITOR_DEBUG_IRQ 0

#if MONITOR_DEBUG_IRQ
extern volatile int last_irq;
#endif

#if MONITOR_DEBUG_SC
extern volatile uint32_t last_raw_sc;
#endif

void monitor_cmd_test(int argc, char ** argv) {
  kprintf("test\n");

  time_t t0 = time_hours(1);
  time_t t1 = time_milliseconds(600000);
  time_t t2 = time_milliseconds(4200000);

  time_t t0_s   = time_convert(t0, TIME_SECONDS);      kprintf("t0 hr->sec %d.%d\n", t0_s.value, t0_s.frac);
  time_t t0_min = time_convert(t0, TIME_MINUTES);      kprintf("t0 hr->min %d.%d\n", t0_min.value, t0_min.frac);
  time_t t0_ms  = time_convert(t0, TIME_MILLISECONDS); kprintf("t0 hr->ms %d.%d\n", t0_ms.value, t0_ms.frac);
  time_t t1_min = time_convert(t1, TIME_MINUTES);      kprintf("t1 ms->min %d.%d\n", t1_min.value, t1_min.frac);
  time_t t2_min = time_convert(t2, TIME_MINUTES);      kprintf("t2 ms->min %d.%d\n", t2_min.value, t2_min.frac);
  time_t t2_hr  = time_convert(t2, TIME_HOURS);        kprintf("t2 ms->hr %d.%d\n", t2_hr.value, t2_hr.frac);

  uintptr_t p1 = kernel_phys_alloc(1);
  uintptr_t p2 = kernel_phys_alloc(1);
  kprintf("Allocated 1 page at %p\n", p1);
  kprintf("Allocated 1 page at %p\n", p2);
  kernel_phys_free(p2, 1);
  kernel_phys_free(p1, 1);

  uintptr_t p3 = kernel_phys_alloc(1);
  kprintf("Allocated 1 page at %p\n", p3);
  kernel_phys_free(p3, 1);

  kprintf("123\t4\n");
  kprintf("1\t2\n");
  kprintf("12345\t6\n");

  kprintf("%4d\n", 42);
  kprintf("%04d\n", 42);
  kprintf("%08x\n", 0xFFAAFF);
  kprintf("%08p\n", 0xFFAAFF);
  kprintf("%016p\n", 0xFA);
  kprintf("%8s\n", "abc");

  // fixed_t f = fixed_new(1, 5);
  // kprintf("%d.%d * %d = %d\n", f.whole, f.decimal, 8, fixed_multiply_int(8, f));

  fixed_t f1 = INT2FIXED(8);
  fixed_t f2 = FIXED(1, 5, 10);
  fixed_t f = FIXED_MUL(f1, f2);
  kprintf("8 * 1.5 = %x %d (%d.%d)\n", f, FIXED2INT(f), FIXED_WHOLE(f), FIXED_FRACTION(f));

  {
    void * p1 = kmalloc(100);
    void * p2 = kmalloc(200);
    kprintf("Allocated: %p, %p\n", p1, p2);
    kfree(p1);
    void *p3 = kmalloc(50); // Should end up in the same spot as p1 (reusing memory)
    kprintf("Reused: %p\n", p3);
  }
}

__OPTIMIZE(0) void monitor_cmd_test2(int argc, char ** argv) {
  if (argc != 2) {
    kprintf("Usage: test2 N\nWhere N = [1, 3]\n");
    return;
  }

  int n = atoi(argv[1], 10);

  void * jump = NULL;

  switch (n) {
    case 0:
case0:
      kprintf("case 0\n");
      return;

    case 1:
      jump = &&case0;
      break;
case1:
      kprintf("case 1\n");
      return;

    case 2:
      jump = &&case1;
      break;
case2:
      kprintf("case 2\n");
      return;

    case 3:
      jump = &&case2;
      break;

    default:
      return;
  }

  goto * jump;
}

void monitor_cmd_panic(int argc, char ** argv) {
  if (argc == 1) {
    kpanic("Manually triggered\n");
  } else {
    kpanic("%s\n", argv[1]);
  }
}

void monitor_cmd_int(int argc, char ** argv) {
  if (argc != 2) {
    kprintf("Usage: int N\n");
    return;
  }

  int irq = atoi(argv[1], 10);

  arch_trigger_irq(irq);
}

#pragma GCC diagnostic ignored "-Wdiv-by-zero"
__OPTIMIZE(0) void monitor_cmd_div0(int argc, char ** argv) {
#if USE_WAIT_BEFORE_TRIGGERING_IRQ
  for (size_t i = 0; i < 500000000; ++i) {
    ;
  }
#endif

  int x = 0 / 0;
  kprintf("%d\n", x);
}
#pragma pop

void monitor_cmd_clear(int argc, char ** argv) {
  kprintf("\033[2J");
}

void monitor_cmd_color(int argc, char ** argv) {
  if (argc == 1) {
    kprintf("Usage color [--list] N...\n");
    return;
  }

  if (!strcmp(argv[1], "--list")) {
    kprintf("FG Black   %d\n", ANSI_COLOR_FG_BLACK);
    kprintf("FG Red     %d\n", ANSI_COLOR_FG_RED);
    kprintf("FG Green   %d\n", ANSI_COLOR_FG_GREEN);
    kprintf("FG Yellow  %d\n", ANSI_COLOR_FG_YELLOW);
    kprintf("FG Blue    %d\n", ANSI_COLOR_FG_BLUE);
    kprintf("FG Magenta %d\n", ANSI_COLOR_FG_MAGENTA);
    kprintf("FG Cyan    %d\n", ANSI_COLOR_FG_CYAN);
    kprintf("FG White   %d\n", ANSI_COLOR_FG_WHITE);
    kprintf("FG Default %d\n", ANSI_COLOR_FG_DEFAULT);
    kprintf("BG Black   %d\n", ANSI_COLOR_BG_BLACK);
    kprintf("BG Red     %d\n", ANSI_COLOR_BG_RED);
    kprintf("BG Green   %d\n", ANSI_COLOR_BG_GREEN);
    kprintf("BG Yellow  %d\n", ANSI_COLOR_BG_YELLOW);
    kprintf("BG Blue    %d\n", ANSI_COLOR_BG_BLUE);
    kprintf("BG Magenta %d\n", ANSI_COLOR_BG_MAGENTA);
    kprintf("BG Cyan    %d\n", ANSI_COLOR_BG_CYAN);
    kprintf("BG White   %d\n", ANSI_COLOR_BG_WHITE);
    kprintf("BG Default %d\n", ANSI_COLOR_BG_DEFAULT);
  } else {
    for (int i = 1; i < argc; ++i) {
      kprintf("\033[%sm", argv[i]);
    }
  }
}

void monitor_cmd_scale(int argc, char ** argv) {
  if (argc == 1) {
    kprintf("Usage scale WHOLE [FRAC]\n");
    return;
  }

  int whole = atoi(argv[1], 10);
  int frac = argc > 2 ? atoi(argv[2], 10) : 0;

  kernel.tty.scale = FIXED(whole, frac, 10);
}

void monitor_cmd_tick(int argc, char ** argv) {
  kprintf("%d\n", arch_get_tick());
}

void monitor_cmd_time(int argc, char ** argv) {
  kprintf("%d ms\n", time_get_ms());
}

void monitor_cmd_sleep(int argc, char ** argv) {
  if (argc == 1) {
    kprintf("Usage: sleep SEC\n");
    return;
  }

  usleep(atoi(argv[1], 10) * 1000 * 1000);
}

void monitor_cmd_usleep(int argc, char ** argv) {
  if (argc == 1) {
    kprintf("Usage: usleep USEC\n");
    return;
  }

  usleep(atoi(argv[1], 10));
}

void monitor_cmd_mem(int argc, char ** argv) {
  kernel_phys_dump();
}

static struct {
  const char * name;
  void (*handler)(int, char **);
} commands[] = {
  {"test", monitor_cmd_test},
  {"test2", monitor_cmd_test2},
  {"panic", monitor_cmd_panic},
  {"int", monitor_cmd_int},
  {"div0", monitor_cmd_div0},
  {"clear", monitor_cmd_clear},
  {"color", monitor_cmd_color},
  {"scale", monitor_cmd_scale},
  {"tick", monitor_cmd_tick},
  {"time", monitor_cmd_time},
  {"sleep", monitor_cmd_sleep},
  {"usleep", monitor_cmd_usleep},
  {"mem", monitor_cmd_mem},
};

__STATIC_INLINE void monitor_run_cmd(char * buf) {
  int argc = 0;
  char * argv[MONITOR_ARGV_MAX];

  char * token = NULL;

  while ((token = strtok(&buf, ' '))) {
    argv[argc++] = token;
  }

  if (!argc) {
    return;
  }

  for (size_t i = 0; i < UTIL_ARR_SIZE(commands); ++i) {
    if (!strcmp(argv[0], commands[i].name)) {
      commands[i].handler(argc, argv);
      return;
    }
  }

  kprintf("Unknown command '%s'\n", argv[0]);
}

void monitor_main() {
  int cycle = 0;
  char buf[32] = {0};
  size_t buf_index = 0;

  kprintf("Monitor\n> ");

  while (1) {
    int stat_x = kernel.tty.fb->size.width / tty_get_glyph_width(&kernel.tty) - 15;

    kprintf("\033[s");
    kprintf("\033[0;%dH cycle=%d", stat_x, cycle);
    kprintf("\033[1;%dH tick=%d", stat_x, arch_get_tick());
#if MONITOR_DEBUG_IRQ
    kprintf("\033[2;%dH irq=%d", stat_x, last_irq);
#endif
#if MONITOR_DEBUG_SC
    kprintf("\033[4;%dH rsc=%p", stat_x, last_raw_sc);
#endif
    kprintf("\033[u");

    if (kgetline_async(buf, sizeof(buf), &buf_index)) {
      monitor_run_cmd(buf);

      memset(buf, 0, sizeof(buf));
      buf_index = 0;

      kprintf("> ");
    }

    cycle++;
  }
}