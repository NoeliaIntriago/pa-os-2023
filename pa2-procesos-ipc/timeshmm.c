#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define SIZE 4096

int main() 
{
    const char* shmm_name = "MAIN";

    int shmm_fd;
    void* ptr;

    shmm_fd = shm_open(shmm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(shmm_fd, SIZE);

    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shmm_fd, 0);
    sprintf(ptr, "%s", "HOLA");

    return 0;
}