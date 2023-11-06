#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>

#define SIZE sizeof(struct timeval)

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage >> %s <command>\n", argv[0]);
        exit(1);
    }

    const char *name = "MAIN"; // Shared memory object name
    int shm_fd;                // File descriptor for shared memory object
    struct timeval *start_time;
    pid_t pid;
    int status;

    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0)
    {
        perror("ERROR >> Failed to create shared memory segment (shm_open)\n");
        exit(1);
    }

    ftruncate(shm_fd, SIZE);
    start_time = (struct timeval *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (start_time == MAP_FAILED)
    {
        perror("ERROR >> Failed to attach shared memory (mmap)\n");
        exit(1);
    }

    pid = fork();
    if (pid < 0)
    {
        // Error while creating child
        perror("ERROR >> Failed to create child process (fork)\n");
        exit(1);
    }
    else if (pid == 0)
    {
        // Child process
        gettimeofday(start_time, NULL);
        printf("Start time: %ld seconds, %ld microseconds\n", start_time->tv_sec, start_time->tv_usec);

        execvp(argv[1], &argv[1]);
        perror("ERROR >> Failed to run execvp (execvp)\n");
        exit(status);
    }
    else
    {
        // Parent process
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            struct timeval end_time;
            gettimeofday(&end_time, NULL);

            long elapsed_sec = end_time.tv_sec - start_time->tv_sec;
            long elapsed_usec = end_time.tv_usec - start_time->tv_usec;
            double elapsed_time = elapsed_sec + elapsed_usec / 1000000.0;
            printf("End time: %ld seconds, %ld microseconds\n", end_time.tv_sec, end_time.tv_usec);
            printf("Elapsed time: %.6f seconds\n", elapsed_time);
        }
        else
        {
            perror("ERROR >> Failed to terminate child\n");
            exit(1);
        }
        if (munmap(start_time, SIZE) < 0)
        {
            perror("ERROR >> Failed to release shared memory (munmap)\n");
            exit(1);
        }
        if (shm_unlink(name))
        {
            perror("ERROR >> Failed to delete shared memory (shm_unlink)\n");
            exit(1);
        }
    }

    return 0;
}