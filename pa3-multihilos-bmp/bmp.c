#include <stdlib.h>
#include <stdio.h>

#include "bmp.h"
/* USE THIS FUNCTION TO PRINT ERROR MESSAGES
   DO NOT MODIFY THIS FUNCTION
*/
void printError(int error) {
  switch(error){
  case ARGUMENT_ERROR:
    printf("Usage:ex5 <source> <destination>\n");
    break;
  case FILE_ERROR:
    printf("Unable to open file!\n");
    break;
  case MEMORY_ERROR:
    printf("Unable to allocate memory!\n");
    break;
  case VALID_ERROR:
    printf("BMP file not valid!\n");
    break;
  default:
    break;
  }
}

/* The input argument is the source file pointer. The function will first construct a BMP_Image image by allocating memory to it.
 * Then the function read the header from source image to the image's header.
 * Compute data size, width, height, and bytes_per_pixel of the image and stores them as image's attributes.
 * Finally, allocate menory for image's data according to the image size.
 * Return image;
*/
BMP_Image* createBMPImage(FILE* fptr) {

  //Allocate memory for BMP_Image*;
  BMP_Image* new_image = malloc(sizeof(BMP_Image));
  if (new_image == NULL) {
    printError(MEMORY_ERROR);
    return NULL;
  }

  //Read the first 54 bytes of the source into the header
  if (fread(&(new_image->header), HEADER_SIZE, 1, fptr) != 1) {
    printError(VALID_ERROR);
    return NULL;
  }

  //Compute data size, width, height, and bytes per pixel
  new_image->norm_height = abs(new_image->header.height_px);
  new_image->bytes_per_pixel = new_image->header.bits_per_pixel / 8;
  new_image->pixels = malloc((new_image->norm_height) * sizeof(Pixel *));

  //Allocate memory for image data
  for (int i = 0; i < new_image->norm_height; i++) {
    new_image->pixels[i] = malloc((new_image->header.width_px) * sizeof(Pixel));
  }

  return new_image;
}

/* The input arguments are the source file pointer, the image data pointer, and the size of image data.
 * The functions reads data from the source into the image data matriz of pixels.
*/
void readImageData(FILE* srcFile, BMP_Image * image, int dataSize) {
  for (int i = 0; i < image->norm_height; i++) {
    for (int j = 0; j < image->header.width_px; j++) {
      if (fread(&image->pixels[i][j], dataSize, 1, srcFile) == 0) {
        exit(FILE_ERROR);
      }
    }
  }
}

/* The input arguments are the pointer of the binary file, and the image data pointer.
 * The functions open the source file and call to CreateBMPImage to load de data image.
*/
BMP_Image* readImage(FILE *srcFile) {
  if (srcFile == NULL) {
    printError(FILE_ERROR);
    return NULL;
  }

  BMP_Image* image = createBMPImage(srcFile);
  if (image == NULL) {
    printError(MEMORY_ERROR);
    return NULL;
  }
  
  return image;
}

/* The input arguments are the destination file name, and BMP_Image pointer.
 * The function write the header and image data into the destination file.
*/
void writeImage(char* destFileName, BMP_Image* dataImage) {
  FILE *fd = fopen(destFileName, "wb");
  if (fd == NULL) {
    printError(FILE_ERROR);
    return;
  }

  fwrite(&(dataImage->header), HEADER_SIZE, 1, fd);

  for (int y = 0; y < dataImage->norm_height; y++) {
    fwrite(dataImage->pixels[y], sizeof(Pixel), dataImage->header.width_px, fd);
  }

  fclose(fd);
  return;
}

/* The input argument is the BMP_Image pointer. The function frees memory of the BMP_Image.
*/
void freeImage(BMP_Image* image) {
  for (int i = 0; i < image->norm_height; i++) {
    free(image->pixels[i]);
  }
  free(image->pixels);

  free(image);
}

/* The functions checks if the source image has a valid format.
 * It returns TRUE if the image is valid, and returns FASLE if the image is not valid.
 * DO NOT MODIFY THIS FUNCTION
*/
int checkBMPValid(BMP_Header* header) {
  // Make sure this is a BMP file
  if (header->type != 0x4d42) {
    return FALSE;
  }
  // Make sure we are getting 24 bits per pixel
  if (header->bits_per_pixel != 24) {
    printf("We're not getting 24 bits per pixel\n");
    return FALSE;
  }
  // Make sure there is only one image plane
  if (header->planes != 1) {
    printf("There isn't just one image\n");
    return FALSE;
  }
  // Make sure there is no compression
  if (header->compression != 0) {
    printf("There is some compression\n");
    return FALSE;
  }
  return TRUE;
}

/* The function prints all information of the BMP_Header.
   DO NOT MODIFY THIS FUNCTION
*/
void printBMPHeader(BMP_Header* header) {
  printf("file type (should be 0x4d42): %x\n", header->type);
  printf("file size: %d\n", header->size);
  printf("offset to image data: %d\n", header->offset);
  printf("header size: %d\n", header->header_size);
  printf("width_px: %d\n", header->width_px);
  printf("height_px: %d\n", header->height_px);
  printf("planes: %d\n", header->planes);
  printf("bits: %d\n", header->bits_per_pixel);
}

/* The function prints information of the BMP_Image.
   DO NOT MODIFY THIS FUNCTION
*/
void printBMPImage(BMP_Image* image) {
  printf("data size is %ld\n", sizeof(image->pixels));
  printf("norm_height size is %d\n", image->norm_height);
  printf("bytes per pixel is %d\n", image->bytes_per_pixel);
}
