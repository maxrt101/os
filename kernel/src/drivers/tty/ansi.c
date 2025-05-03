#include <drivers/tty/ansi.h>
#include <util/string.h>
#include <util/assert.h>
#include <util/util.h>
#include <stddef.h>
#include <limits.h>

#define IS_NUM(c) ((c) >= '0' && (c) <= '9')

__STATIC_INLINE int num_size(const char * str) {
  int i = 0;

  while (IS_NUM(str[i])) {
    i++;
  }

  return i;
}

const char * ansi_parse_esc_seq(const char * str, ansi_esc_seq_handler_t handler, void * ctx) {
  ASSERT_RETURN(str, NULL);
  ASSERT_RETURN(str[0] == ANSI_ESC_CODE, str);

  size_t index = 1;
  ansi_esc_seq_t seq;

  seq.type = ANSI_ESC_SEQ_INVALID;
  seq.payload.raw._1 = INT_MAX;
  seq.payload.raw._2 = INT_MAX;

  if (str[index] == '[') {
    index += 1;

    if (IS_NUM(str[index])) {
      seq.payload.raw._1 = atoi(&str[index], 10);
      index += num_size(&str[index]);
    }

    if (str[index] == ';') {
      index += 1;
      seq.payload.raw._2 = atoi(&str[index], 10);
      index += num_size(&str[index]);
    }
  } else {
    if (IS_NUM(str[index])) {
      seq.payload.raw._1 = atoi(&str[index], 10);
      index += num_size(&str[index]);
    }
  }

  switch (str[index]) {
    case 'A': seq.type = ANSI_ESC_SEQ_MOVE_CURSOR_UP; break;
    case 'B': seq.type = ANSI_ESC_SEQ_MOVE_CURSOR_DOWN; break;
    case 'C': seq.type = ANSI_ESC_SEQ_MOVE_CURSOR_RIGHT; break;
    case 'D': seq.type = ANSI_ESC_SEQ_MOVE_CURSOR_LEFT; break;
    case 'E': seq.type = ANSI_ESC_SEQ_MOVE_CURSOR_LINE_START_DOWN; break;
    case 'F': seq.type = ANSI_ESC_SEQ_MOVE_CURSOR_LINE_START_UP; break;
    case 'G': seq.type = ANSI_ESC_SEQ_MOVE_CURSOR_COLUMN; break;
    case 'H': seq.type = seq.payload.raw._1 == INT_MAX ? ANSI_ESC_SEQ_MOVE_CURSOR_HOME : ANSI_ESC_SEQ_MOVE_CURSOR_TO; break;
    case 'f': seq.type = ANSI_ESC_SEQ_MOVE_CURSOR_TO; break;
    case 'M': seq.type = ANSI_ESC_SEQ_MOVE_CURSOR_LINE_UP; break;
    case '7':
    case 's': seq.type = ANSI_ESC_SEQ_SAVE_CURSOR_POSITION; break;
    case '8':
    case 'u': seq.type = ANSI_ESC_SEQ_RESTORE_CURSOR_POSITION; break;

    case 'J': {
      switch (seq.payload.raw._1) {
        case INT_MAX:
        case 0: seq.type = ANSI_ESC_SEQ_ERASE_FROM_CURSOR_TO_END; break;
        case 1: seq.type = ANSI_ESC_SEQ_ERASE_FROM_CURSOR_TO_START; break;
        case 2: seq.type = ANSI_ESC_SEQ_ERASE_ENTIRE_SCREEN; break;
        case 3: seq.type = ANSI_ESC_SEQ_ERASE_SAVED_LINES; break;
        default:
          break;
      }

      break;
    }

    case 'K': {
      switch (seq.payload.raw._1) {
        case INT_MAX:
        case 0: seq.type = ANSI_ESC_SEQ_ERASE_FROM_CURSOR_TO_LINE_END; break;
        case 1: seq.type = ANSI_ESC_SEQ_ERASE_FROM_CURSOR_TO_LINE_START; break;
        case 2: seq.type = ANSI_ESC_SEQ_ERASE_LINE; break;
        default:
          break;
      }

      break;
    }

    case 'm': seq.type = ANSI_ESC_SEQ_SET_STYLE; break;

    default:
      seq.type = ANSI_ESC_SEQ_INVALID;
      break;
  }

  if (seq.type != ANSI_ESC_SEQ_INVALID) {
    handler(&seq, ctx);
  }

  return &str[index+1];
}
