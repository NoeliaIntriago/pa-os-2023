#include "bmp.h"

#define FILTER_SIZE 3

void apply(BMP_Image * imageIn, BMP_Image * imageOut, int boxFilter[FILTER_SIZE][FILTER_SIZE]);

void applyParallel(BMP_Image * imageIn, BMP_Image * imageOut, int boxFilter[FILTER_SIZE][FILTER_SIZE], int numThreads);

void *filterThreadWorker(void * args);