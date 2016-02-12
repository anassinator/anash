#include "output.h"


void prompt() {
  printf(PS1);
}


void bad_prompt() {
  printf("%s%s%s", ANSI_COLOR_RED, PS1, ANSI_COLOR_RESET);
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
