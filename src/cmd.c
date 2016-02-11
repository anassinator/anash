#include "cmd.h"


cmd_t* getcmd() {
  // Initialize cmd.
  cmd_t* cmd = (cmd_t*) malloc(sizeof(cmd_t));
  cmd->ok = 1;

  // Get line from STDIN.
  char* line = NULL;
  size_t linecap = 0;
  int length = getline(&line, &linecap, stdin);
  if (length <= 0) {
    exit(-1);
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

  // Set last element to null pointer.
  cmd->args[arg_count] = NULL;

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
      break;
    }
  }

  // Free structure.
  free(cmd);
}

int executecmd(cmd_t* cmd) {
  if (!cmd->args[0]) {
    // Somebody just pressed enter...
    return 0;
  } else if (!cmd->ok) {
    // Command isn't safe to run.
    return -1;
  }

  // Fork and execute.
  pid_t pid = fork();
  if (pid == 0) {
    // Child process executes command.
    int code = execvp(cmd->args[0], cmd->args);

    // Exit with error code.
    exit(code);
  } else if (cmd->bg) {
    // TODO: Set up job here.

    // Exit code will come later.
    return 0;
  } else {
    // Wait for child process to exit.
    int status;
    pid_t child_pid;
    while ((child_pid = waitpid(-1, &status, WNOHANG)) != -1) {
      if (child_pid == pid) {
        // Found foreground child.
        break;
      }

      if (WIFEXITED(status)) {
        // TODO: Notify a background job is complete.
      }
    }

    // Get exit code.
    int exit_code = WEXITSTATUS(status);
    switch (exit_code) {
      case 0:
        // All is good.
        return 0;
      case 255:
        // Command not found, do not keep in history.
        // TODO: Find better way of determining command was not found.
        cmd->ok = 0;
        fprintf(stderr, "%s: command not found: %s\n", SHELL, cmd->args[0]);
        return 127;
      default:
        // Other error code.
        return exit_code;
    }
  }
}
