
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t run_search(char *arguments[]);

int main(int argc, char *argv[]) {
    char *arguments[] = {"./mfind", argv[1], argv[2]};
    int num_processes = 10;
    // Run the search processes
    for (int i = 0; i < num_processes; i++) {
        run_search(arguments);
    }

    // Wait for all children to finish
    for (int i = 0; i < num_processes; i++) {
        wait(NULL);
    }
}

pid_t run_search(char *arguments[]) {
  pid_t pid;
  pid = fork();
  if ( pid < 0 ) {
    fprintf(stderr, "Forking failed\n");
    perror("fork");
    exit(1);
  } else if ( pid == 0 ) {
    /* Child: run the command */
    if (execvp(arguments[0], arguments) < 0) {
      perror("exec");
      exit(1);
    }
    // Child done
  } else {
    /* Parent: wait for child to finish */
    return pid;
  }
  // should never get here
  exit(1);
}
