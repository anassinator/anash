#include "anash.h"


int main() {
  int last_exit_code = 0;

  while (1) {
    // Print prompt.
    if (last_exit_code) {
      bad_prompt();
      last_exit_code = 0;
    } else {
      prompt();
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
