#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include "filter.h"

#define NUMTHREADS 2

int main(int argc, char **argv)
{
  FILE *source;
  FILE *dest;
  BMP_Image *imageIn = NULL;
  BMP_Image *imageOut = NULL;

  /* Checks for 3 arguments in console */
  if (argc != 3)
  {
    printError(ARGUMENT_ERROR);
    exit(EXIT_FAILURE);
  }

  /* Checks if source file exists */
  if ((source = fopen(argv[1], "rb")) == NULL)
  {
    printError(FILE_ERROR);
    exit(EXIT_FAILURE);
  }

  /* Checks if destiny file is valid */
  if ((dest = fopen(argv[2], "wb")) == NULL)
  {
    printError(FILE_ERROR);
    exit(EXIT_FAILURE);
  }

  imageIn = malloc(sizeof(BMP_Image));
  imageOut = malloc(sizeof(BMP_Image));
  if (imageIn == NULL || imageOut == NULL)
  {
    printError(MEMORY_ERROR);
    exit(EXIT_FAILURE);
  }

  readImage(source, imageIn);
  

  if (!checkBMPValid(&imageIn->header))
  {
    printError(VALID_ERROR);
    return EXIT_FAILURE;
  }

  modifyImage(imageIn, imageOut);

  if (!checkBMPValid(&imageOut->header))
  {
    printError(VALID_ERROR);
    exit(EXIT_FAILURE);
  }

  applyParallel(imageIn, imageOut, NUMTHREADS);
  writeImage(dest, imageOut);

  freeImage(imageIn);
  freeImage(imageOut);

  fclose(source);
  fclose(dest);

  exit(EXIT_SUCCESS);
}
