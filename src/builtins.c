#include "builtins.h"


int is_builtin(cmd_t* cmd) {
  if (strcmp(cmd->args[0], "jobs") == 0) {
    return 1;
  }

  if (strcmp(cmd->args[0], "history") == 0) {
    return 1;
  }

  if (strcmp(cmd->args[0], "exit") == 0) {
    return 1;
  }

  char* endptr = NULL;
  long num = strtol(cmd->args[0], &endptr, 10);
  if (*endptr == '\0' && !(num == 0 && errno == EINVAL)) {
    return 1;
  }

  // No.
  return 0;
}


int execute_builtin(cmd_t* cmd, job_list_t* jobs, history_t* hist) {
  // List jobs.
  if (strcmp(cmd->args[0], "jobs") == 0) {
    add_to_history(hist, cmd);
    return builtin_jobs(jobs);
  }

  // List history.
  if (strcmp(cmd->args[0], "history") == 0) {
    int exit_code = builtin_history(hist);
    add_to_history(hist, cmd);
    return exit_code;
  }

  if (strcmp(cmd->args[0], "exit") == 0) {
    return builtin_exit();
  }

  // Execute from history.
  char* endptr = NULL;
  long num = strtol(cmd->args[0], &endptr, 10);
  if (*endptr == '\0' && !(num == 0 && errno == EINVAL)) {
    // Do not save to history to avoid confusion.
    int exit_code = builtin_exec_from_history(cmd, jobs, hist);
    freecmd(cmd);
    return exit_code;
  }

  // Command not found.
  return COMMAND_NOT_FOUND;
}


int builtin_jobs(job_list_t* jobs) {
  // List running jobs.
  job_list_t* curr_job_list = jobs;
  while (curr_job_list && curr_job_list->root) {
    // Print current job.
    job_t* curr_job = curr_job_list->root;
    printf("[%d]  + running\t", curr_job->index);
    printcmd(curr_job->cmd);

    // Update pointer.
    if (curr_job_list->next) {
      curr_job_list = curr_job_list->next;
    } else {
      break;
    }
  }

  // Return success.
  return 0;
}


int builtin_history(history_t* hist) {
  // Compute starting index.
  int start = hist->count > MAX_HISTORY ? hist->count - MAX_HISTORY : 0;

  // List them all.
  for (int i = start; i < hist->count; i++) {
    int index = i % MAX_HISTORY;
    cmd_t* cmd = hist->commands[index];
    if (cmd) {
      printf("%5d  ", i + 1);
      printcmd(cmd);
    } else {
      break;
    }
  }

  // Return success.
  return 0;
}


int builtin_exec_from_history(cmd_t* cmd, job_list_t* jobs, history_t* hist) {
  int index = (int) strtol(cmd->args[0], NULL, 10);
  cmd_t* past_cmd = get_from_history(hist, index);
  if (past_cmd) {
    printcmd(past_cmd);
    return executecmd(past_cmd, jobs, hist);
  }
  return COMMAND_NOT_FOUND;
}


int builtin_exit() {
  exit(0);
  return 0;
}
