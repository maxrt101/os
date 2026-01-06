#include <drivers/keyboard/keyboard.h>
#include <monitor/monitor.h>
#include <memory/virtmem.h>
#include <memory/memory.h>
#include <event/event.h>
#include <util/util.h>
#include <kernel.h>
#include <arch.h>

#define FB_TEST 0

#if 0
const char* intro_art[] = {
  "                                       )                       \n",
  "                            )      ((     (                    \n",
  "                           (        ))     )                   \n",
  "                    )       )      //     (                    \n",
  "               _   (        __    (     ~->>                   \n",
  "        ,-----' |__,_~~___<'__`)-~__--__-~->> <                \n",
  "        | //  : | -__   ~__ o)____)),__ - '> >-  >             \n",
  "        | //  : |- \\_ \\ -\\_\\ -\\ \\ \\ ~\\_  \\ ->> - ,  >>\n",
  "        | //  : |_~_\\ -\\__\\ \\~'\\ \\ \\, \\__ . -<-  >>    \n",
  "        `-----._| `  -__`-- - ~~ -- ` --~> >                   \n",
  "         _/___\\_    //)_`//  | ||]                            \n",
  "   _____[_______]_[~~-_ (.L_/  ||                              \n",
  "  [____________________]' `\\_,/'/                             \n",
  "    ||| /          |||  ,___,'./                               \n",
  "    ||| \\          |||,'______|                               \n",
  "    ||| /          /|| I==||                                   \n",
  "    ||| \\       __/_||  __||__                                \n",
  "-----||-/------`-._/||-o--o---o---                             \n",
  "  ~~~~~'                                                       \n"
};
#endif

#if 0
const char* intro_art[] = {
  " ____________________ \n",
  "|  |              |  |\n",
  "|[]|  No Name OS  |[]|\n",
  "|  |     v0.1     |  |\n",
  "|  |              |  |\n",
  "|  |              |  |\n",
  "|  |______________|  |\n",
  "|                    |\n",
  "|     ____________   |\n",
  "|    | __      |  |  |\n",
  "|    ||  |     |  |  |\n",
  "|    ||__|     |  |  |\n",
  "|____|_________|__|__|\n"
};
#endif

#if 1
const char* intro_art[] = {
"         .--------.\n",
"  .----. |.------.|\n",
"  |____| || >_   ||\n",
"  |==..| ||      ||\n",
"  |----| |'------'|\n",
"  |    | | ..     |\n",
"  |____|~'--------'\n",
};
#endif

kernel_t kernel;

static event_ctx_t kernel_event_sys_timer;

__STATIC_INLINE void kernel_init_events(kernel_t * kernel) {
  event_register(KERNEL_EVENT_SYS_TIMER, &kernel_event_sys_timer);
}

__STATIC_INLINE void print_intro_art(void) {
  kprintf("OS with no name - v0.1\n");
  for (int i = 0; i < UTIL_ARR_SIZE(intro_art); ++i) {
    kprintf("%s", intro_art[i]);
  }
  kprintf("\n");
}

void kernel_init(kernel_t * kernel) {
  arch_disable_interrupts();

  kernel_init_events(kernel);
  kernel_init_port(kernel);
  kernel_init_io(kernel);

  print_intro_art();

  kprintf("Initializing kernel\n");

  memmap_dump(&kernel->mem.map);
  kernel_phys_map(&kernel->mem.map);
  arch_init(kernel);
  kernel_virt_map();
  kheap_init();

  sched_init(&kernel->sched);

  arch_enable_interrupts();
}

#if FB_TEST
void fb_test() {
  framebuffer_draw_line(&kernel.framebuffer, (position_t){100, 200}, (position_t){200, 300}, (color_t){255, 0, 0});
  framebuffer_draw_line(&kernel.framebuffer, (position_t){200, 300}, (position_t){300, 400}, (color_t){0, 255, 0});
  framebuffer_draw_line(&kernel.framebuffer, (position_t){300, 400}, (position_t){400, 500}, (color_t){0, 0, 255});
  framebuffer_scroll(&kernel.framebuffer, 100, (color_t){128, 128, 128});
}
#endif

int kernel_main(void) {
  kernel_init(&kernel);

  UTIL_IF_1(FB_TEST, fb_test());

  kprintf("Kernel initialized\n");

  monitor_main();

  arch_abort();
  return 0;
}
