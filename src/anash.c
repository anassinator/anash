#include "anash.h"


int main() {
  int last_exit_code = 0;

  while (1) {
    // Print prompt.
    if (last_exit_code) {
      printf("%s%s%s", ANSI_COLOR_RED, PS1, ANSI_COLOR_RESET);
      last_exit_code = 0;
    } else {
      printf(PS1);
    }

    // Get command.
    cmd_t* cmd = getcmd();

    // Execute command.
    last_exit_code = executecmd(cmd);

    // Free memory.
    freecmd(cmd);
  }


  return 0;
}
