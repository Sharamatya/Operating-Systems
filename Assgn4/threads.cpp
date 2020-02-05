// A. Sharma, D. Mittal

#include <iostream>
#include <pthread.h>
#include<bits/stdc++.h>
#include<sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <unistd.h>

using namespace std;

#define MAX_PRODUC 10
#define MAX_SIZE 10000
#define WAITTIME 4

#ifndef _POSIX_THREAD_PROCESS_SHARED
#error This system does not support process shared mutex
#endif

pthread_mutex_t m1,m2;

pthread_mutex_t    *mptr = &m1; //Mutex Pointer

pthread_mutexattr_t matr; //Mutex Attribute

pthread_mutex_t    *mptr2 = &m2; //Mutex Pointer

pthread_mutexattr_t matr2; //Mutex Attribute

// pthread_mutex_t m1,m2;
// function declarations
void* proutine(void* args);

void* croutine(void* args);

void* rroutine(void* args);

void* sroutine(void* args);

void legendary(int id){;}

void waitforit(int id){
  pause();
}

typedef struct worker{
  pthread_t id;
  int type;     /// producer or consumercd 
  int status;   /// sleeping = 0 / wake = 1
} worker;

int buffer[MAX_SIZE], size, np, nc, n;
vector<worker> workers;
pthread_t scheduler, reporter;   // instantiate thread vars


int main(){
  // int np, nc, i;
  int i,rtn;
  int arr[n];
  cin>>n;      // user input #producers and consumers
  srand(time(0));
  np = 1+rand()%(n-1);
  nc = n - np;
  cout<<"np= "<< np << " nc= "<<nc<<endl;
  signal(SIGUSR1, legendary);   // run for sigusr1
  signal(SIGUSR2, waitforit); 
  // pthread_aatr_init(&attr2);   // initialize the attribute to pthread


  if (rtn = pthread_mutexattr_init(&matr))
  {
      fprintf(stderr,"pthreas_mutexattr_init: %s",strerror(rtn)),exit(1);
  }
  if (rtn = pthread_mutexattr_setpshared(&matr,PTHREAD_PROCESS_SHARED))
  {
      fprintf(stderr,"pthread_mutexattr_setpshared %s",strerror(rtn)),exit(1);
  }
  // cout<<2<<endl;
  if (rtn = pthread_mutex_init(mptr, &matr))
  {
      fprintf(stderr,"pthread_mutex_init %s",strerror(rtn)), exit(1);
  }
  // cout<<1<<endl;

  if (rtn = pthread_mutexattr_init(&matr2))
  {
      fprintf(stderr,"pthreas_mutexattr_init: %s",strerror(rtn)),exit(1);
  }
  if (rtn = pthread_mutexattr_setpshared(&matr2,PTHREAD_PROCESS_SHARED))
  {
      fprintf(stderr,"pthread_mutexattr_setpshared %s",strerror(rtn)),exit(1);
  }
  if (rtn = pthread_mutex_init(mptr2, &matr2))
  {
      fprintf(stderr,"pthread_mutex_init %s",strerror(rtn)), exit(1);
  }


  // create scheduler
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  if(pthread_create( &(scheduler), &attr, &sroutine, NULL ) != 0 ){
             // initialize the attribute to pthread
          perror("THREAD CREATION ERROR : ");
        }


  // create reporter
  pthread_attr_t attr1;
  pthread_attr_init(&attr1); 
  if(pthread_create( &(reporter), &attr1, &rroutine, NULL ) != 0 ){
           // initialize the attribute to pthread

          perror("THREAD CREATION ERROR : ");
        }

  // create producers
  for ( i = 0; i<np ; i++)
  {
        worker node;
        node.type = 0;
        node.status = 0;
        arr[i]=i;
        pthread_attr_t attr2;
        pthread_attr_init(&attr2);   // initialize the attribute to pthread

        if(pthread_create( &(node.id), &attr2, &proutine, &(arr[i]) ) != 0 ){
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
        arr[i]= i+np;
        pthread_attr_t attr3;
        pthread_attr_init(&attr3);   // initialize the attribute to pthread

        if(pthread_create( &(node.id), &attr3, &croutine, &(arr[i]) ) != 0 ){
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
        workers[i].status = -1;
  }

  // join consumers
  for ( i = 0; i<nc ; i++){
        if(pthread_join((workers[i+np].id), NULL) != 0 ){
          perror("THREAD JOINING ERROR : ");
        }
        workers[i+np].status = -1;

  }


  



  return 0;
}


// function definintions


void* proutine(void* args){
  int id = *(int *)args;
  int i, j= size;
  
  for(i=0; i<MAX_PRODUC; i++){
    srand(time(0)*i);
    while(size>=MAX_SIZE){  cout<<"producer"<<endl;
      // sleep(1);           // sleep while queue is fulll
    }
    // apply lock 
    // push job to the queue
    pthread_mutex_lock(mptr2);
    buffer[size] = rand()%10000;
    size++;
    pthread_mutex_unlock(mptr2);

  }

  workers[id].status = -1;
  pthread_exit(0);

}

void* croutine(void* args){
  int id = *(int *)args;
  while ( size <= 0){
    cout<<"consumer"<<endl;
    sleep(1);
  }

  pthread_mutex_lock(mptr2);
  size--;       // remove elements from stack buffer
  pthread_mutex_unlock(mptr2);
  workers[id].status= -1;
  pthread_exit(0);

}

void* rroutine(void* args){
  // status print

  sleep(.1);

  int prev[n], count;
  for( int i = 0 ; i<n ;i++){
    prev[i] = -2;
  }

  while(1){
    // int flag = 0;
    // cout<<"reporter"<<endl;
    pthread_mutex_lock(mptr);
    // cout<<"hi reporter"<<endl;
    count = 0;
    cout<<"Report:"<<endl;
    for( int i= 0; i<n; i++){

      if( workers[i].status != prev[i]){
        // flag = 1;
        cout << "Worker : "<<i<<" Context Switched from ";
        switch(prev[i]){
          case -1:  cout<<"Terminated";break;
          case 0: cout<<"Sleeping";break;
          case 1: cout<<"AWake";break;
          case -2: cout<<"Initiation";break;
        }
        cout<<" TO : ";

        switch(workers[i].status){
          case -1:  cout<<"Terminated";break;
          case 0: cout<<"Sleeping";break;
          case 1: cout<<"AWake";break;
          case -2: cout<<"Initiation";break;
        }
        cout<<endl;

      }
      if(workers[i].status == -1)count++;
      prev[i]=workers[i].status;
    }

    pthread_mutex_unlock(mptr);
    // cout<<"bye bye reporter"<<endl;
    if ( count >= n)break;
  }

  pthread_exit(0);



}

void* sroutine(void* args)
{
  sleep(0.5);

  int turn = 0,count = 0;

  while(1){
    //signal thread[turn]
      pthread_mutex_lock(mptr);
      cout<<"Count "<<count<<endl;
      if( workers[turn].status != -1){
        // cout<<-1<<endl;
        count = 0;

        // cout<<0<<endl;
        
        // cout<<"hi scheduler"<<endl;
        //signal worker[turn]
        workers[turn].status = 1; 
        // cout<<1<<endl; 
        pthread_kill(workers[turn].id, SIGUSR1);

        pthread_mutex_unlock(mptr);

        sleep(WAITTIME);
        // cout<<2<<endl;
        pthread_mutex_lock(mptr);

        workers[turn].status = 0;  
        pthread_kill(workers[turn].id, SIGUSR2);
        // cout<<3<<endl;
        pthread_mutex_unlock(mptr);

      }

      else{ count++;pthread_mutex_unlock(mptr);}

      turn = (turn+1)%n;
      // cout<<"bye scheduler"<<endl;
      
      if( count >= n)break;



  }

  pthread_exit(0);

}



/* The  sigwait()  function suspends execution of the calling thread until
       one of the signals specified in the signal  set  set  becomes  pending.
       The  function  accepts  the signal (removes it from the pending list of
       signals), and returns the signal number in sig.
*/