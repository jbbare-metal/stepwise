#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#define TAG_TRACER "[tracer] "
#define TAG_TRACEE "[tracee] "

static void run_child(char **tracee_argv) {
    printf("argv:%s", tracee_argv[0]);
    fflush(stdout);
    ptrace(PTRACE_TRACEME, 0, NULL, NULL); // Allow parent to trace child prcocess
    execvp(tracee_argv[0], tracee_argv);

    // execvp only returns if it failed.
    perror("execvp");
    _exit(1);
}

static void run_parent(pid_t child_pid) {
     int status;

     // Wait for the initial stop when the child process is replaced
     waitpid(child_pid, &status, 0); 
     printf(TAG_TRACER "child stopped using signal %d\n", WSTOPSIG(status));

     // Resume the tracee
     ptrace(PTRACE_CONT, child_pid, NULL, NULL);
     waitpid(child_pid, &status, 0);

     if (WIFEXITED(status)) {
          printf(TAG_TRACER "child exited with code %d\n", WEXITSTATUS(status));
     } else if(WIFSIGNALED(status)) {
          printf(TAG_TRACER "child was killed by signal %d\n", WTERMSIG(status));
     } else if(WIFSTOPPED(status)) {
          printf(TAG_TRACER "child was stopped by signal %d\n", WSTOPSIG(status));
     }
}

int main(int argc, char **argv) {
     if (argc < 2) {
          fprintf(stderr, "The name of the program is to be attached to be must be specified");

          return 2;
     }

     fflush(stdout); // guard against unflushed content from parent when there is no new line to flush
     pid_t pid = fork();

     if (pid < 0) { // Error forking
          perror("fork");
          return 1;
     } else if (pid == 0) { // Child process block
          run_child(&argv[1]);
     } else { // Parent process block
          run_parent(pid);
     }

     return 0;
}
