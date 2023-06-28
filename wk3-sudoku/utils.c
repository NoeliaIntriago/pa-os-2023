#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/* loadBoard(nameFile, board) carga el archivo de texto de nombre nFile en la matriz board[SIZE][SIZE]*/
int loadBoard(char * nFile, int board[SIZE][SIZE])
{
  char * line = NULL;
  size_t len = 0;
  ssize_t nread;
  FILE *fp = fopen(nFile, "r");
  if (fp == NULL) {
    perror("Error in fopen line 17");
    return -1;
  }
  int idx=0;
  while ((nread = getline(&line, &len, fp)) != -1) {
    fwrite(line, nread, 1, stdout);

    char *token = strtok(line, " ");
    int row=0 , col=0;
    while(token != NULL) {
      // Sólo en la primera pasamos la cadena; en las siguientes pasamos NULL
      row = idx / SIZE;
      col = idx % SIZE;
      board[row][col] = atoi(token);
      token = strtok(NULL, " ");
      idx ++;
    }
  }

  free(line);
  fclose(fp);
  return 0;
}