#include "output.h"


void prompt() {
  if (isatty(fileno(stdin))) {
    // Print only if STDIN is a TTY.
    printf(PS1);
  }
}


void bad_prompt() {
  if (isatty(fileno(stdin))) {
    // Print only if STDIN is a TTY.
    printf("%s%s%s", ANSI_COLOR_RED, PS1, ANSI_COLOR_RESET);
  }
}


void print_error(const char *format, ...) {
  // Get N arguments.
  va_list args;
  va_start(args, format);

  // Print error to STDERR.
  fprintf(stderr, "%s: ", SHELL);
  vfprintf(stderr, format, args);
  fprintf(stderr, "\n");

  // Free memory.
  va_end(args);
}
