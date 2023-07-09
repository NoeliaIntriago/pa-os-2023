#include "bmp.h"
#include "filter.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    BMP_Image *imageIn;
    BMP_Image *imageOut;
    int startRow;
    int endRow;
} parameters;

int boxFilter[FILTER_SIZE][FILTER_SIZE] = {{1, 1, 1},
                                           {1, 1, 1},
                                           {1, 1, 1}};

/* RETRIEVE ACTUAL PIXEL CHANNEL VALUE */
int getPixelValue(Pixel **pixels, int color, int x, int y)
{
    int value = 0;

    switch (color)
    {
    case 'r':
    {
        for (int k = 0; k < FILTER_SIZE; k++)
        {
            for (int l = 0; l < FILTER_SIZE; l++)
            {
                int m = x - FILTER_SIZE / 2 + k;
                int n = y - FILTER_SIZE / 2 + l;
                value += pixels[m][n].red * boxFilter[k][l];
            }
        }
        break;
    }
    case 'b':
    {
        for (int k = 0; k < FILTER_SIZE; k++)
        {
            for (int l = 0; l < FILTER_SIZE; l++)
            {
                int m = x - FILTER_SIZE / 2 + k;
                int n = y - FILTER_SIZE / 2 + l;
                value += pixels[m][n].blue * boxFilter[k][l];
            }
        }
        break;
    }
    case 'g':
    {
        for (int k = 0; k < FILTER_SIZE; k++)
        {
            for (int l = 0; l < FILTER_SIZE; l++)
            {
                int m = x - FILTER_SIZE / 2 + k;
                int n = y - FILTER_SIZE / 2 + l;
                value += pixels[m][n].green * boxFilter[k][l];
            }
        }
        break;
    }
    }

    return value;
}

/* HANDLE PADDING FOR IMAGE */
void handlePadding(BMP_Image *imageIn)
{
    int width_px = imageIn->header.width_px + 2;
    int height_px = imageIn->norm_height + 2;

    Pixel **padding = calloc(height_px, sizeof(Pixel *));
    for (int i = 0; i < height_px; i++)
    {
        padding[i] = calloc(width_px, sizeof(Pixel));
    }

    for (int j = 0; j < imageIn->norm_height; j++)
    {
        for (int k = 0; k < imageIn->header.width_px; k++)
        {
            padding[j + 1][k + 1].red = imageIn->pixels[j][k].red;
            padding[j + 1][k + 1].blue = imageIn->pixels[j][k].blue;
            padding[j + 1][k + 1].green = imageIn->pixels[j][k].green;
            padding[j + 1][k + 1].alpha = imageIn->pixels[j][k].alpha;
        }
    }

    imageIn->pixels = padding;
}

void apply(BMP_Image *imageIn, BMP_Image *imageOut, int startRow, int endRow)
{
    for (int i = startRow; i < endRow; i++)
    {
        for (int j = 1; j < imageIn->header.width_px + 1; j++)
        {
            imageOut->pixels[i - 1][j - 1].red = getPixelValue(imageIn->pixels, 'r', i, j) / (FILTER_SIZE * FILTER_SIZE);
            imageOut->pixels[i - 1][j - 1].green = getPixelValue(imageIn->pixels, 'g', i, j) / (FILTER_SIZE * FILTER_SIZE);
            imageOut->pixels[i - 1][j - 1].blue = getPixelValue(imageIn->pixels, 'b', i, j) / (FILTER_SIZE * FILTER_SIZE);
            imageOut->pixels[i - 1][j - 1].alpha = 255;
        }
    }
}

void *filterThreadWorker(void *args)
{
    parameters *params = (parameters *)args;

    BMP_Image *imageIn = params->imageIn;
    BMP_Image *imageOut = params->imageOut;
    int startRow = params->startRow;
    int endRow = params->endRow;

    apply(imageIn, imageOut, startRow, endRow);

    return NULL;
}

void applyParallel(BMP_Image *imageIn, BMP_Image *imageOut, int numThreads)
{
    pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
    parameters *params = malloc(numThreads * sizeof(parameters));

    Pixel **temp = imageIn->pixels;
    handlePadding(imageIn); // Zero Padding

    int height_px = imageIn->norm_height;
    int rowsPerThread = height_px / numThreads;
    int remainingRows = height_px % numThreads;
    int startRow = 1;
    int endRow;

    for (int i = 0; i < numThreads; i++)
    {
        endRow = startRow + rowsPerThread - 1;
        if (remainingRows > 0)
        {
            endRow++;
            remainingRows--;
        }

        params[i].imageIn = imageIn;
        params[i].imageOut = imageOut;
        params[i].startRow = startRow;
        params[i].endRow = endRow;

        pthread_create(&threads[i], NULL, filterThreadWorker, &params[i]);

        startRow = endRow + 1;
    }

    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Free the image with padding
    for (int i = 0; i < imageIn->norm_height + 2; i++)
    {
        free(imageIn->pixels[i]);
    }
    free(imageIn->pixels);

    // Reassign the previous image
    imageIn->pixels = temp;

    free(threads);
    free(params);
}
