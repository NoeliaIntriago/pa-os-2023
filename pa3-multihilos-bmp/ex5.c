#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int main(int argc, char **argv) {
  FILE* source;
  FILE* dest;
  BMP_Image* imageIn = NULL;
  BMP_Image* imageOut = NULL;
  /* Checks for 3 arguments in console */
  if (argc != 3) {
    printError(ARGUMENT_ERROR);
    exit(EXIT_FAILURE);
  }
  
  /* Checks if source file exists */
  if((source = fopen(argv[1], "rb")) == NULL) {
    printError(FILE_ERROR);
    exit(EXIT_FAILURE);
  }

  /* Checks if destiny file is valid */
  if((dest = fopen(argv[2], "wb")) == NULL) {
    printError(FILE_ERROR);
    exit(EXIT_FAILURE);
  } 

  imageIn = malloc(sizeof(BMP_Image));
  imageOut = malloc(sizeof(BMP_Image));
  if (imageIn == NULL || imageOut == NULL) {
    printError(MEMORY_ERROR);
    exit(EXIT_FAILURE);
  }
  
  if (readImage(source, imageIn) != 0){
    printError(MEMORY_ERROR);
    exit(EXIT_FAILURE);
  }

  if(!checkBMPValid(&imageIn->header)) {
    printError(VALID_ERROR);
    return EXIT_FAILURE;
  }

  if(modifyImage(imageIn, imageOut) != 0){
    freeImage(imageIn);
    freeImage(imageOut);

    fclose(source);
    fclose(dest);
  }

  if(!checkBMPValid(&imageOut->header)) {
    printError(VALID_ERROR);
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
