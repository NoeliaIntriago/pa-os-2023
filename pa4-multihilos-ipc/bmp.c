#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bmp.h"

/* USE THIS FUNCTION TO PRINT ERROR MESSAGES
   DO NOT MODIFY THIS FUNCTION
*/
void printError(int error){
  switch(error){
  case ARGUMENT_ERROR:
    printf("Usage: ex5 <n.threads>\n");
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
BMP_Image* createBMPImage(FILE* fptr, BMP_Image *dataImage) {

  //Allocate memory for BMP_Image*;
  if(memset(dataImage, 0, sizeof(BMP_Image)) == NULL) return NULL;

  //Read the first 54 bytes of the source into the header
  if(fread(&dataImage->header, HEADER_SIZE, 1, fptr) == 0) return NULL;

  //Compute data size, width, height, and bytes per pixel;
  int width_px = dataImage->header.width_px;
  int height_px = abs(dataImage->header.height_px);

  dataImage->bytes_per_pixel = dataImage->header.bits_per_pixel / BITS; 
  dataImage->norm_height = height_px;
  
  //Allocate memory for image data
  dataImage->pixels = malloc(height_px * sizeof(Pixel *));
  for(int i = 0; i < height_px; i++) dataImage->pixels[i] = malloc(width_px * sizeof(Pixel));

  return dataImage;
}

/* The input arguments are the source file pointer, the image data pointer, and the size of image data.
 * The functions reads data from the source into the image data matriz of pixels.
*/
int readImageData(FILE* srcFile, BMP_Image * image, int dataSize) {

  for(int i = 0; i< image->norm_height; i++){
    for(int j = 0; j < image->header.width_px; j++){
      if(fread(&image->pixels[i][j], dataSize, 1, srcFile) == 0){
        fprintf(stderr, "Pixel (%d,%d) no se pudo leer desde el archivo\n", i, j);
        return 1;
      }
    }
  }

  return 0;

}

/* The input arguments are the pointer of the binary file, and the image data pointer.
 * The functions open the source file and call to CreateBMPImage to load de data image.
*/
int readImage(FILE *srcFile, BMP_Image * dataImage) {

  BMP_Image *tmp = createBMPImage(srcFile, dataImage);
  if(tmp == NULL) return 1;

  return readImageData(srcFile, dataImage, dataImage->bytes_per_pixel);

}

/* The input arguments are the destination file name, and BMP_Image pointer.
 * The function write the header and image data into the destination file.
*/
int writeImage(FILE* destFileName, BMP_Image* dataImage) {

  // Header
  if(fwrite(&dataImage->header, HEADER_SIZE, 1, destFileName) == 0) return 1;
  // fwrite(dataImage->pixels, dataImage->header.imagesize, 1, destFileName);

  // Data
  for(int i = 0; i< dataImage->norm_height; i++){
    if(fwrite(dataImage->pixels[i], sizeof(Pixel), dataImage->header.width_px, destFileName) == 0){
      fprintf(stderr, "Fila (%d) no se pudo escribir en la imagen de salida\n", i);
    }
  }

  return 0;

}

int modify_new_image(BMP_Image *image, BMP_Image *new_image){

  // Copia del header completo
  if(memcpy(&new_image->header, &image->header, sizeof(BMP_Header)) == NULL) return 1;

  switch (image->header.bits_per_pixel) {
    case 24: {
      // Modificacion de la informacion del header 
      new_image->header.bits_per_pixel = 32;
      new_image->header.imagesize = image->norm_height * image->header.width_px * (new_image->header.bits_per_pixel / BITS);
      new_image->header.size = new_image->header.imagesize + HEADER_SIZE;

      new_image->bytes_per_pixel = (new_image->header.bits_per_pixel / BITS);
      new_image->norm_height = image->norm_height;

      break;
    }
    case 32:{

      new_image->bytes_per_pixel = image->bytes_per_pixel;
      new_image->norm_height = image->norm_height;

      break;
    }
  }

  if(new_image->pixels == NULL){

    new_image->pixels = malloc(new_image->norm_height * sizeof(Pixel *));
    if(new_image->pixels == NULL) return 1;

    for(int i = 0; i < image->norm_height; i++) new_image->pixels[i] = malloc(image->header.width_px * sizeof(Pixel));

  }

  return 0;

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
  // if(header->bits_per_pixel != 24 && header->bits_per_pixel != 32){
  if (!((header->bits_per_pixel & 31) == 24) && (header->bits_per_pixel != 32)) {
    return FALSE;
  }
  // Make sure there is only one image plane
  if (header->planes != 1) {
    return FALSE;
  }
  // Make sure there is no compression
  if (header->compression != 0) {
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
