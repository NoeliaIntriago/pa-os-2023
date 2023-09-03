#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>    
#include "bmp.h"
#include <stdlib.h>
#include <pthread.h>

#define FILTER_SIZE 3

const char *IMAGE_INPUT = "/input_image_obj";
int image_fd = 0;

const char *IMAGE_OUTPUT = "/output_image_obj";
int output_fd = 0;

typedef enum {RED, BLUE, GREEN} Channel;
int filter[FILTER_SIZE][FILTER_SIZE] = { {1,0,-1}, 
                                         {2,0,-2}, 
                                         {1,0,-1} };

typedef struct
{
  Pixel **imageIn;
  Pixel *imageOut;
  int startRow;
  int endRow;
  int columns;
  int processRow;
} parameters;

void *getInputImage(int s);
void *getOutputImage(int s);
void *filterThreadWorker(void * args);

void applyFilter(Pixel *imageIn, Pixel* imageOut, int height, int width, int i, int j, int numThreads);
Pixel **addPadding(Pixel *imageIn, int height, int width);
void freePaddedImage(Pixel **data, int height);
int compute_channel(Pixel **pixels_in, Channel c, int x, int y);

int main(int argc, char *argv[])
{
  int i = atoi(strtok(argv[1], ","));
  int j = atoi(strtok(NULL, ","));
  int height = atoi(strtok(NULL, ","));
  int width = atoi(strtok(NULL, ","));
  int nthreads = atoi(strtok(NULL, ","));

  Pixel *image_data = (Pixel *)getInputImage(height * width * sizeof(Pixel));
  if(image_data == NULL)
  {
    fprintf(stderr, "Imagen compartida: No se pudo recuperar la imagen compartida de entrada\n");
    return 1;
  }

  Pixel *output_image_data = (Pixel *)getOutputImage(height * width * sizeof(Pixel));
  if(output_image_data == NULL)
  {
    fprintf(stderr, "Imagen compartida: No se pudo recuperar la imagen compartida de salida\n");
    return 1;
  }

  applyFilter(image_data, output_image_data, height, width, i, j, nthreads);

  return 0;
}

void *getInputImage(int s)
{
  image_fd = shm_open(IMAGE_INPUT, O_RDWR, 0666);
  if(image_fd == -1) return NULL;

  void *t = mmap(0, s, PROT_READ | PROT_WRITE, MAP_SHARED, image_fd, 0);

  return t;
}

void *getOutputImage(int s)
{
  output_fd = shm_open(IMAGE_OUTPUT, O_RDWR, 0666);
  if(output_fd == -1) return NULL;

  void *t = mmap(0, s, PROT_READ | PROT_WRITE, MAP_SHARED, output_fd, 0);

  return t;
}

void applyFilter(Pixel *imageIn, Pixel* imageOut, int height, int width, int i, int j, int numThreads)
{
  pthread_t *hilos_workers = malloc(numThreads * sizeof(pthread_t));
  parameters *arguments = malloc(numThreads * sizeof(parameters));

  Pixel **paddedImage = addPadding(imageIn, height, width); // Zero Padding

  int rowsPerThread = (j-i) / numThreads;
  int remainingRows = (j-i) % numThreads;
  int startRow = i;
  int endRow;

  for (int k = 0; k < numThreads; k++)
  {
    endRow = startRow + rowsPerThread;
    if (remainingRows > 0)
    {
      endRow++;
      remainingRows--;
    }

    if(k != 0) 
    {
      startRow++;
    }

    arguments[k].imageIn = paddedImage;
    arguments[k].imageOut = imageOut;
    arguments[k].startRow = startRow;
    arguments[k].endRow = endRow;
    arguments[k].columns = width;
    arguments[k].processRow = j;

    startRow = endRow;

    pthread_create(&hilos_workers[k], NULL, filterThreadWorker, &arguments[k]);
    // printf("%d %d\n", arguments[k].startRow, arguments[k].endRow);
  }

  for(int k = 0; k < numThreads; k++){
    pthread_join(hilos_workers[k], NULL);
  }

  free(arguments);
  free(hilos_workers);
  freePaddedImage(paddedImage, height + 2);
}

Pixel **addPadding(Pixel *imageIn, int height, int width)
{
  int new_height = height + 2;
  int new_width = width + 2;

  Pixel **pixels_padding = calloc(new_height, sizeof(Pixel *));
  for(int i = 0 ; i < new_height; i++) pixels_padding[i] = calloc(new_width, sizeof(Pixel));

  for(int j = 0; j < height; j++)
  {
    for(int k = 0; k < width; k++)
    {
      pixels_padding[j+1][k+1].red = imageIn[j*width + k].red;
      pixels_padding[j+1][k+1].blue = imageIn[j*width + k].blue;
      pixels_padding[j+1][k+1].green = imageIn[j*width + k].green;
      pixels_padding[j+1][k+1].alpha = imageIn[j*width + k].alpha;
    }
  }

  return pixels_padding;
}

void freePaddedImage(Pixel **data, int height)
{
  for(int i = 0; i < height; i++)
  {
    free(data[i]);
  }
  free(data);
}

void *filterThreadWorker(void * args)
{
  parameters *thread_data = (parameters *)args;

  Pixel **imageIn = thread_data->imageIn;
  Pixel *imageOut = thread_data->imageOut;

  int startRow = (thread_data->startRow == 0)? 1 : thread_data->startRow;
  int endRow = (thread_data->processRow == thread_data->endRow)? thread_data->endRow - 1: thread_data->endRow;

  for (int i = startRow; i <= endRow; i++) 
  {
    for (int j = 1; j < thread_data->columns + 1; j++) 
    {
      imageOut[(i-1)*thread_data->columns + (j-1)].red = compute_channel(imageIn, RED, i, j) / (FILTER_SIZE * FILTER_SIZE);
      imageOut[(i-1)*thread_data->columns + (j-1)].blue = compute_channel(imageIn, BLUE, i, j) / (FILTER_SIZE * FILTER_SIZE);
      imageOut[(i-1)*thread_data->columns + (j-1)].green = compute_channel(imageIn, GREEN, i, j) / (FILTER_SIZE * FILTER_SIZE);
      imageOut[(i-1)*thread_data->columns + (j-1)].alpha = 255;
    }
  }

  return NULL;
}

int compute_channel(Pixel **pixels_in, Channel c, int x, int y)
{
  int total = 0;

  switch (c) 
  {
    case RED:{

      for(int k = 0; k < FILTER_SIZE; k++)
      {
        for (int l = 0 ; l < FILTER_SIZE; l++)
        {
          int m = x - FILTER_SIZE / 2 + k;
          int n = y - FILTER_SIZE / 2 + l;

          total += pixels_in[m][n].red * filter[k][l];
        }
      }
      break;
    }
    case BLUE:{

      for(int k = 0; k < FILTER_SIZE; k++)
      {
        for (int l = 0 ; l < FILTER_SIZE; l++)
        {
          int m = x - FILTER_SIZE / 2 + k;
          int n = y - FILTER_SIZE / 2 + l;

          total += pixels_in[m][n].blue * filter[k][l];
        }
      }
      break;
    }
    case GREEN: {

      for(int k = 0; k < FILTER_SIZE; k++)
      {
        for (int l = 0 ; l < FILTER_SIZE; l++)
        {
          int m = x - FILTER_SIZE / 2 + k;
          int n = y - FILTER_SIZE / 2 + l;

          total += pixels_in[m][n].green * filter[k][l];
        }
      }
      break;
    }
  }

  return total;
}
