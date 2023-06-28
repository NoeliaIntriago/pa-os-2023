#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> //Prototypes for many call systems

int main(int argc, char **argv) {
  pid_t pid = fork();
  int status;

  if (pid == -1) {
    printf("ERROR: Proceso hijo #1 no pudo ser creado\n");
    exit(1);
  } else if (pid != 0) {
    printf("Soy el padre con id: %d!\n", getpid());
    wait(&status);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
      pid_t pid2 = fork();
      int status2;

      if (pid2 == -1) {
        printf("ERROR: Proceso hijo #2 no pudo ser creado\n");
        exit(1);
      } else if (pid2 != 0) {
        printf("Soy el padre con id: %d!\n", getpid());
        wait(&status2);

        if (WIFEXITED(status2) && WEXITSTATUS(status2) == 0) {
          printf("OK: Procesos terminados con éxito!\n");
          exit(status);
        } else {
          printf("ERROR: Proceso hijo #2 terminado con errores\n");
          exit(status2);
        }
      } else {
        printf("Soy el hijo #2 con id: %d!\n", getpid());
        char *argv_list[] = {"/usr/bin/wc", argv[1], NULL};
        execvp(argv_list[0], argv_list);
        exit(status);
      }
    } else {
      printf("ERROR: Proceso hijo #1 terminado con errores\n");
      exit(status);
    }
  } else {
    printf("Soy el hijo #1 con id: %d!\n", getpid());
    char *argv_list[] = {"/bin/cat", argv[1], NULL};
    execvp(argv_list[0], argv_list);
    exit(status);
  }
  return 0;
}