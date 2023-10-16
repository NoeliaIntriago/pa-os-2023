#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX 1024
int main (int argc, char* argv[]) {
    char line[MAX];
    int output;

    if (argc > 2) {
        printf("Use: mycat [namefile].\n");
        return 0;
    }

    if (argc == 1) {
        while((output = read(STDIN_FILENO, line, MAX)) > 0){
            write(STDOUT_FILENO, line, output);
        }
    } else {
        int file;
        char* filename = argv[1];
        file = open(filename, O_RDONLY);

        if (file == -1) {
            printf("ERROR: No such file or directory\n");
            return 1;
        }
        
        while((output = read(file, line, MAX)) > 0){
            write(1, line, output);
        }

        close(file);
    }
    return 0;
}