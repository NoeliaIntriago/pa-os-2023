#include "bmp.h"
#include "filter.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    BMP_Image *imageIn;
    BMP_Image *imageOut;
    int index;
    int numThreads;
} parameters;

int boxFilter[FILTER_SIZE][FILTER_SIZE] = {{1, 1, 1},
                                           {1, 1, 1},
                                           {1, 1, 1}};

int getPixelValue(Pixel **pixels_in, int color, int x, int y)
{
    int value = 0;

    for (int i = 0; i < FILTER_SIZE; i++)
    {
        for (int j = 0; j < FILTER_SIZE; j++)
        {
            int m = x - FILTER_SIZE / 2 + i;
            int n = y - FILTER_SIZE / 2 + j;

            int channel;

            switch (color)
            {
            case 'r':
                channel = pixels_in[m][n].red;
                break;
            case 'g':
                channel = pixels_in[m][n].green;
                break;
            case 'b':
                channel = pixels_in[m][n].blue;
                break;
            default:
                return 0;
            }

            value += channel * boxFilter[i][j];
        }
    }

    return value;
}

void apply(BMP_Image *imageIn, BMP_Image *imageOut)
{
    int width_px = imageIn->header.width_px;
    int height_px = imageIn->norm_height;

    // Perform computation on the input image
    for (int i = 0; i < height_px; i++)
    {
        for (int j = 0; j < width_px; j++)
        {
            imageOut->pixels[i][j].red = getPixelValue(imageIn->pixels, 'r', i, j) / (FILTER_SIZE * FILTER_SIZE);
            imageOut->pixels[i][j].green = getPixelValue(imageIn->pixels, 'g', i, j) / (FILTER_SIZE * FILTER_SIZE);
            imageOut->pixels[i][j].blue = getPixelValue(imageIn->pixels, 'b', i, j) / (FILTER_SIZE * FILTER_SIZE);
            imageOut->pixels[i][j].alpha = 255;
        }
    }
}

void *filterThreadWorker(void *args)
{
    parameters *data = (parameters *)args;
    BMP_Image *imageIn = data->imageIn;
    BMP_Image *imageOut = data->imageOut;
    int index = data->index;
    int numThreads = data->numThreads;

    int width_px = imageIn->header.width_px;
    int height_px = imageIn->norm_height;

    int rowsPerThread = height_px / numThreads;
    int startRow = rowsPerThread * index + ((index == 0) ? 1 : 0);
    int endRow = (index == numThreads - 1) ? height_px + 1 : rowsPerThread * (index + 1);

    for (int i = startRow; i < endRow; i++)
    {
        for (int j = 0; j < width_px; j++)
        {
            imageOut->pixels[i - 1][j].red = getPixelValue(imageIn->pixels, 'r', i, j + 1) / (FILTER_SIZE * FILTER_SIZE);
            imageOut->pixels[i - 1][j].blue = getPixelValue(imageIn->pixels, 'b', i, j + 1) / (FILTER_SIZE * FILTER_SIZE);
            imageOut->pixels[i - 1][j].green = getPixelValue(imageIn->pixels, 'g', i, j + 1) / (FILTER_SIZE * FILTER_SIZE);
            imageOut->pixels[i - 1][j].alpha = 255;
        }
    }

    return NULL;
}

void applyParallel(BMP_Image *imageIn, BMP_Image *imageOut, int numThreads)
{
    pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
    parameters *threadData = malloc(numThreads * sizeof(parameters));

    // Create threads and assign work
    for (int i = 0; i < numThreads; i++)
    {
        threadData[i].imageIn = imageIn;
        threadData[i].imageOut = imageOut;
        threadData[i].numThreads = numThreads;
        pthread_create(&threads[i], NULL, filterThreadWorker, &threadData[i]);
    }

    // Wait for threads to complete
    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Clean up resources
    free(threads);
    free(threadData);
}
