#include <x86_64.h>
#include <drivers/keyboard/keyboard.h>
#include <util/string.h>
#include <stdbool.h>

#define PS2_REG_DATA    0x60
#define PS2_REG_STATUS  0x64
#define PS2_REG_COMMAND 0x64

#define PS2_CMD_SET_LED                               0xED
#define PS2_CMD_ECHO                                  0xEE
#define PS2_CMD_SET_SCAN_CODE_SET                     0xF0
#define PS2_CMD_KB_ID                                 0xF2
#define PS2_CMD_SET_TYPEMATIC                         0xF3
#define PS2_CMD_SCAN_ENABLE                           0xF4
#define PS2_CMD_SCAN_DISABLE                          0xF5
#define PS2_CMD_SET_DEFAULT                           0xF6
#define PS2_CMD_SET_AUTOREPEAT                        0xF7
#define PS2_CMD_SET_MAKE_REPEAT                       0xF8
#define PS2_CMD_SET_MAKE_ONLY                         0xF9
#define PS2_CMD_SET_TYPEMATIC_AUTOREPEAT_MAKE_REPEAT  0xFA
#define PS2_CMD_SET_KEY_TYPEMATIC_AUTOREPEAT          0xFB
#define PS2_CMD_SET_KEY_MAKE_RELEASE                  0xFC
#define PS2_CMD_SET_KEY_MAKE                          0xFD
#define PS2_CMD_RESEND_LAT_BYTE                       0xFE
#define PS2_CMD_RESET_AND_SELFTEST                    0xFF

#define PS2_RES_ERR_00                                0x00
#define PS2_RES_SELFTEST_OK                           0xAA
#define PS2_RES_ECHO                                  0xEE
#define PS2_RES_ACK                                   0xFA
#define PS2_RES_SELFTEST_FAIL_FC                      0xFC
#define PS2_RES_SELFTEST_FAIL_FD                      0xFD
#define PS2_RES_RESEND                                0xFE
#define PS2_RES_ERR_FF                                0xFF

#define PS2_KEYBOARD_IRQ 33

#define FLAG_READING    (1 << 0)
#define FLAG_CAPS       (1 << 1)
#define FLAG_SHIFT      (1 << 2)
#define FLAG_ALT        (1 << 3)
#define FLAG_CTRL       (1 << 4)

key_scancode_t scancode_table[] = {
  0,
  0,
  KEY_SCANCODE_1,
  KEY_SCANCODE_2,
  KEY_SCANCODE_3,
  KEY_SCANCODE_4,
  KEY_SCANCODE_5,
  KEY_SCANCODE_6,
  KEY_SCANCODE_7,
  KEY_SCANCODE_8,
  KEY_SCANCODE_9,
  KEY_SCANCODE_0,
  KEY_SCANCODE_HYPHEN,
  KEY_SCANCODE_EQUALS,
  KEY_SCANCODE_BACKSPACE,
  KEY_SCANCODE_TAB,
  KEY_SCANCODE_Q,
  KEY_SCANCODE_W,
  KEY_SCANCODE_E,
  KEY_SCANCODE_R,
  KEY_SCANCODE_T,
  KEY_SCANCODE_Y,
  KEY_SCANCODE_U,
  KEY_SCANCODE_I,
  KEY_SCANCODE_O,
  KEY_SCANCODE_P,
  KEY_SCANCODE_LEFT_SQUARE_BRACE,
  KEY_SCANCODE_RIGHT_SQUARE_BRACE,
  KEY_SCANCODE_ENTER,
  0,
  KEY_SCANCODE_A,
  KEY_SCANCODE_S,
  KEY_SCANCODE_D,
  KEY_SCANCODE_F,
  KEY_SCANCODE_G,
  KEY_SCANCODE_H,
  KEY_SCANCODE_J,
  KEY_SCANCODE_K,
  KEY_SCANCODE_L,
  KEY_SCANCODE_SEMICOLON,
  KEY_SCANCODE_SINGLE_QUOTES,
  KEY_SCANCODE_BACKTICK,
  0,
  KEY_SCANCODE_BACKSLASH,
  KEY_SCANCODE_Z,
  KEY_SCANCODE_X,
  KEY_SCANCODE_C,
  KEY_SCANCODE_V,
  KEY_SCANCODE_B,
  KEY_SCANCODE_N,
  KEY_SCANCODE_M,
  KEY_SCANCODE_COMMA,
  KEY_SCANCODE_DOT,
  KEY_SCANCODE_SLASH,
  0,
  KEY_SCANCODE_STAR,
  0,
  KEY_SCANCODE_SPACE,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  KEY_SCANCODE_ESC,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  KEY_SCANCODE_SHIFT,
  0x1C, // File separator
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  KEY_SCANCODE_SLASH,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0x1E, // Record separator
  0x1F, // Unit separator
  KEY_SCANCODE_SPACE,
  KEY_SCANCODE_EXCLAMATION_POINT,
  KEY_SCANCODE_QUOTES,
  KEY_SCANCODE_HASH,
  KEY_SCANCODE_DOLLAR,
  KEY_SCANCODE_PERCENT,
  KEY_SCANCODE_AMP,
  KEY_SCANCODE_SINGLE_QUOTES,
  KEY_SCANCODE_LEFT_PAREN,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  KEY_SCANCODE_COMMA,
};

key_scancode_t shifted_scancode_table[] = {
  0,
  0,
  KEY_SCANCODE_EXCLAMATION_POINT,
  KEY_SCANCODE_AT,
  KEY_SCANCODE_HASH,
  KEY_SCANCODE_DOLLAR,
  KEY_SCANCODE_PERCENT,
  KEY_SCANCODE_CARET,
  KEY_SCANCODE_AMP,
  KEY_SCANCODE_STAR,
  KEY_SCANCODE_LEFT_PAREN,
  KEY_SCANCODE_RIGHT_PAREN,
  KEY_SCANCODE_UNDERSCORE,
  KEY_SCANCODE_PLUS,
  KEY_SCANCODE_BACKSPACE,
  KEY_SCANCODE_TAB,
  KEY_SCANCODE_Q,
  KEY_SCANCODE_W,
  KEY_SCANCODE_E,
  KEY_SCANCODE_R,
  KEY_SCANCODE_T,
  KEY_SCANCODE_Y,
  KEY_SCANCODE_U,
  KEY_SCANCODE_I,
  KEY_SCANCODE_O,
  KEY_SCANCODE_P,
  KEY_SCANCODE_LEFT_BRACE,
  KEY_SCANCODE_RIGHT_BRACE,
  KEY_SCANCODE_ENTER,
  0,
  KEY_SCANCODE_A,
  KEY_SCANCODE_S,
  KEY_SCANCODE_D,
  KEY_SCANCODE_F,
  KEY_SCANCODE_G,
  KEY_SCANCODE_H,
  KEY_SCANCODE_J,
  KEY_SCANCODE_K,
  KEY_SCANCODE_L,
  KEY_SCANCODE_COLON,
  KEY_SCANCODE_QUOTES,
  KEY_SCANCODE_TILDA,
  0,
  KEY_SCANCODE_VBAR,
  KEY_SCANCODE_Z,
  KEY_SCANCODE_X,
  KEY_SCANCODE_C,
  KEY_SCANCODE_V,
  KEY_SCANCODE_B,
  KEY_SCANCODE_N,
  KEY_SCANCODE_M,
  KEY_SCANCODE_LESS_THEN,
  KEY_SCANCODE_MORE_THEN,
  KEY_SCANCODE_QUESTION_MARK,
  0,
  KEY_SCANCODE_STAR,
  0,
  KEY_SCANCODE_SPACE,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  KEY_SCANCODE_ESC,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  KEY_SCANCODE_SHIFT,
  0x1C, // File separator
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  KEY_SCANCODE_QUESTION_MARK,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0x1E, // Record separator
  0x1F, // Unit separator
  KEY_SCANCODE_SPACE,
  KEY_SCANCODE_EXCLAMATION_POINT,
  KEY_SCANCODE_QUOTES,
  KEY_SCANCODE_HASH,
  KEY_SCANCODE_DOLLAR,
  KEY_SCANCODE_PERCENT,
  KEY_SCANCODE_AMP,
  KEY_SCANCODE_SINGLE_QUOTES,
  KEY_SCANCODE_LEFT_PAREN,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  KEY_SCANCODE_COMMA,
};

static volatile struct {
  uint8_t flags;
  uint8_t current;

  struct {
    key_t   data[16];
    uint8_t write_index;
    uint8_t read_index;
  } buffer;
} ps2_ctx;

__STATIC_INLINE key_flags_t ps2_flags_to_key_flags(uint8_t flags) {
  uint8_t result = 0;

  if (flags & FLAG_SHIFT) result |= KEY_FLAG_SHIFT;
  if (flags & FLAG_ALT)   result |= KEY_FLAG_ALT;
  if (flags & FLAG_CTRL)  result |= KEY_FLAG_CTRL;
  if (flags & FLAG_CAPS)  result |= KEY_FLAG_SHIFT;

  return result;
}

__STATIC_INLINE bool ps2_should_shift() {
  return (ps2_ctx.flags & FLAG_SHIFT) || (ps2_ctx.flags & FLAG_CAPS);
}

__STATIC_INLINE bool ps2_can_pop() {
  return ps2_ctx.buffer.read_index != ps2_ctx.buffer.write_index;
}

__STATIC_INLINE void ps2_push_key() {
  ps2_ctx.buffer.data[ps2_ctx.buffer.write_index].scancode = ps2_should_shift() ? shifted_scancode_table[ps2_ctx.current] : scancode_table[ps2_ctx.current];
  ps2_ctx.buffer.data[ps2_ctx.buffer.write_index].flags = ps2_flags_to_key_flags(ps2_ctx.flags);

#if USE_KEY_RAW_SCANCODE
  ps2_ctx.buffer.data[ps2_ctx.buffer.write_index].raw = ps2_ctx.current;
#endif

  ps2_ctx.buffer.write_index++;

  if (ps2_ctx.buffer.write_index >= UTIL_ARR_SIZE(ps2_ctx.buffer.data)) {
    ps2_ctx.buffer.write_index = 0;
  }

  if (ps2_ctx.buffer.write_index == ps2_ctx.buffer.read_index) {
    ps2_ctx.buffer.read_index++;
    if (ps2_ctx.buffer.read_index >= UTIL_ARR_SIZE(ps2_ctx.buffer.data)) {
      ps2_ctx.buffer.read_index = 0;
    }
  }
}

__STATIC_INLINE key_t ps2_pop_key() {
  // TODO: Check if can pop

  key_t data = ps2_ctx.buffer.data[ps2_ctx.buffer.read_index];

  ps2_ctx.buffer.read_index++;

  if (ps2_ctx.buffer.read_index >= UTIL_ARR_SIZE(ps2_ctx.buffer.data)) {
    ps2_ctx.buffer.read_index = 0;
  }

  return data;
}

volatile uint32_t last_raw_sc = 0;

void x86_64_ps2_irq_handler(void * ctx) {
  if (inb(PS2_REG_STATUS) & 1) {
    ps2_ctx.current = inb(PS2_REG_DATA);
    last_raw_sc = ps2_ctx.current;

    switch (ps2_ctx.current) {
      // TODO: CTRL & ALT

      // LR Shift press
      case 0x2A:
      case 0x36:
        ps2_ctx.flags |= FLAG_SHIFT;
        break;

      // LR Shift release
      case 0xAA:
      case 0xB6:
        ps2_ctx.flags &= ~FLAG_SHIFT;
        break;

      // Capslock
      case 0x3A:
        if (ps2_ctx.flags & FLAG_CAPS) {
          ps2_ctx.flags &= ~FLAG_CAPS;
        } else {
          ps2_ctx.flags |= FLAG_CAPS;
        }
        break;

      default:
        // High bit set if key is released
        if (ps2_ctx.current & 0x80) {
          // Ignore releases, only record presses
          break;
        }
        ps2_push_key();
        break;
    }
  }
}

__STATIC_INLINE void ps2_flush_buf() {
  while ((inb(PS2_REG_COMMAND) >> 0) & 1) {
    inb(PS2_REG_DATA);
  }
}

__STATIC_INLINE void ps2_cmd_trx(uint8_t * cmd, uint8_t cmd_size, uint8_t * res, uint8_t res_size) {
  for (uint8_t i = 0; i < cmd_size; ++i) {
    outb(PS2_REG_COMMAND, cmd[i]);
  }
  for (uint8_t i = 0; i < res_size; ++i) {
    // TODO: Check if should be reading from data register
    uint8_t tmp = inb(PS2_REG_DATA);
    if (res) {
      res[i] = tmp;
    }
  }
}

void x86_64_init_ps2() {
  vmemset(&ps2_ctx, 0, sizeof(ps2_ctx));

  // Run selftest, discarding output
  uint8_t cmd = PS2_CMD_RESET_AND_SELFTEST;
  ps2_cmd_trx(&cmd, 1, NULL, 2);

  x86_64_irq_register_handler(PS2_KEYBOARD_IRQ, &x86_64_ps2_irq_handler);
  x86_64_irq_unmask(PS2_KEYBOARD_IRQ);
}

key_t key_get_port() {
  if (ps2_can_pop()) {
    return ps2_pop_key();
  }

  return (key_t){KEY_SCANCODE_NONE, KEY_FLAG_NONE};
}

key_t key_wait_port() {
  while (!ps2_can_pop()) {
    ;
  }

  return ps2_pop_key();
}
