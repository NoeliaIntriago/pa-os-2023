#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <sys/wait.h>
#include <fcntl.h>    
#include <string.h>
#include <stdlib.h>
#include "bmp.h"
#include <stdbool.h>

const char *IMAGE_INPUT = "/input_image_obj";
int image_fd = 0;
void *input_pixels_image = NULL;

const char *IMAGE_OUTPUT = "/output_image_obj";
int output_fd = 0;
void *output_pixels_image = NULL;

int loadImage(BMP_Image *image);
int loadFilteredImage(BMP_Image *new_image);
int createImageShm(BMP_Image *image);

pid_t filterBlur(BMP_Image *img, int numThreads);
pid_t filterEdgeDetection(BMP_Image *img, int numThreads);

int modifyImage(BMP_Image *image, BMP_Image *new_image);

void clearShmObjs(int s);

int main(int argc, char *argv[])
{

  if(argc != 2)
  {
    printError(ARGUMENT_ERROR);
    return 1;
  }

  int nthreads = atoi(argv[1]);

  if(nthreads <= 0)
  {
    fprintf(stderr, "El numero de hilos debe ser mayor a 0\n");
    return 1;
  }

  char filename_output[] = "outputs/filtered.bmp";
  char filename[100] = {0};
  while(true)
  {
    printf("Ingrese la ruta de la imagen: ");
    fgets(filename, 100, stdin);

    filename[strlen(filename) - 1] = '\0';

    FILE *sc = fopen(filename, "rb");
    if (sc == NULL) fprintf(stderr, "Ingrese una ruta correcta...\n");
    else {
      fclose(sc);
      break;
    }

  }

  FILE *source;
  if((source = fopen(filename, "rb")) == NULL) 
  {
    printError(FILE_ERROR);
    return 1;
  }

  FILE *dest;
  if((dest = fopen(filename_output, "wb")) == NULL) 
  {
    printError(FILE_ERROR);
    return 1;
  }

  BMP_Image* image = malloc(sizeof(BMP_Image));
  BMP_Image* new_image = malloc(sizeof(BMP_Image));
  if(image == NULL || new_image == NULL)
  {
    printError(MEMORY_ERROR);
    return 1;
  }

  if(readImage(source, image) != 0)
  {
    fprintf(stderr, "No se pudo leer la imagen\n");
    return 1;
  }

  if(!checkBMPValid(&image->header)) 
  {
    printf("Imagen de entrada: \n");
    printError(VALID_ERROR);
    return 1;
  }

  printf("IMAGEN DE ENTRADA: \n");
  printBMPHeader(&image->header);
  printBMPImage(image);

  if(loadImage(image) != 0) 
  {
    fprintf(stderr, "Memoria compartida: No se pudo crear el espacio de memoria compartida para los pixeles de la imagen de entrada\n");
    freeImage(image);
    fclose(source);
    return 1;
  }

  if(createImageShm(image) != 0) 
  {
    fprintf(stderr, "Memoria compartida: No se pudo crear el espacio de memoria compartida para los pixeles de la imagen de salida\n");
    freeImage(image);
    fclose(source);

    clearShmObjs(sizeof(Pixel) * image->norm_height * image->header.width_px);

    return 1;
  }

  pid_t blur_filter = filterBlur(image, nthreads);
  if(blur_filter == -1) 
  {
    fprintf(stderr, "Blur Filter: Error al ejecutar el programa blur_filter\n");

    freeImage(image);
    fclose(source);

    clearShmObjs(sizeof(Pixel) * image->norm_height * image->header.width_px);

    return 1;
  }

  pid_t edge_filter = filterEdgeDetection(image, nthreads);
  if(edge_filter == -1) 
  {
    fprintf(stderr, "Edge Detection Filter: Error al ejecutar el programa edge_detection_filter\n");

    freeImage(image);
    fclose(source);

    clearShmObjs(sizeof(Pixel) * image->norm_height * image->header.width_px);

    return 1;
  }

  int status_blur, status_edge;
  waitpid(blur_filter, &status_blur, 0);
  waitpid(edge_filter, &status_edge, 0);

  if(status_blur != 0 || status_edge != 0)
  {
    fprintf(stderr, "Procesos de filtros: Error los procesos de filtros terminaron de forma inesperada\n");
    fprintf(stderr, "Terminando programa...\n");

    freeImage(image);
    fclose(source);

    clearShmObjs(sizeof(Pixel) * image->norm_height * image->header.width_px);
  }

  if(modifyImage(image, new_image) == -1)
  {
    fprintf(stderr, "Nueva imagen: Error no se pudo crear la nueva imagen para escribirla\n");

    freeImage(image);
    fclose(source);

    clearShmObjs(sizeof(Pixel) * image->norm_height * image->header.width_px);

    return 1;
  }

  if(loadFilteredImage(new_image)) 
  {
    fprintf(stderr, "Nueva imagen: Error no se pudo cargar la imagen filtrada para escribirla\n");

    freeImage(image);
    fclose(source);

    clearShmObjs(sizeof(Pixel) * image->norm_height * image->header.width_px);

    return 1;
  }

  if(writeImage(dest, new_image) == 0){ printf("*** Imagen escrita correctamente ***\n\n"); }
  else fprintf(stderr, "Escribir imagen: Error no se pudo escribir la imagen\n");

  freeImage(image);
  fclose(source);

  clearShmObjs(sizeof(Pixel) * image->norm_height * image->header.width_px);

  return 0;
}

int loadImage(BMP_Image *image)
{
  image_fd = shm_open(IMAGE_INPUT, O_CREAT | O_TRUNC | O_RDWR, 0666);
  if(image_fd == -1) return -1;

	int truncate = ftruncate(image_fd, image->norm_height * image->header.width_px * sizeof(Pixel));
  if(truncate != 0) return -1;

  input_pixels_image = mmap(0, image->norm_height * image->header.width_px * sizeof(Pixel), PROT_WRITE | PROT_READ, MAP_SHARED, image_fd, 0);
  if(input_pixels_image == NULL) return -1;

  int step = sizeof(Pixel) * image->header.width_px;
  for(int i = 0, j = 0; i < image->norm_height; i++, j+=step) {
    memcpy(input_pixels_image + j, image->pixels[i], step);
  }

  return 0;
}

int createImageShm(BMP_Image *image) 
{
  output_fd = shm_open(IMAGE_OUTPUT, O_CREAT | O_TRUNC | O_RDWR, 0666);
  if(output_fd == -1) return -1;

	int truncate = ftruncate(output_fd, image->norm_height * image->header.width_px * sizeof(Pixel));
  if(truncate != 0) return -1;

  output_pixels_image = mmap(0, image->norm_height * image->header.width_px * sizeof(Pixel), PROT_WRITE | PROT_READ, MAP_SHARED, output_fd, 0);
  if(output_pixels_image == NULL) return -1;

  return 0;
}

pid_t filterBlur(BMP_Image *img, int numThreads)
{
  char command[100] = {0};
  // sprintf(command, "./blur_filter %d,%d,%d,%d,%d", 0, img->norm_height / 2 - 1, img->norm_height, img->header.width_px, numThreads);
  sprintf(command, "./blur_filter %d,%d,%d,%d,%d", img->norm_height / 2, img->norm_height, img->norm_height, img->header.width_px, numThreads);
  command[strlen(command)] = '\0';

  pid_t child = fork();

  if(child == 0) 
  {
    execlp("/bin/sh","/bin/sh", "-c", command, (char *)NULL);
    return -1;
  }

  return child;
}

pid_t filterEdgeDetection(BMP_Image *img, int numThreads)
{
  char command[100] = {0};
  // sprintf(command, "./edge_detection_filter %d,%d,%d,%d,%d", img->norm_height / 2, img->norm_height, img->norm_height, img->header.width_px, numThreads);
  sprintf(command, "./edge_detection_filter %d,%d,%d,%d,%d", 0, img->norm_height / 2 - 1, img->norm_height, img->header.width_px, numThreads);
  command[strlen(command)] = '\0';

  pid_t child = fork();

  if(child == 0) 
  {
    execlp("/bin/sh","/bin/sh", "-c", command, (char *)NULL);
    return -1;
  }

  return child;
}

int modifyImage(BMP_Image *image, BMP_Image *new_image)
{
  memcpy(&(new_image->header), &(image->header), sizeof(BMP_Header));

  switch (image->header.bits_per_pixel)
  {
    case 24: {
      new_image->header.bits_per_pixel = 32;
      new_image->header.imagesize = image->norm_height * image->header.width_px * (new_image->header.bits_per_pixel / 8);
      new_image->header.size = new_image->header.imagesize + sizeof(BMP_Header);

      new_image->bytes_per_pixel = new_image->header.bits_per_pixel / 8;
      new_image->norm_height = image->norm_height;

      break;
    }

    case 32:{
      new_image->bytes_per_pixel = image->bytes_per_pixel;
      new_image->norm_height = image->norm_height;

      break;
    }
  }

  new_image->pixels = (Pixel **)malloc(new_image->norm_height * sizeof(Pixel *));
  if (new_image->pixels == NULL)
  {
    printError(MEMORY_ERROR);
    return -1;
  }

  for (int i = 0; i < image->norm_height; i++)
  {
    new_image->pixels[i] = (Pixel *)malloc(image->header.width_px * sizeof(Pixel));
    if (new_image->pixels[i] == NULL)
    {
      for (int j = 0; j < i; j++)
      {
        free(new_image->pixels[j]);
      }
      free(new_image->pixels);
      printError(MEMORY_ERROR);
      return -1;
    }
  }

  return 0;
}

int loadFilteredImage(BMP_Image *new_image){

  int step = sizeof(Pixel) * new_image->header.width_px;
  for(int i = 0, j = 0; i < new_image->norm_height; i++, j+=step) {
    memcpy(new_image->pixels[i], output_pixels_image + j,step);
  }

  return 0;
}

void clearShmObjs(int s)
{
  close(image_fd);
  close(output_fd);

  munmap(input_pixels_image, s);
  munmap(output_pixels_image, s);

  shm_unlink(IMAGE_INPUT);
  shm_unlink(IMAGE_OUTPUT);
}
