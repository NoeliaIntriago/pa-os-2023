#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int main(int argc, char **argv) {
  FILE* source;
  FILE* dest;
  BMP_Image* image = NULL;

  if (argc != 3) {
    printError(ARGUMENT_ERROR);
    exit(EXIT_FAILURE);
  }
  
  if((source = fopen(argv[1], "rb")) == NULL) {
    printError(FILE_ERROR);
    exit(EXIT_FAILURE);
  }
  if((dest = fopen(argv[2], "wb")) == NULL) {
    printError(FILE_ERROR);
    exit(EXIT_FAILURE);
  } 

  image = readImage(source);
  if (image == NULL) {
    printError(VALID_ERROR);
    freeImage(image);
    fclose(source);
    return EXIT_FAILURE;
  }

  freeImage(image);
  fclose(source);
  fclose(dest);

  exit(EXIT_SUCCESS);
}
