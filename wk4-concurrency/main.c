#define _GNU_SOURCE
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Shared variables*/
int moleculas_H; // number of hydrogen molecules
int moleculas_O; // number of oxygen molecules
int moleculas_W; // number of water molecules

pthread_t *worker_H; // threads's array for producer hidrogen molecula
pthread_t *worker_O;
pthread_t worker_W;

sem_t mutex;
sem_t cond_water;

void *hidrogens(void *args) {
  sem_wait(&mutex);
  moleculas_H++;
  printf("A hydrogen molecule has been generated. Hydrogen count: %d\n",
         moleculas_H);

  // Check if there are at least 2 hydrogens and 1 oxygen
  if (moleculas_H >= 2 && moleculas_O >= 1) {
    sem_post(&cond_water);
  }
  sem_post(&mutex);

  return NULL;
}

void *oxygens(void *args) {
  sem_wait(&mutex);
  moleculas_O++;
  printf("A oxygen molecule has been generated. Oxygen count: %d\n",
         moleculas_O);

  // Check if there are at least 2 hydrogens and 1 oxygen
  if (moleculas_H >= 2 && moleculas_O >= 1) {
    sem_post(&cond_water);
  }
  sem_post(&mutex);

  return NULL;
}

void *water(void *args) {
  while (1) {
    sem_wait(&cond_water);

    sem_wait(&mutex);
    moleculas_H -= 2;
    moleculas_O--;
    moleculas_W++;
    printf("A water molecule has been generated. Water count: %d\n",
           moleculas_W);

    sem_post(&mutex);
  }

  return NULL;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    perror("Use: ./main numHidrog numOxig");
    return 1;
  }

  moleculas_H = 0;
  moleculas_O = 0;
  moleculas_W = 0;

  sem_init(&mutex, 0, 1);
  sem_init(&cond_water, 0, 0);

  int N = atoi(argv[1]);
  int M = atoi(argv[2]);

  worker_H = malloc(N * sizeof(pthread_t));
  worker_O = malloc(M * sizeof(pthread_t));

  for (int i = 0; i < N; i++)
    pthread_create(&worker_H[i], NULL, hidrogens, NULL);

  for (int i = 0; i < M; i++)
    pthread_create(&worker_O[i], NULL, oxygens, NULL);

  pthread_create(&worker_W, NULL, water, NULL);

  for (int i = 0; i < N; i++)
    pthread_join(worker_H[i], NULL);
  for (int i = 0; i < M; i++)
    pthread_join(worker_O[i], NULL);

  pthread_cancel(worker_W);
  pthread_join(worker_W, NULL);

  sem_destroy(&mutex);
  sem_destroy(&cond_water);

  printf("----------- RESULTS ----------\n");
  printf("Hidrogen count: %d\n", moleculas_H);
  printf("Oxygen count: %d\n", moleculas_O);
  printf("Water count: %d\n", moleculas_W);

  return 0;
}