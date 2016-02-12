#include "builtins.h"

int is_builtin(cmd_t* cmd) {
  if (strcmp(cmd->args[0], "jobs") == 0) {
    return 1;
  }

  // No.
  return 0;
}


int execute_builtin(cmd_t* cmd, job_list_t* jobs) {
  // List jobs.
  if (strcmp(cmd->args[0], "jobs") == 0) {
    return builtin_jobs(jobs);
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
