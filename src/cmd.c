#include "cmd.h"


void printcmd(cmd_t* cmd) {
  if (!cmd) {
    return;
  }
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
}


cmd_t* getcmd() {
  // Initialize cmd.
  cmd_t* cmd = (cmd_t*) malloc(sizeof(cmd_t));
  cmd->ok = 1;

  // Get line from STDIN.
  char* line = NULL;
  size_t linecap = 0;
  ssize_t length = getline(&line, &linecap, stdin);
  if (length == -1) {
    // Handle EOF.
    exit(0);
  }

  // Check if background is specified.
  char* loc;
  if ((loc = index(line, '&')) != NULL) {
    cmd->bg = 1;
    *loc = ' ';
  } else {
    cmd->bg = 0;
  }

  // Tokenize.
  char* token;
  int arg_count = 0;
  char* start_of_line = line;
  while ((token = strsep(&line, " \t\n")) != NULL) {
    // Set null character to token.
    for (int j = 0; j < (int) strlen(token); j++) {
      if (token[j] <= 32) {
        token[j] = '\0';
      }
    }

    // Parse argument.
    if (strlen(token) > 0) {
      if (arg_count >= NUM_ARGS - 1) {
        // Too many arguments to hold along with null pointer.
        cmd->ok = 0;
        print_error("too many arguments");
        return cmd;
      } else {
        // Treat every token as a CLI argument.
        cmd->args[arg_count] = (char*) malloc(strlen(token) + 1);
        strcpy(cmd->args[arg_count], token);
        arg_count++;
      }
    }
  }

  // Set last element to null pointer as well as all of the following.
  for (int i = arg_count; i < NUM_ARGS; i++) {
    cmd->args[i] = NULL;
  }

  // Free line from memory.
  free(start_of_line);

  return cmd;
}


void freecmd(cmd_t* cmd) {
  // Free all arguments from memory.
  for (int i = 0; i < NUM_ARGS; i++) {
    if (cmd->args[i]) {
      free(cmd->args[i]);
    } else {
      // break;
    }
  }

  // Free structure.
  free(cmd);
}


cmd_t* copycmd(cmd_t* old_cmd) {
  // Deep copy old command.
  cmd_t* new_cmd = (cmd_t*) malloc(sizeof(cmd_t));
  new_cmd->ok = old_cmd->ok;
  new_cmd->bg = old_cmd->bg;
  for (int i = 0; i < NUM_ARGS; i++) {
    if (old_cmd->args[i]) {
      new_cmd->args[i] = (char*) malloc(strlen(old_cmd->args[i]) + 1);
      strcpy(new_cmd->args[i], old_cmd->args[i]);
    } else {
      new_cmd->args[i] = NULL;
    }
  }
  return new_cmd;
}


int waitfor(pid_t pid, job_list_t* jobs) {
  // Wait for child process to exit.
  int status;
  pid_t child_pid;
  while ((child_pid = waitpid(-1, &status, WNOHANG)) != -1) {
    if (child_pid == pid) {
      // Found foreground child.
      break;
    }

    // TODO: Deal with suspended, aborted, etc.
    if (WIFEXITED(status)) {
      // TODO: Notify as soon as process is complete instead of on synchronous
      // command execution.

      // Get corresponding job.
      job_t* job = get_job(jobs, child_pid);
      if (job) {
        // Get exit status.
        char exit_status[10];
        int exit_code = WEXITSTATUS(status);
        if (exit_code == 0) {
          sprintf(exit_status, "done");
        } else {
          sprintf(exit_status, "exit %d", exit_code);
        }

        // Notify completion.
        printf("[%d]  + %d %s\t", job->index, job->pid, exit_status);
        printcmd(job->cmd);

        // Free memory allocated to job.
        free(job);
      }
    }
  }

  // Return status.
  return status;
}


int executecmd(cmd_t* cmd, job_list_t* jobs, history_t* hist) {
  if (!cmd->args[0]) {
    // Somebody just pressed enter...
    return 0;
  } else if (!cmd->ok) {
    // Command isn't safe to run.
    return -1;
  }

  if (is_builtin(cmd)) {
    int exit_code = execute_builtin(cmd, jobs, hist);
    return exit_code;
  }

  // Fork and execute.
  pid_t pid = fork();
  if (pid == 0) {
    // Child process executes command.
    if (execvp(cmd->args[0], cmd->args) == -1) {
      // Command was not found, so set exit code accordingly.
      fprintf(stderr, "%s: command not found: %s\n", SHELL, cmd->args[0]);
      exit(COMMAND_NOT_FOUND);
    }

    // Exit with error code.
    exit(errno);
  } else if (cmd->bg) {
    // Set up job.
    job_t* new_job = (job_t*) malloc(sizeof(job_t));
    new_job->cmd = cmd;
    new_job->pid = pid;

    // Add job to list.
    add_job(jobs, new_job);
    printf("[%d] %d\n", new_job->index, new_job->pid);

    // Add to history, we don't know if it's good yet.
    add_to_history(hist, cmd);

    // Exit code will come later.
    return 0;
  } else {
    // Hang until complete.
    int status = waitfor(pid, jobs);

    // Get exit code.
    int exit_code = WEXITSTATUS(status);
    if (exit_code != 0) {
      // Oops.
      cmd->ok = 0;
    } else {
      // It's good, add to history.
      add_to_history(hist, cmd);
    }

    return exit_code;
  }
}


job_list_t* create_job_list() {
  job_list_t* jobs = (job_list_t*) malloc(sizeof(job_list_t));
  jobs->root = NULL;
  jobs->next = NULL;
  return jobs;
}


void add_job(job_list_t* jobs, job_t* new_job) {
  // Set job index.
  new_job->index = 1;

  // Add job to root if list is empty.
  if (!jobs->root) {
    jobs->root = new_job;
    return;
  }

  // Otherwise, traverse list until a missing index is found.
  job_list_t* prev_job_list = NULL;
  job_list_t* curr_job_list = jobs;
  while (curr_job_list) {
    // Found empty spot.
    if (new_job->index < curr_job_list->root->index) {
      job_list_t* new_node = create_job_list();
      if (prev_job_list) {
        // Add job to the middle of the list.
        new_node->root = new_job;
        new_node->next = curr_job_list;
        prev_job_list->next = new_node;
      } else {
        // Add job to the beginning of the list.
        new_node->root = curr_job_list->root;
        new_node->next = curr_job_list->next;
        curr_job_list->root = new_job;
        curr_job_list->next = new_node;
      }
      return;
    }

    // Increment index.
    new_job->index++;

    // Update pointers.
    prev_job_list = curr_job_list;
    curr_job_list = curr_job_list->next;
  }

  // Add new job to end of list.
  job_list_t* new_node = create_job_list();
  new_node->root = new_job;
  if (prev_job_list) {
    prev_job_list->next = new_node;
  } else {
    jobs->next = new_node;
  }
}


job_t* get_job(job_list_t* jobs, pid_t pid) {
  // Not found if empty.
  if (!jobs->root) {
    return NULL;
  }

  // Get current job.
  job_t* curr_job = jobs->root;

  // Extract current job if first in list.
  if (curr_job->pid == pid) {
    if (jobs->next) {
      // Bring next job_list forth.
      job_list_t* old_next = jobs->next;
      jobs->root = jobs->next->root;
      jobs->next = jobs->next->next;
      free(old_next);
    } else {
      // Clear list if now empty.
      jobs->root = NULL;
    }
    return curr_job;
  }

  // Otherwise traverse list until found.
  job_list_t* prev_job_list = NULL;
  job_list_t* curr_job_list = jobs;
  while (curr_job_list) {
    curr_job = curr_job_list->root;

    // Extract job from list and free allocated memory.
    if (curr_job->pid == pid) {
      if (prev_job_list) {
        prev_job_list->next = curr_job_list->next;
      } else {
        jobs->next = curr_job_list->next;
      }
      free(curr_job_list);
      return curr_job;
    }

    // Update pointers.
    prev_job_list = curr_job_list;
    curr_job_list = curr_job_list->next;
  }

  // Not found.
  return NULL;
}


job_t* get_job_by_index(job_list_t* jobs, int index) {
  // Not found if empty.
  if (!jobs->root) {
    return NULL;
  }

  // Get current job.
  job_t* curr_job = jobs->root;

  // Get current job if first in list.
  if (curr_job->index == index) {
    return curr_job;
  }

  // Otherwise traverse list until found.
  job_list_t* curr_job_list = jobs;
  while (curr_job_list->next) {
    curr_job = curr_job_list->root;

    // Get job from list.
    if (curr_job->index == index) {
      return curr_job;
    }

    // Update pointers.
    curr_job_list = curr_job_list->next;
  }

  // Not found.
  return NULL;
}



job_t* get_latest_job(job_list_t* jobs) {
  // Not found if empty.
  if (!jobs->root) {
    return NULL;
  }

  // Go through list.
  job_list_t* curr_job_list = jobs;
  while (curr_job_list->next) {
    curr_job_list = curr_job_list->next;
  }

  return curr_job_list->root;
}


history_t* create_history() {
  history_t* hist = (history_t*) malloc(sizeof(history_t));
  hist->count = 0;
  for (int i = 0; i < MAX_HISTORY; i++) {
    hist->commands[i] = NULL;
  }
  return hist;
}


void add_to_history(history_t* hist, cmd_t* cmd) {
  // Determine index of command in history.
  int index = hist->count % MAX_HISTORY;

  // Add to history.
  if (hist->commands[index]) {
    // Free old commands from memory.
    freecmd(hist->commands[index]);
  }
  hist->commands[index] = cmd;

  // Increment count.
  hist->count++;
}


cmd_t* get_from_history(history_t* hist, uint i) {
  // Bound checking.
  if (i > hist->count) {
    print_error("command %d not found in history", i);
    return NULL;
  }

  // Extract command in question.
  int index = (i % MAX_HISTORY) - 1;
  return hist->commands[index];
}
