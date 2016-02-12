#ifndef __CMD_H
#define __CMD_H

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

// Job type.
typedef struct job_t {
  int index;
  pid_t pid;
  cmd_t* cmd;
} job_t;

// Job linked list type.
typedef struct job_list_t {
  job_t* root;
  struct job_list_t* next;
} job_list_t;

#include "builtins.h"

// Command prototypes.
cmd_t* getcmd();
void freecmd(cmd_t*);
int waitfor(pid_t, job_list_t*);
int executecmd(cmd_t*, job_list_t*);

// Job prototypes.
job_list_t* create_job_list();
void add_job(job_list_t*, job_t*);
job_t* get_job(job_list_t*, pid_t);

#endif
