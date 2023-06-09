#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>

#define SIZE 4096

int main(int argc, char *argv[]) 
{
    if(argc < 2) {
        printf("Usage: ./time <command>\n");
        return 1;
    }

    const char* shmm_name = "MAIN";

    int shmm_fd;
    struct timeval *start_time;
    void* ptr;

    shmm_fd = shm_open(shmm_name, O_CREAT | O_RDWR, 0666);
    if (shmm_fd < 0) {
        printf("ERROR: Failed to open shared memory file descriptor (shm_open)\n");
        return 1;
    }

    ftruncate(shmm_fd, SIZE);
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shmm_fd, 0);
    pid_t pid = fork();
    if(pid < 0) {
        printf("ERROR: Failed to create child process (fork)\n");
        return 1;
    } else if (pid == 0) {
        gettimeofday((struct timeval *)ptr, NULL);
        execvp(argv[1], &argv[1]);
    } else {
        int status;
        waitpid(pid, &status, 0);

        struct timeval end_time;
        gettimeofday(&end_time, NULL);
    }

    return 0;
}