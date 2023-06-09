#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>

#define SIZE sizeof(struct timeval)

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Usage: %s <command>\n", argv[0]);
        return 1;
    }

    int fd[2];                  // Array for pipe
    struct timeval start_time;
    pid_t pid;

    if (pipe(fd) < 0) {
        perror("ERROR: Failed to create pipe (pipe)\n");
        exit(1);
    }

    pid = fork();
    int status;
    if (pid < 0) {
        // Error while creating child process
        perror("ERROR: Failed to create child process (fork)\n");
        exit(1);
    } else if (pid == 0) {
        // Child process
        close(fd[0]);
        gettimeofday(&start_time, NULL);
        if(write(fd[1], &start_time, SIZE) < 0) {
            perror("ERROR: Failed to write to the pipe (write)\n");
            exit(status);
        }

        close(fd[1]);
        execvp(argv[1], &argv[1]);
        perror("ERROR: Failed to run execvp (execvp)\n");
        exit(status);
    } else {
        // Parent process
        close(fd[1]);
        waitpid(pid, &status, 0);
        struct timeval start_time_buffer;
        if (read(fd[0], &start_time_buffer, SIZE) < 0) {
            perror("ERROR: Failed to read from the pipe (read)\n");
            exit(status);
        }

        close(fd[0]);
        struct timeval end_time;
        gettimeofday(&end_time, NULL);

        long elapsed_sec = end_time.tv_sec - start_time_buffer.tv_sec;
        long elapsed_usec = end_time.tv_usec - start_time_buffer.tv_usec;
        double elapsed_time = elapsed_sec + elapsed_usec / 1000000.0;
        printf("Elapsed time: %.6f seconds\n", elapsed_time);
    }

    return 0;
}