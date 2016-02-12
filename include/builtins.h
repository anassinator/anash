#ifndef __BUILTINS_H
#define __BUILTINS_H

#include <stdlib.h>

#include "cmd.h"

// Prototypes.
int is_builtin(cmd_t*);
int execute_builtin(cmd_t*, job_list_t*);

// Builtins prototypes.
int builtin_jobs(job_list_t*);

#endif
