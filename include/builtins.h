#ifndef __BUILTINS_H
#define __BUILTINS_H

#include <pwd.h>
#include <stdlib.h>

#include "cmd.h"

// Prototypes.
int is_builtin(cmd_t*);
int execute_builtin(cmd_t*, job_list_t*, history_t*);

// Builtins prototypes.
int builtin_pwd();
int builtin_exit();
int builtin_cd(cmd_t*);
int builtin_jobs(job_list_t*);
int builtin_history(history_t*);
int builtin_exec_from_history(cmd_t*, job_list_t*, history_t*);

#endif
