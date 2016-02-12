#include "builtins.h"

int is_builtin(cmd_t* cmd) {
  if (strcmp(cmd->args[0], "jobs") == 0) {
    return 1;
  }

  if (strcmp(cmd->args[0], "history") == 0) {
    return 1;
  }

  // No.
  return 0;
}


int execute_builtin(cmd_t* cmd, job_list_t* jobs, history_t* hist) {
  // List jobs.
  if (strcmp(cmd->args[0], "jobs") == 0) {
    return builtin_jobs(jobs);
  }

  // List history.
  if (strcmp(cmd->args[0], "history") == 0) {
    return builtin_history(hist);
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
    for (int i = 0; i < NUM_ARGS; i++) {
      if (curr_job->cmd->args[i] != NULL) {
        printf("%s ", curr_job->cmd->args[i]);
      } else {
        printf("\n");
        break;
      }
    }

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
      for (int j = 0; j < NUM_ARGS; j++) {
        if (cmd->args[j] != NULL) {
          printf("%s ", cmd->args[j]);
        } else {
          if (cmd->bg) {
            printf("&");
          }
          printf("\n");
          break;
        }
      }
    } else {
      break;
    }
  }

  return 0;
}
