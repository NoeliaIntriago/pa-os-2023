#include "bmp.h"

#define FILTER_SIZE 3
#define NUMTHREADS 5

void apply(BMP_Image * imageIn, BMP_Image * imageOut, int startRow, int endRow);

void applyParallel(BMP_Image * imageIn, BMP_Image * imageOut, int numThreads);

void *filterThreadWorker(void * args);