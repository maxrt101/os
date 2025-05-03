#include <x86_64.h>

/** PIT OCW bit masks */
#define PIT_OCW_MASK_BINCOUNT       1
#define PIT_OCW_MASK_MODE           0xE
#define PIT_OCW_MASK_RL             0x30
#define PIT_OCW_MASK_COUNTER        0xC0

/** PIT Command Control Bits */
#define PIT_OCW_BINCOUNT_BINARY     0
#define PIT_OCW_BINCOUNT_BCD        1

#define PIT_OCW_RL_LATCH            0
#define PIT_OCW_RL_LSBONLY          0x10
#define PIT_OCW_RL_MSBONLY          0x20
#define PIT_OCW_RL_DATA             0x30

#define PIT_REG_COUNTER0            0x40
#define PIT_REG_COUNTER1            0x41
#define PIT_REG_COUNTER2            0x42
#define PIT_REG_COMMAND             0x43

#define PIT_IRQ                     32

static volatile uint64_t tick_counter = 0;

void x86_64_pit_irq_handler(void * ctx) {
  tick_counter++;
}

__STATIC_INLINE void pit_send_cmd(uint8_t cmd) {
  outb(PIT_REG_COMMAND, cmd);
}

__STATIC_INLINE void pit_send_data(uint8_t counter, uint8_t data) {
  uint8_t port = 0;

  switch (counter) {
    case PIT_COUNTER_0:
      port = PIT_REG_COUNTER0;
      break;

    case PIT_COUNTER_1:
      port = PIT_REG_COUNTER1;
      break;

    case PIT_COUNTER_2:
      port = PIT_REG_COUNTER2;
      break;

    default:
      return;
  }

  outb(port, data);
}

void x86_64_init_pit(uint32_t freq, uint8_t counter, uint8_t mode) {
  tick_counter = 0;

  uint32_t divisor = 1193180 / freq;

  pit_send_cmd(mode | PIT_OCW_RL_DATA | counter);

  pit_send_data(counter, divisor & 0xFF);
  pit_send_data(counter, divisor >> 8);

  x86_64_irq_register_handler(PIT_IRQ, x86_64_pit_irq_handler);
  x86_64_irq_unmask(PIT_IRQ);
}

uint64_t arch_get_tick(void) {
  return tick_counter;
}
