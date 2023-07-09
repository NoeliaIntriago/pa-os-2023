#include "bmp.h"
#include <pthread.h>
#include <stdlib.h>

#define FILTER_SIZE 3
typedef struct 
{ 
    BMP_Image* imageIn;
    BMP_Image* imageOut;
    int startRow;
    int boxFilter[3][3];
    int steps;
    int index;
    int numThreads;
} parameters;

/* FUNCTIONS TO CALCULATE NEW VALUE -> BLURRED PIXEL */

int check(BMP_Image* image, int x, int y, char color) {
    if (x < 0) {
    x++;
  }
  if (x >= image->header.width_px) {
    x--;
  }
  if (y < 0) {
    y++;
  }
  if (y >= image->norm_height) {
    y--;
  }
  if (color == 'b') {
    return image->pixels[y][x].blue;
  }
  else if (color == 'g') {
    return image->pixels[y][x].green;
  }
  else if (color == 'r') {
    return image->pixels[y][x].red;
  }
  else {
    return image->pixels[y][x].alpha;
  }
}

int getRedBlurred(BMP_Image* image, int x, int y, int filter[FILTER_SIZE][FILTER_SIZE]) {
    if (filter == NULL) {
        for(int i = 0; i < FILTER_SIZE; i++) {
            for(int j = 0; j < FILTER_SIZE; j++) {
                filter[i][j] = 1;
            }
        }
    }

    int blurredValue = image->pixels[x][y].red * filter[1][1];

    blurredValue += check(image, x - 1, y - 1, "r") * filter[0][0];
    blurredValue += check(image, x - 1, y, "r") * filter[0][1];
    blurredValue += check(image, x - 1, y + 1, "r") * filter[0][2];

    blurredValue += check(image, x, y - 1, "r") * filter[1][0];
    blurredValue += check(image, x, y + 1, "r") * filter[1][2];

    blurredValue += check(image, x + 1, y - 1, "r") * filter[2][0];
    blurredValue += check(image, x + 1, y, "r") * filter[2][1];
    blurredValue += check(image, x + 1, y + 1, "r") * filter[2][2];

    return blurredValue / 9;
}

int getGreenBlurred(BMP_Image* image, int x, int y, int filter[FILTER_SIZE][FILTER_SIZE]) {
    if (filter == NULL) {
        for(int i = 0; i < FILTER_SIZE; i++) {
            for(int j = 0; j < FILTER_SIZE; j++) {
                filter[i][j] = 1;
            }
        }
    }

    int blurredValue = image->pixels[x][y].green * filter[1][1];

    blurredValue += check(image, x - 1, y - 1, "g") * filter[0][0];
    blurredValue += check(image, x - 1, y, "g") * filter[0][1];
    blurredValue += check(image, x - 1, y + 1, "g") * filter[0][2];

    blurredValue += check(image, x, y - 1, "g") * filter[1][0];
    blurredValue += check(image, x, y + 1, "g") * filter[1][2];

    blurredValue += check(image, x + 1, y - 1, "g") * filter[2][0];
    blurredValue += check(image, x + 1, y, "g") * filter[2][1];
    blurredValue += check(image, x + 1, y + 1, "g") * filter[2][2];

    return blurredValue / 9;
}

int getBlueBlurred(BMP_Image* image, int x, int y, int filter[FILTER_SIZE][FILTER_SIZE]) {
    if (filter == NULL) {
        for(int i = 0; i < FILTER_SIZE; i++) {
            for(int j = 0; j < FILTER_SIZE; j++) {
                filter[i][j] = 1;
            }
        }
    }

    int blurredValue = image->pixels[x][y].blue * filter[1][1];

    blurredValue += check(image, x - 1, y - 1, "b") * filter[0][0];
    blurredValue += check(image, x - 1, y, "b") * filter[0][1];
    blurredValue += check(image, x - 1, y + 1, "b") * filter[0][2];

    blurredValue += check(image, x, y - 1, "b") * filter[1][0];
    blurredValue += check(image, x, y + 1, "b") * filter[1][2];

    blurredValue += check(image, x + 1, y - 1, "b") * filter[2][0];
    blurredValue += check(image, x + 1, y, "b") * filter[2][1];
    blurredValue += check(image, x + 1, y + 1, "b") * filter[2][2];

    return blurredValue / 9;
}

int getAplhaBlurred(BMP_Image* image, int x, int y, int filter[FILTER_SIZE][FILTER_SIZE]) {
    if (filter == NULL) {
        for(int i = 0; i < FILTER_SIZE; i++) {
            for(int j = 0; j < FILTER_SIZE; j++) {
                filter[i][j] = 1;
            }
        }
    }

    int blurredValue = image->pixels[x][y].alpha * filter[1][1];

    blurredValue += check(image, x - 1, y - 1, "a") * filter[0][0];
    blurredValue += check(image, x - 1, y, "a") * filter[0][1];
    blurredValue += check(image, x - 1, y + 1, "a") * filter[0][2];

    blurredValue += check(image, x, y - 1, "a") * filter[1][0];
    blurredValue += check(image, x, y + 1, "a") * filter[1][2];

    blurredValue += check(image, x + 1, y - 1, "a") * filter[2][0];
    blurredValue += check(image, x + 1, y, "a") * filter[2][1];
    blurredValue += check(image, x + 1, y + 1, "a") * filter[2][2];

    return blurredValue / 9;
}

/* THREADS FUNCTIONS */

void apply(BMP_Image * imageIn, BMP_Image * imageOut) {
    // Prepare computed data for new image
    imageOut->header = imageIn->header;
    imageOut->bytes_per_pixel = imageIn->bytes_per_pixel;
    imageOut->norm_height = imageIn->norm_height;

    // Write new colors for pixels
    for (int i = 0; i < imageIn->norm_height; i++) {
        for (int j = 0; j < imageIn->header.width_px; j++) {
            int newRed = obtenerRojo(imageIn, i, j, NULL);
            int newGreen = obtenerVerde(imageIn, i, j, NULL);
            int newBlue = obtenerAzul(imageIn, i, j, NULL);
            int newAlpha = obtenerAlfa(imageIn, i, j, NULL);

            imageOut->pixels[i][j].red = newRed;
            imageOut->pixels[i][j].green = newGreen;
            imageOut->pixels[i][j].blue = newBlue;
            imageOut->pixels[i][j].alpha = newAlpha;
        }
    }  
}

void applyParallel(BMP_Image * imageIn, BMP_Image * imageOut, int boxFilter[3][3], int numThreads) {
    imageOut->header = imageIn->header;
    imageOut->bytes_per_pixel = imageIn->bytes_per_pixel;
    imageOut->norm_height = imageIn->norm_height;

    pthread_t threads[numThreads];
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    
    int status;

    int steps = imageIn->norm_height/numThreads;
    int contador = 0;
    for(int i = 0; i < imageIn->norm_height; i += steps){
        parameters *data = (parameters *) malloc(sizeof(parameters));

        data->startRow = i;
        data->steps = steps;
        data->imageIn = imageIn;
        data->imageOut = imageOut;
        data->index = contador;
        data->numThreads = numThreads;
        for(int x = 0;  x < FILTER_SIZE; x++){
            for(int y = 0; y < FILTER_SIZE; y++){
                data->boxFilter[x][y] = boxFilter[x][y];
            }
        }
        status = pthread_create(&threads[contador], &thread_attr, filterThreadWorker, (void*) data);
        if (status != 0)
            exit(status);
        contador++;
    }
    
    pthread_attr_destroy(&thread_attr);
    for (int i = 0; i < contador; i++){
        status = pthread_join(threads[i], (void **)&status);
        if (status != 0)
            exit(status);
    }
}

void *filterThreadWorker(void * args) {
    parameters* data= (parameters*) args;
    int startRow = data->startRow;
    int endRow;
    
    if(data->index == data->numThreads - 1){
        endRow = data->imageIn->norm_height - 1;
    } else {
        endRow = startRow + data->numThreads;
    }

    for(int i = startRow; i <= endRow; i++){
        for(int j = 0; j < data->imageIn->header.width_px; j++){
            int newRed = obtenerRojo(data->imageIn, i, j, data->boxFilter);
            int newGreen = obtenerVerde(data->imageIn, i, j, data->boxFilter);
            int newBlue = obtenerAzul(data->imageIn, i, j, data->boxFilter);
            int newAlpha = obtenerAlfa(data->imageIn, i, j, data->boxFilter);

            data->imageOut->pixels[i][j].red = newRed;
            data->imageOut->pixels[i][j].green = newGreen;
            data->imageOut->pixels[i][j].blue = newBlue;
            data->imageOut->pixels[i][j].alpha = newAlpha;
        }
    }
    pthread_exit((void *) 1);
    return NULL;
}