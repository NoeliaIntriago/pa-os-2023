#include "bmp.h"
#include "filter.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    BMP_Image *imageIn;
    BMP_Image *imageOut;
    int threadIndex;
    int numThreads;
    int boxFilter[FILTER_SIZE][FILTER_SIZE];
} ThreadArgs;

int check(BMP_Image *image, int x, int y, char color)
{
    if (x < 0)
    {
        x++;
    }
    if (x >= image->header.width_px)
    {
        x--;
    }
    if (y < 0)
    {
        y++;
    }
    if (y >= image->norm_height)
    {
        y--;
    }
    if (color == 'b')
    {
        return image->pixels[y][x].blue;
    }
    else if (color == 'g')
    {
        return image->pixels[y][x].green;
    }
    else if (color == 'r')
    {
        return image->pixels[y][x].red;
    }
    else
    {
        return image->pixels[y][x].alpha;
    }
}

int getRedBlurred(BMP_Image *image, int x, int y, int filter[FILTER_SIZE][FILTER_SIZE])
{
    if (filter == NULL)
    {
        for (int i = 0; i < FILTER_SIZE; i++)
        {
            for (int j = 0; j < FILTER_SIZE; j++)
            {
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

int getGreenBlurred(BMP_Image *image, int x, int y, int filter[FILTER_SIZE][FILTER_SIZE])
{
    if (filter == NULL)
    {
        for (int i = 0; i < FILTER_SIZE; i++)
        {
            for (int j = 0; j < FILTER_SIZE; j++)
            {
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

int getBlueBlurred(BMP_Image *image, int x, int y, int filter[FILTER_SIZE][FILTER_SIZE])
{
    if (filter == NULL)
    {
        for (int i = 0; i < FILTER_SIZE; i++)
        {
            for (int j = 0; j < FILTER_SIZE; j++)
            {
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

int getAplhaBlurred(BMP_Image *image, int x, int y, int filter[FILTER_SIZE][FILTER_SIZE])
{
    if (filter == NULL)
    {
        for (int i = 0; i < FILTER_SIZE; i++)
        {
            for (int j = 0; j < FILTER_SIZE; j++)
            {
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

void apply(BMP_Image *imageIn, BMP_Image *imageOut, int boxFilter[FILTER_SIZE][FILTER_SIZE])
{
    // Prepare computed data for the new image
    imageOut->header = imageIn->header;
    imageOut->bytes_per_pixel = imageIn->bytes_per_pixel;
    imageOut->norm_height = imageIn->norm_height;

    // Write new colors for pixels
    for (int i = 0; i < imageIn->norm_height; i++)
    {
        for (int j = 0; j < imageIn->header.width_px; j++)
        {
            int newRed = getRedBlurred(imageIn, i, j, boxFilter);
            int newGreen = getGreenBlurred(imageIn, i, j, boxFilter);
            int newBlue = getBlueBlurred(imageIn, i, j, boxFilter);
            int newAlpha = getAlphaBlurred(imageIn, i, j, boxFilter);

            imageOut->pixels[i][j].red = newRed;
            imageOut->pixels[i][j].green = newGreen;
            imageOut->pixels[i][j].blue = newBlue;
            imageOut->pixels[i][j].alpha = newAlpha;
        }
    }
}

void *filterThreadWorker(void *args)
{
    ThreadArgs *threadData = (ThreadArgs *)args;

    int startRow = (threadData->threadIndex * threadData->imageIn->norm_height) / threadData->numThreads;
    int endRow = ((threadData->threadIndex + 1) * threadData->imageIn->norm_height) / threadData->numThreads;

    for (int i = startRow; i < endRow; i++)
    {
        for (int j = 0; j < threadData->imageIn->header.width_px; j++)
        {
            int newRed = getRedBlurred(threadData->imageIn, i, j, threadData->boxFilter);
            int newGreen = getGreenBlurred(threadData->imageIn, i, j, threadData->boxFilter);
            int newBlue = getBlueBlurred(threadData->imageIn, i, j, threadData->boxFilter);
            int newAlpha = getAlphaBlurred(threadData->imageIn, i, j, threadData->boxFilter);

            threadData->imageOut->pixels[i][j].red = newRed;
            threadData->imageOut->pixels[i][j].green = newGreen;
            threadData->imageOut->pixels[i][j].blue = newBlue;
            threadData->imageOut->pixels[i][j].alpha = newAlpha;
        }
    }

    return NULL;
}

void applyParallel(BMP_Image *imageIn, BMP_Image *imageOut, int boxFilter[FILTER_SIZE][FILTER_SIZE], int numThreads)
{
    imageOut->header = imageIn->header;
    imageOut->bytes_per_pixel = imageIn->bytes_per_pixel;
    imageOut->norm_height = imageIn->norm_height;

    pthread_t threads[numThreads];
    ThreadArgs threadData[numThreads];

    for (int i = 0; i < numThreads; i++)
    {
        threadData[i].imageIn = imageIn;
        threadData[i].imageOut = imageOut;
        threadData[i].threadIndex = i;
        threadData[i].numThreads = numThreads;

        for (int x = 0; x < FILTER_SIZE; x++)
        {
            for (int y = 0; y < FILTER_SIZE; y++)
            {
                threadData[i].boxFilter[x][y] = boxFilter[x][y];
            }
        }

        pthread_create(&threads[i], NULL, filterThreadWorker, &threadData[i]);
    }

    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}
