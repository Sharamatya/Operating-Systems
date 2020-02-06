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

#define MAX_PRODUC 100
#define MAX_SIZE 10000
#define MAXLIMIT 100
#define WAITTIME 4

#ifndef _POSIX_THREAD_PROCESS_SHARED
#error This system does not support process shared mutex
#endif

pthread_mutex_t m1,m2;

pthread_mutex_t    *mptr = &m1; //Mutex Pointer

pthread_mutexattr_t matr; //Mutex Attribute

pthread_mutex_t    *mptr2 = &m2; //Mutex Pointer

pthread_mutexattr_t matr2; //Mutex Attribute

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
  int number;
  pthread_t id;
  int type;     /// producer or consumercd 
  int status;   /// sleeping = 0 / wake = 1
} worker;

int buffer[MAX_SIZE], size, np, nc, n,tj;
// vector<worker> workers;
worker workers[MAXLIMIT];
pthread_t scheduler, reporter;   // instantiate thread vars


int main(){
  int i,rtn;
  int arr[MAXLIMIT];
  cin>>n;      // user input #producers and consumers
  srand(time(0));
  np = 1+rand()%(n-1);
  nc = n - np;
  cout<<"np= "<< np << " nc= "<<nc<<endl;
  signal(SIGUSR1, legendary);   // run for sigusr1
  signal(SIGUSR2, waitforit); 
  tj=0;
  size=0;


  if (rtn = pthread_mutexattr_init(&matr))
  {
      fprintf(stderr,"pthreas_mutexattr_init: %s",strerror(rtn)),exit(1);
  }
  if (rtn = pthread_mutexattr_setpshared(&matr,PTHREAD_PROCESS_SHARED))
  {
      fprintf(stderr,"pthread_mutexattr_setpshared %s",strerror(rtn)),exit(1);
  }
  if (rtn = pthread_mutex_init(mptr, &matr))
  {
      fprintf(stderr,"pthread_mutex_init %s",strerror(rtn)), exit(1);
  }

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

  // create producers
  // int index=0;
  for ( i = 0; i<np ; i++)
  {
        worker node;
        node.number = i;
        node.type = 0;
        node.status = 0;
        arr[i]=i;
        pthread_attr_t attr2;
        pthread_attr_init(&attr2);   // initialize the attribute to pthread
        
        if(pthread_create( &(node.id), NULL, proutine, &(arr[i]) ) != 0 ){
          perror("THREAD CREATION ERROR : ");
        }
        
        workers[i].id =  node.id;
        workers[i].number =  node.number;
        workers[i].type =  node.type;
        workers[i].status =  node.status;
        cout<<"id: "<<workers[i].id<<endl;
        pthread_mutex_lock(mptr);
        pthread_kill(workers[i].id,SIGUSR2);
        pthread_mutex_unlock(mptr);
        // workers.push_back(node);
  }
  cout<<"producers created\n";
  // create consumers
  for ( i = 0; i<nc ; i++)
  {
        worker node;
        node.type = 1;
        node.number =i+np;
        node.status = 0;
        arr[i+np]= i+np;
        // pthread_attr_t attr3;
        // pthread_attr_init(&attr3);   // initialize the attribute to pthread
        if(pthread_create( &(node.id), NULL, croutine, &(arr[i+np]) ) != 0 ){
          perror("THREAD CREATION ERROR : ");
        }
        // workers.push_back(node);
        workers[i+np].id =  node.id;
        workers[i+np].number =  node.number;
        workers[i+np].type =  node.type;
        workers[i+np].status =  node.status;
        cout<<"id: "<<workers[i+np].id<<endl;

        

        pthread_mutex_lock(mptr);
        pthread_kill(workers[i+np].id,SIGUSR2);
        pthread_mutex_unlock(mptr);
        // workers.push_back(node);
  }
    cout<<"consumers created\n";



  if(pthread_create( &(scheduler), &attr, sroutine, NULL ) != 0 ){
             // initialize the attribute to pthread
          perror("THREAD CREATION ERROR : ");
        }


  // create reporter
  pthread_attr_t attr1;
  pthread_attr_init(&attr1); 
  if(pthread_create( &(reporter), &attr1, rroutine, NULL ) != 0 ){
           // initialize the attribute to pthread

          perror("THREAD CREATION ERROR : ");
        }



  // join scheduler


  // join producers
  for ( i = 0; i<np ; i++){
        if(pthread_join((workers[i].id), NULL) != 0 ){
          printf("THREAD JOINING ERROR : %d\n",i);
        }
        workers[i].status = -1;
  }

  // join consumers
  for ( i = 0; i<nc ; i++){
        if(pthread_join((workers[i+np].id), NULL) != 0 ){
          printf("THREAD JOINING ERROR : %d\n",i+np);
        }
        workers[i+np].status = -1;

  }

    if(pthread_join( (scheduler), NULL) != 0 )
  {
          perror("THREAD JOINING ERROR : ");
  }

 cout<<"In main(): joined scheduler"<<endl;
  // join reporter
  if(pthread_join((reporter), NULL) != 0 ){
          perror("THREAD JOINING ERROR : ");
        }


  cout<<"In main() : joined reporter"<<endl;



  return 0;
}


// function definintions


void* proutine(void* args){
  int id = *(int *)args;
  int i, j= size;
  
  for(i=0; i<MAX_PRODUC; i++){
    srand(time(0)*i);
    while(size>=MAX_SIZE){  
      // cout<<"producer"<<endl;
      sleep(1);           // sleep while queue is fulll
    }
    // apply lock 
    // push job to the queue
    pthread_mutex_lock(mptr);
    // cout<<"P:"<<id<<" J:"<<i<<" S:"<<size<<endl;
    buffer[size] = rand()%10000;
    size++;
    pthread_mutex_unlock(mptr);

  }
  // cout<<"P:"<<id<<"terminated"<<endl;
  workers[id].status = -1;
  pthread_exit(0);

}

void* croutine(void* args){
  int id = *(int *)args;
  int count=0,flag=1;
  while(flag){

    while ( size <= 0 && tj < np*MAX_PRODUC ){
      // cout<<"consumer"<<endl;
      sleep(1);
    }

    pthread_mutex_lock(mptr);
    int fg=0;
    for(int i =0;i<np;i++){
      if(workers[i].status == -1);
      else fg=1;
    }
    if(fg==0 &&size<=0){      // if all producers terminated and buffer is empty
      
      flag=0;
      pthread_mutex_unlock(mptr);
      break;
    
    }
    if(tj>=np*MAX_PRODUC){
      // cout<<"tj:"<<tj<<endl;
      
      flag=0;
      pthread_mutex_unlock(mptr);
      break;
    }
    if(size<=0)continue;
    size--;       // remove elements from stack buffer
    tj++;
    // cout<<"C:"<<id<<" S:"<<size<<endl;
    if(tj>=np*MAX_PRODUC){
      
      flag=0;
      pthread_mutex_unlock(mptr);
      break;
    }
    pthread_mutex_unlock(mptr);
  }
  workers[id].status= -1;

  // cout<<"C:"<<id<<"terminated"<<endl;
  pthread_exit(0);
// 
}

void* rroutine(void* args){
  // status print

  sleep(.1);

  int prev[n], count;
  for( int i = 0 ; i<n ;i++){
    prev[i] = -2;
  }

  while(1)
  {
    int flag = 0;
    pthread_mutex_lock(mptr);
    count = 0;
    
    for( int i= 0; i<n; i++)
    {
      if( workers[i].status != prev[i])
      {
        // flag = 1;
        if(flag==0)
        {
          cout<<"REPORT-------------------------"<<endl;        
          flag=1;
        }
        switch(workers[i].type)
        {
          case 0: cout<<" Producer";break;
          case 1: cout <<" Consumer";break;
        }
        cout << " : "<<i<<" Context Switched from ";
        switch(prev[i])
        {
          case -1:  cout<<"Terminated";break;
          case 0: cout<<"Sleeping";break;
          case 1: cout<<"AWake";break;
          case -2: cout<<"Initiation";break;
        }
        cout<<" to ";

        switch(workers[i].status)
        {
          case -1:  cout<<"Terminated";break;
          case 0: cout<<"Sleeping";break;
          case 1: cout<<"AWake";break;
          case -2: cout<<"Initiation";break;
        }

        cout<<". Items in buffer: "<<size<< endl;

      }
      if(workers[i].status == -1)count++;
      prev[i]=workers[i].status;
    }

    pthread_mutex_unlock(mptr);
    if ( count >= n)break;

  }
  cout<<"All workers terminated. Items in buffer :"<<size<<endl;
  pthread_exit(0);



}

void* sroutine(void* args)
{
  sleep(0.5);

  int turn = 0,count = 0;

  while(1)
  {
      pthread_mutex_lock(mptr);
      if( workers[turn].status != -1)
      {
        count = 0;
        workers[turn].status = 1; 
        pthread_kill(workers[turn].id, SIGUSR1);
        pthread_mutex_unlock(mptr);

        sleep(WAITTIME);

        pthread_mutex_lock(mptr);
        if( workers[turn].status != -1)
        {
          workers[turn].status = 0;  
          pthread_kill(workers[turn].id, SIGUSR2);

        }
        pthread_mutex_unlock(mptr);

      }

      else
      {
        count++;
        pthread_mutex_unlock(mptr);
      }

      turn = (turn+1)%n;
    
      if( count >= n)break;
      sleep(.01);
  }
  cout<<" scheduler terminated"<<endl;
  pthread_exit(0);

}

