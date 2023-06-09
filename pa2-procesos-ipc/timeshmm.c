#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>

#define SIZE 4096

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Usage: %s <command>\n", argv[0]);
        return 1;
    }

    const char* name = "MAIN";  // Shared memory object name
    int shm_fd;                 // File descriptor for shared memory object
    void *ptr;                  // Pointer to attach shared memory
    struct timeval start_time;
    pid_t pid;

    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        return 1;
    }

    ftruncate(shm_fd, SIZE);
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED){
        perror("mmap");
        return 1;
    }

    gettimeofday(&start_time, NULL);
    pid = fork();
    if (pid < 0) {
        // Error while creating child
        printf("ERROR: Failed to create child process\n");
        return 1;
    } else if (pid == 0) {
        // Child process
        execvp(argv[0], argv);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);

        struct timeval end_time;
        gettimeofday(&end_time, NULL);


    }
}