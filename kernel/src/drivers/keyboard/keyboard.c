#include <drivers/keyboard/keyboard.h>
#include <util/util.h>

int key_to_char(key_t key) {
  switch (key.scancode) {
    case KEY_SCANCODE_NONE:               return 0;
    case KEY_SCANCODE_ESC:                return '\033';
    case KEY_SCANCODE_ENTER:              return '\n';
    case KEY_SCANCODE_SPACE:              return ' ';
    case KEY_SCANCODE_BACKSPACE:          return '\b';
    case KEY_SCANCODE_SHIFT:              return 0;
    case KEY_SCANCODE_ALT:                return 0;
    case KEY_SCANCODE_CTRL:               return 0;
    case KEY_SCANCODE_TAB:                return '\t';
    case KEY_SCANCODE_HTAB:               return '\v';
    case KEY_SCANCODE_UP_ARROW:           return 0;
    case KEY_SCANCODE_DOWN_ARROW:         return 0;
    case KEY_SCANCODE_LEFT_ARROW:         return 0;
    case KEY_SCANCODE_RIGHT_ARROW:        return 0;
    case KEY_SCANCODE_A:                  return key.flags & KEY_FLAG_SHIFT ? 'A' : 'a';
    case KEY_SCANCODE_B:                  return key.flags & KEY_FLAG_SHIFT ? 'B' : 'b';
    case KEY_SCANCODE_C:                  return key.flags & KEY_FLAG_SHIFT ? 'C' : 'c';
    case KEY_SCANCODE_D:                  return key.flags & KEY_FLAG_SHIFT ? 'D' : 'd';
    case KEY_SCANCODE_E:                  return key.flags & KEY_FLAG_SHIFT ? 'E' : 'e';
    case KEY_SCANCODE_F:                  return key.flags & KEY_FLAG_SHIFT ? 'F' : 'f';
    case KEY_SCANCODE_G:                  return key.flags & KEY_FLAG_SHIFT ? 'G' : 'g';
    case KEY_SCANCODE_H:                  return key.flags & KEY_FLAG_SHIFT ? 'H' : 'h';
    case KEY_SCANCODE_I:                  return key.flags & KEY_FLAG_SHIFT ? 'I' : 'i';
    case KEY_SCANCODE_J:                  return key.flags & KEY_FLAG_SHIFT ? 'J' : 'j';
    case KEY_SCANCODE_K:                  return key.flags & KEY_FLAG_SHIFT ? 'K' : 'k';
    case KEY_SCANCODE_L:                  return key.flags & KEY_FLAG_SHIFT ? 'L' : 'l';
    case KEY_SCANCODE_M:                  return key.flags & KEY_FLAG_SHIFT ? 'M' : 'm';
    case KEY_SCANCODE_N:                  return key.flags & KEY_FLAG_SHIFT ? 'N' : 'n';
    case KEY_SCANCODE_O:                  return key.flags & KEY_FLAG_SHIFT ? 'O' : 'o';
    case KEY_SCANCODE_P:                  return key.flags & KEY_FLAG_SHIFT ? 'P' : 'p';
    case KEY_SCANCODE_Q:                  return key.flags & KEY_FLAG_SHIFT ? 'Q' : 'q';
    case KEY_SCANCODE_R:                  return key.flags & KEY_FLAG_SHIFT ? 'R' : 'r';
    case KEY_SCANCODE_S:                  return key.flags & KEY_FLAG_SHIFT ? 'S' : 's';
    case KEY_SCANCODE_T:                  return key.flags & KEY_FLAG_SHIFT ? 'T' : 't';
    case KEY_SCANCODE_U:                  return key.flags & KEY_FLAG_SHIFT ? 'U' : 'u';
    case KEY_SCANCODE_V:                  return key.flags & KEY_FLAG_SHIFT ? 'V' : 'v';
    case KEY_SCANCODE_W:                  return key.flags & KEY_FLAG_SHIFT ? 'W' : 'w';
    case KEY_SCANCODE_X:                  return key.flags & KEY_FLAG_SHIFT ? 'X' : 'x';
    case KEY_SCANCODE_Y:                  return key.flags & KEY_FLAG_SHIFT ? 'Y' : 'y';
    case KEY_SCANCODE_Z:                  return key.flags & KEY_FLAG_SHIFT ? 'Z' : 'z';
    case KEY_SCANCODE_0:                  return '0';
    case KEY_SCANCODE_1:                  return '1';
    case KEY_SCANCODE_2:                  return '2';
    case KEY_SCANCODE_3:                  return '3';
    case KEY_SCANCODE_4:                  return '4';
    case KEY_SCANCODE_5:                  return '5';
    case KEY_SCANCODE_6:                  return '6';
    case KEY_SCANCODE_7:                  return '7';
    case KEY_SCANCODE_8:                  return '8';
    case KEY_SCANCODE_9:                  return '9';
    case KEY_SCANCODE_SLASH:              return '/';
    case KEY_SCANCODE_BACKSLASH:          return '\\';
    case KEY_SCANCODE_VBAR:               return '|';
    case KEY_SCANCODE_HYPHEN:             return '-';
    case KEY_SCANCODE_UNDERSCORE:         return '_';
    case KEY_SCANCODE_EQUALS:             return '=';
    case KEY_SCANCODE_PLUS:               return '+';
    case KEY_SCANCODE_STAR:               return '*';
    case KEY_SCANCODE_COLON:              return ':';
    case KEY_SCANCODE_SEMICOLON:          return ';';
    case KEY_SCANCODE_TILDA:              return '~';
    case KEY_SCANCODE_BACKTICK:           return '`';
    case KEY_SCANCODE_DOLLAR:             return '$';
    case KEY_SCANCODE_PERCENT:            return '%';
    case KEY_SCANCODE_AMP:                return '&';
    case KEY_SCANCODE_AT:                 return '@';
    case KEY_SCANCODE_LEFT_PAREN:         return '(';
    case KEY_SCANCODE_RIGHT_PAREN:        return ')';
    case KEY_SCANCODE_LEFT_BRACE:         return '{';
    case KEY_SCANCODE_RIGHT_BRACE:        return '}';
    case KEY_SCANCODE_LEFT_SQUARE_BRACE:  return '[';
    case KEY_SCANCODE_RIGHT_SQUARE_BRACE: return ']';
    case KEY_SCANCODE_EXCLAMATION_POINT:  return '!';
    case KEY_SCANCODE_QUESTION_MARK:      return '?';
    case KEY_SCANCODE_DOT:                return '.';
    case KEY_SCANCODE_COMMA:              return ',';
    case KEY_SCANCODE_LESS_THEN:          return '<';
    case KEY_SCANCODE_MORE_THEN:          return '>';
    case KEY_SCANCODE_HASH:               return '#';
    case KEY_SCANCODE_SINGLE_QUOTES:      return '\'';
    case KEY_SCANCODE_QUOTES:             return '"';
    case KEY_SCANCODE_CARET:              return '^';
    default:                              return 0;
  }
}

key_t key_get() {
  return key_get_port();
}

key_t key_wait() {
  return key_wait_port();
}

// __WEAK key_t key_get_port() {
//   return (key_t){KEY_SCANCODE_NONE, KEY_FLAG_NONE};
// }
//
// __WEAK key_t key_wait_port() {
//   return (key_t){KEY_SCANCODE_NONE, KEY_FLAG_NONE};
// }
