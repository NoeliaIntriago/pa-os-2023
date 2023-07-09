#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bmp.h"

void printError(int error) {
  switch (error) {
    case ARGUMENT_ERROR:
      printf("Usage: ex5 <source> <destination>\n");
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

BMP_Image* createBMPImage(FILE* fptr) {
  BMP_Image* image = (BMP_Image*)malloc(sizeof(BMP_Image));
  if (image == NULL) {
    return NULL;
  }

  if (fread(&(image->header), sizeof(BMP_Header), 1, fptr) != 1) {
    free(image);
    return NULL;
  }

  int width_px = image->header.width_px;
  int height_px = abs(image->header.height_px);

  image->bytes_per_pixel = image->header.bits_per_pixel / 8;
  image->norm_height = height_px;

  image->pixels = (Pixel**)malloc(height_px * sizeof(Pixel*));
  if (image->pixels == NULL) {
    free(image);
    return NULL;
  }

  for (int i = 0; i < height_px; i++) {
    image->pixels[i] = (Pixel*)malloc(width_px * sizeof(Pixel));
    if (image->pixels[i] == NULL) {
      for (int j = 0; j < i; j++) {
        free(image->pixels[j]);
      }
      free(image->pixels);
      free(image);
      return NULL;
    }
  }

  return image;
}

int readImageData(FILE* srcFile, BMP_Image* image, int dataSize) {
  for (int i = 0; i < image->norm_height; i++) {
    for (int j = 0; j < image->header.width_px; j++) {  // Add this line
      if (fread(&image->pixels[i][j], dataSize, 1, srcFile) != 1) {  // Modify this line
        fprintf(stderr, "Pixel (%d,%d) could not be read from the file\n", i, j);
        return 1;
      }
    }
  }

  return 0;
}

int readImage(FILE* srcFile, BMP_Image* dataImage) {
  BMP_Image* tmp = createBMPImage(srcFile);
  if (tmp == NULL) {
    return 1;
  }

  *dataImage = *tmp;
  free(tmp);

  return readImageData(srcFile, dataImage, dataImage->bytes_per_pixel);
}

int writeImage(char* destFileName, BMP_Image* dataImage) {
  FILE* output = fopen(destFileName, "wb");
  if (fwrite(&(dataImage->header), sizeof(BMP_Header), 1, output) != 1) {
    return 1;
  }

  for (int i = 0; i < dataImage->norm_height; i++) {
    if (fwrite(dataImage->pixels[i], sizeof(Pixel), dataImage->header.width_px, output) != dataImage->header.width_px) {
      fprintf(stderr, "Row (%d) could not be written to the output image\n", i);
    }
  }

  return 0;
}

int modifyImage(BMP_Image* image, BMP_Image* new_image) {
  memcpy(&(new_image->header), &(image->header), sizeof(BMP_Header));

  switch (image->header.bits_per_pixel) {
    case 24: {
      new_image->header.bits_per_pixel = 32;
      new_image->header.imagesize = image->norm_height * image->header.width_px * (new_image->header.bits_per_pixel / 8);
      new_image->header.size = new_image->header.imagesize + sizeof(BMP_Header);

      new_image->bytes_per_pixel = new_image->header.bits_per_pixel / 8;
      new_image->norm_height = image->norm_height;

      break;
    }
    case 32: {
      new_image->bytes_per_pixel = image->bytes_per_pixel;
      new_image->norm_height = image->norm_height;

      break;
    }
  }

  new_image->pixels = (Pixel**)malloc(new_image->norm_height * sizeof(Pixel*));
  if (new_image->pixels == NULL) {
    return 1;
  }

  for (int i = 0; i < image->norm_height; i++) {
    new_image->pixels[i] = (Pixel*)malloc(image->header.width_px * sizeof(Pixel));
    if (new_image->pixels[i] == NULL) {
      for (int j = 0; j < i; j++) {
        free(new_image->pixels[j]);
      }
      free(new_image->pixels);
      return 1;
    }
  }

  return 0;
}

void freeImage(BMP_Image* image) {
  for (int i = 0; i < image->norm_height; i++) {
    free(image->pixels[i]);
  }
  free(image->pixels);
  free(image);
}

int checkBMPValid(BMP_Header* header) {
  if (header->type != 0x4D42) {
    return FALSE;
  }

  if (!((header->bits_per_pixel & 31) == 24) && (header->bits_per_pixel != 32)) {
    return FALSE;
  }

  if (header->planes != 1) {
    return FALSE;
  }

  if (header->compression != 0) {
    return FALSE;
  }

  return TRUE;
}

void printBMPHeader(BMP_Header* header) {
  printf("file type (should be 0x4D42): %x\n", header->type);
  printf("file size: %d\n", header->size);
  printf("offset to image data: %d\n", header->offset);
  printf("header size: %d\n", header->header_size);
  printf("width_px: %d\n", header->width_px);
  printf("height_px: %d\n", header->height_px);
  printf("planes: %d\n", header->planes);
  printf("bits: %d\n", header->bits_per_pixel);
}

void printBMPImage(BMP_Image* image) {
  printf("data size is %ld\n", sizeof(image->pixels));
  printf("norm_height size is %d\n", image->norm_height);
  printf("bytes per pixel is %d\n", image->bytes_per_pixel);
}
