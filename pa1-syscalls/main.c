#include <stdio.h>

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
        printf("Reading a file\n");
    }
    
    return 0;
}