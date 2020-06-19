#include <stdio.h>        // printf
#include <stdlib.h>       // exit, atoi
#include <pthread.h>      // pthread*
#include <semaphore.h>
#include <unistd.h>

#define BUFSIZE 1000
#define gMax    20

int NUMPROD = 1;    // number of producers
int NUMCONS = 1;    // number of consumers

int gBuf[BUFSIZE] = {0};  // global buffer. Initialized the array with value of 0 for every indices
int gNum = 0;       // global counter

int gIn  = 0;       // input  cursor in gBuf
int gOut = 0;       // output cursor in gBuf

int gCount = 0;

sem_t empty; //the buffer is full
sem_t full;  //the buffer is empty
//sem_t lock;
pthread_mutex_t mutex;

void say(int me, char* msg, int x) {
    printf("%d ", me);
    printf(msg);
    printf("%d \n", x);
}

void insert(int x) {
        gBuf[gIn] = x;
        ++gIn;
}

int extract() {
    int x = gBuf[gOut];
    gBuf[gOut] = 0; //after the consumer extracts, change the value to 0 to indicate it's empty
    ++gOut;
  return x;
}

int incgNum() {
  int num = ++gNum;
  return num;
}

//creates a method to decreasing gNum
void decNum() {
    --gNum;
}

int getgNum() {
  int num = gNum;
  return num;
}


/*void* producer(void* arg) {
  int me = pthread_self();
    int num = getgNum();
  while (num < gMax) {
      sem_wait(&empty);
      pthread_mutex_lock(&mutex);
      //sem_wait(&lock);
          num = incgNum();
          insert(gNum);
          say(me, "Produced: ", num);
      //sem_post(&lock);
      pthread_mutex_unlock(&mutex);
      sem_post(&full);
      }
  return NULL;
}*/

void* producer(void* arg) {
    int me = pthread_self();
    while (1) {
        sem_wait(&empty); //the producer needs to wait for the consumer to consume
        pthread_mutex_lock(&mutex);  //lock the critical section
        //sem_wait(&lock);
        int num = getgNum();
        if(num == gMax) {  //if the producer produced 20, the thread will exit
            //remember to unlock and release the semaphore to the consumer
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
            break;
        }
        num = incgNum();
        insert(gNum); //produce
        say(me, "Produced: ", num);
        //sem_post(&lock);
        //unlock and signal the consumer to consume
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg) {
  int me = pthread_self();
  while (1) {
      sem_wait(&full); //the consumer needs to wait for the producer to produce
      int exit;
      /*sem_getvalue(&full, &exit);
      if(exit < 0) {
          return NULL;
      }*/
      pthread_mutex_lock(&mutex);  //lock the critical section
      //sem_wait(&lock);
      int num = extract();  //consume
      decNum();
      if(num == 0) {  //if a consumer consume nothing, then it should exit
          //remember to unlock and increment the semaphore in case any other thread is
          //still waiting on the semaphore
          pthread_mutex_unlock(&mutex);
          sem_post(&empty);
          break;
      }
      say(me, "Consumed: ", num);
      if(num == gMax) {  //the producer has stopped producing, it should also exit
          //remember to unlock and increment the semaphore in case any other thread is
          //still waiting on the semaphore
          pthread_mutex_unlock(&mutex);
          sem_post(&empty);
          break;
      }
      //unlock and signal the producer to produce
      pthread_mutex_unlock(&mutex);
      //sem_post(&lock);
      sem_post(&empty);
  }
  return NULL;
}

void checkInput(int argc, char* argv[]) {
  if (argc == 1) {
    NUMPROD = 1;
    NUMCONS = 1;
    return;
  }

  if (argc != 3) {
    printf("Specify <producers>  <consumer> \n");
    printf("Eg:  2  3 \n");
    exit(0);
  }

  NUMPROD = atoi(argv[1]);
  if (NUMPROD < 1 || NUMPROD > 10) {
    printf("Number of producers must lie in the range 1..10 \n");
    exit(0);
  }

  NUMCONS = atoi(argv[2]);
  if (NUMCONS < 1 || NUMCONS > 10) {
    printf("Number of consumers must lie in the range 1..10 \n");
    exit(0);
  }
}


int main(int argc, char* argv[]) {

  checkInput(argc, argv);

  pthread_t prod[NUMPROD];
  pthread_t cons[NUMCONS];
  
    //initialize the mutex lock and 2 semaphores
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFSIZE);
    sem_init(&full, 0, 0);
    //sem_init(&lock, 0, 1);

    
    for (int i = 0; i < NUMPROD; ++i) {
        pthread_create(&prod[i], 0, producer, NULL); }
    for (int i = 0; i < NUMCONS; ++i) {pthread_create(&cons[i], 0, consumer, NULL);}

    for (int i = 0; i < NUMPROD; ++i) {pthread_join(prod[i], NULL);}
    for (int i = 0; i < NUMCONS; ++i) {pthread_join(cons[i], NULL);}
    
    //destroy the mutex lock and the 2 semaphores to prevent memory leak
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    
  printf("All done! Hit any key to finish \n");
  getchar();
  
    
  return 0;
}
