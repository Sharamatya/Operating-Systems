// A. Sharma

#include <iostream>
#include <pthread.h>
#include<bits/stdc++.h>
#include<sys/wait.h>

using namespace std;

#define MAX_PRODUC 1000
#define MAX_SIZE 10000
// function declarations
void* proutine(void* args);

void* croutine(void* args);

void* rroutine(void* args);

void* sroutine(void* args);

typedef struct worker{
  pthread_t id;
  int type;     /// producer or consumercd 
  int status;   /// sleeping/ wake
} worker;

int buffer[MAX_SIZE], size, np, nc;
vector<worker> workers;
pthread_t scheduler, reporter;   // instantiate thread vars

int main(){
  // int np, nc, i;
  int i;
  cin>>np>>nc;      // user input #producers and consumers
  pthread_attr_t attr;

  // create scheduler
  if(pthread_create( &(scheduler), &attr, &sroutine, NULL ) != 0 ){
          perror("THREAD CREATION ERROR : ");
        }


  // create reporter
  if(pthread_create( &(reporter), &attr, &rroutine, NULL ) != 0 ){
          perror("THREAD CREATION ERROR : ");
        }

  // create producers
  for ( i = 0; i<np ; i++)
  {
        worker node;
        node.type = 0;
        node.status = 0;
        if(pthread_create( &(node.id), &attr, &proutine, NULL ) != 0 ){
          perror("THREAD CREATION ERROR : ");
        }
        workers.push_back(node);
  }

  // create consumers
  for ( i = 0; i<nc ; i++)
  {
        worker node;
        node.type = 1;
        node.status = 0;
        if(pthread_create( &(node.id), &attr, &croutine, NULL ) != 0 ){
          perror("THREAD CREATION ERROR : ");
        }
        workers.push_back(node);
  }



  // join scheduler
  if(pthread_join( (scheduler), NULL) != 0 )
  {
          perror("THREAD JOINING ERROR : ");
  }


  // join reporter
  if(pthread_join((reporter), NULL) != 0 ){
          perror("THREAD JOINING ERROR : ");
        }

  // join producers
  for ( i = 0; i<np ; i++){
        if(pthread_join((workers[i].id), NULL) != 0 ){
          perror("THREAD JOINING ERROR : ");
        }
  }

  // join consumers
  for ( i = 0; i<nc ; i++){
        if(pthread_join((workers[i+np].id), NULL) != 0 ){
          perror("THREAD JOINING ERROR : ");
        }
  }


  






  return 0;
}


// function definintions


void* proutine(void* args){
  int i, j= size;
  
  for(i=0; i<MAX_PRODUC; i++){
    srand(time(0)*i);
    while(size>=MAX_SIZE){
      sleep(1);           // sleep while queue is fulll
    }
    // push job to the queue
    buffer[i+size] = rand()%10000;
    size++;
  }

  exit(0);

}

void* croutine(void* args){

  while ( size <= 0){
    sleep(1);
  }
  size--;       // remove elements from stack buffer

}

void* rroutine(void* args){
  // status print

}

void* sroutine(void* args){

  int turn = 0;

  while(1){
    //signal thread[turn]
  }




}



/* The  sigwait()  function suspends execution of the calling thread until
       one of the signals specified in the signal  set  set  becomes  pending.
       The  function  accepts  the signal (removes it from the pending list of
       signals), and returns the signal number in sig.
*/

/*
The  pthread_kill() function sends the signal sig to thread, a thread in the same process as
       the caller.  The signal is asynchronously directed to thread.

       If sig is 0, then no signal is sent, but error checking is still performed.
*/
