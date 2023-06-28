#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define NUMOFTHREADS 11

int sudokuBoard[SIZE][SIZE];
int region_isvalid[NUMOFTHREADS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int validateGrids = 2; // Counter for grids validation

typedef struct {
  int row;
  int column;
} parameters;

// Worker function to check rows.
void *checkRows(void *args) {
  parameters *data = (parameters *)malloc(sizeof(parameters));

  for (int row = 0; row < SIZE; row++) {
    int check_array[SIZE] = {0};
    data->row = row;
    for (int col = 0; col < SIZE; col++) {
      int num = sudokuBoard[data->row][col];
      // Checks if number in a cell is valid or wasn't already seen
      if (num < 1 || num > 9) {
        pthread_exit((void *)-1);
      }
      if (check_array[num - 1] == 1) {
        pthread_exit((void *)-1);
      }
      check_array[num - 1] = 1;
    }
  }

  region_isvalid[0] = 1;
  pthread_exit((void *)0);
}

// Worker function to check columns.
void *checkColumns(void *args) {
  parameters *data = (parameters *)malloc(sizeof(parameters));

  for (int col = 0; col < SIZE; col++) {
    int check_array[SIZE + 1] = {0};
    data->column = col;
    for (int row = 0; row < SIZE; row++) {
      int num = sudokuBoard[row][data->column];
      // Checks if number in a cell is valid or wasn't already seen
      if (num < 1 || num > 9) {
        pthread_exit((void *)-1);
      }
      if (check_array[num - 1] == 1) {
        pthread_exit((void *)-1);
      }
      check_array[num - 1] = 1;
    }
  }

  region_isvalid[1] = 1;
  pthread_exit((void *)0);
}

// Worker function to check submatrix.
void *checkSubMatrix(void *args) {
  parameters *coords = (parameters *)args;
  int startRow = coords->row;
  int startCol = coords->column;
  int check_array[SIZE] = {0};

  for (int row = startRow; row < startRow + 3; row++) {
    for (int col = startCol; col < startCol + 3; col++) {
      int num = sudokuBoard[row][col];
      // Checks if number in a cell is valid or wasn't already seen
      if (num < 1 || num > 9) {
        pthread_exit((void *)-1);
      }
      if (check_array[num] == 1) {
        pthread_exit((void *)-1);
      }
      check_array[num] = 1;
    }
  }

  region_isvalid[validateGrids] = 1;
  validateGrids++;
  pthread_exit((void *)0);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: ./main [filename]\n");
    exit(EXIT_FAILURE);
  }

  parameters *coords = (parameters *)malloc(sizeof(parameters));
  coords->row = 0;
  coords->column = 0;

  parameters *coord_sg[9] = {0};
  // Sub-grid 1
  coord_sg[0] = (parameters *)malloc(sizeof(parameters));
  coord_sg[0]->row = 0;
  coord_sg[0]->column = 0;
  // Sub-grid 2
  coord_sg[1] = (parameters *)malloc(sizeof(parameters));
  coord_sg[1]->row = 0;
  coord_sg[1]->column = 3;
  // Sub-grid 3
  coord_sg[2] = (parameters *)malloc(sizeof(parameters));
  coord_sg[2]->row = 0;
  coord_sg[2]->column = 6;
  // Sub-grid 4
  coord_sg[3] = (parameters *)malloc(sizeof(parameters));
  coord_sg[3]->row = 3;
  coord_sg[3]->column = 0;
  // Sub-grid 5
  coord_sg[4] = (parameters *)malloc(sizeof(parameters));
  coord_sg[4]->row = 3;
  coord_sg[4]->column = 3;
  // Sub-grid 6
  coord_sg[5] = (parameters *)malloc(sizeof(parameters));
  coord_sg[5]->row = 3;
  coord_sg[5]->column = 6;
  // Sub-grid 7
  coord_sg[6] = (parameters *)malloc(sizeof(parameters));
  coord_sg[6]->row = 6;
  coord_sg[6]->column = 0;
  // Sub-grid 8
  coord_sg[7] = (parameters *)malloc(sizeof(parameters));
  coord_sg[7]->row = 6;
  coord_sg[7]->column = 3;
  // Sub-grid 9
  coord_sg[8] = (parameters *)malloc(sizeof(parameters));
  coord_sg[8]->row = 6;
  coord_sg[8]->column = 6;

  char *file_name = argv[1];
  loadBoard(file_name, sudokuBoard);
  pthread_t threads[NUMOFTHREADS];
  pthread_attr_t thread_attrs[NUMOFTHREADS];

  // Thread to check rows
  pthread_attr_init(&thread_attrs[0]);
  pthread_create(&threads[0], &thread_attrs[0], checkRows, NULL);

  // Thread to check columns
  pthread_attr_init(&thread_attrs[1]);
  pthread_create(&threads[1], &thread_attrs[1], checkColumns, NULL);

  // Threads to check sub-grids
  for (int i = 2; i < NUMOFTHREADS; i++) {
    pthread_attr_init(&thread_attrs[i]);
    pthread_create(&threads[i], &thread_attrs[i], checkSubMatrix,
                   (void *)coord_sg[i - 2]);
  }

  // Joining threads
  for (int i = 0; i < NUMOFTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  for (int i = 0; i < NUMOFTHREADS; i++) {
    if (!region_isvalid[i]) {
      printf("ERROR: Sudoku board not valid\n");
      exit(EXIT_FAILURE);
    }
  }

  printf("Sudoku board valid\n");
  exit(EXIT_SUCCESS);
}
