#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "output.h"

#define NUM_ARGS 20
#define COMMAND_NOT_FOUND 127

// Command type.
typedef struct cmd_t {
  int ok;
  char* args[NUM_ARGS];
  int bg;
} cmd_t;

// Prototypes.
cmd_t* getcmd();
void freecmd(cmd_t*);
int executecmd(cmd_t*);
