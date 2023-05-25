#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAX 1024
int main (int argc, char* argv[]) {
    char line[MAX];
    int file;

    if (argc > 2) {
        printf("Use: mycat namefile.\n");
        return 0;
    }

    if (argc == 1) {
        while (fgets(line, sizeof(line), stdin)) {
            printf("%s", line);
        }
    } else {
        char* filename = argv[1];
        char* output = (char*) calloc(MAX, sizeof(char));
        int fd = open(filename, O_RDONLY);

        if (fd == -1) {
            fprintf(stderr, "No such file or directory\n");
            return 1;
        }
        
        
    }
    
    return 0;
}