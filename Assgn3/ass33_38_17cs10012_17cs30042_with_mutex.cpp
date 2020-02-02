#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_CHAR 100
using namespace std;
const int  MAX_SIZE = 8;

#ifndef _POSIX_THREAD_PROCESS_SHARED
#error This system does not support process shared mutex
#endif

typedef struct process{
    int procID;
    int prod_num;
    int priority;
    int comp_time;
    int jobID;
    // struct process* l;
} process;


process make_job(int pid,int prod)
{
   process job;
   job.procID = pid;
   job.prod_num = prod;
   srand(time(0)*pid);
   job.priority =  rand()%10+1; 
   job.comp_time = rand()%4+1;
   job.jobID = rand()%100000+1;
   return job;
}

typedef struct total{
   int job_completed;
   int job_created;
   int turn;
   int turn2;
   int size;
   int aux[MAX_SIZE];
   process pq[MAX_SIZE]; 
   pthread_mutex_t   mutex;
} node;

pthread_mutex_t    *mptr; //Mutex Pointer
pthread_mutexattr_t matr; //Mutex Attribute

void insert(process job,node* total)
{
   int i;

   for(i=0;i<MAX_SIZE;i++)
   {
      if((total->aux) [i]==-1)
      {
         (total->pq)[i] = job;
         (total->size)++;
         total->aux[i] = job.priority;
         break;
      }
   }
}
process pop(node* total)
{
   int i,mx=0;
   process job;
   for(i=0;i<MAX_SIZE;i++)
   {
      mx=max(mx,total->aux[i]);
   }
   for(i=0;i<MAX_SIZE;i++)
   {
      if((total->aux)[i]==mx)
      {
         (total->aux)[i] = -1;
         job = (total->pq)[i];
         (total->size)--;
         break;
      }
   }
   return job;
}



int main()
{
   int i,j,nc,np,m,k,jobs;
   cin>>np>>nc>>jobs;
   key_t key = ftok("shmfile",65);
    int shmid = shmget(key,1024,0666|IPC_CREAT);
    node * total=(node *) shmat(shmid, (void*)0,0); 
    total->job_created=0;
    total->job_completed = 0;
    total->turn = 0;
    total->turn2 = 0;
    total->size = 0;

    mptr = &(total->mutex);
    int rtn;  // retrn value
    // Setup Mutex
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

    for(i=0;i<MAX_SIZE;i++)
      total->aux[i] = -1;
    time_t initial = time(NULL); 

    for(i=0;i<nc+np;i++)
    {
      if(fork()==0)
      {
         if(i<np)
         {
            while(1)
            {

               // while(total->turn !=i )
                  // usleep(100000);
              process job = make_job(getpid(),i);
              // cout<<"                        [Created]Producer: "<<i<<" PID: "<<getpid()<<" Priority: "<<(job.priority)<<" Job ID: "<<job.jobID<<" Compute time: "<<job.comp_time<<endl;
              usleep(rand()%3000000);
              
              pthread_mutex_lock( mptr );
              // cout <<"Produce "<<i<<" locked"<<endl;
              if((total->job_created) < jobs && total->size < 7)
               {
                  
                  // cout<<"Job created by "<<i<<" priority "<<job.priority<<" "<<job.jobID<<endl;
                  
                  // while((total->size)>=MAX_SIZE)
                  // {
                  //    cout<<"Producer "<<i<<" Queue is full : Waiting"<<endl;
                  //    pthread_mutex_unlock( mptr );
                  //    usleep(1000000);
                  // }
                  insert(job,total);
                  (total->job_created)++;
                  cout<<"                                 Producer: "<<i<<" PID: "<<getpid()<<" Priority: "<<(job.priority)<<" Job ID: "<<job.jobID<<" Compute time: "<<job.comp_time<<"      "<<"Jobs Completed :"<<(total->job_completed)<<" Jobs Created :"<< total->job_created << endl;
                  pthread_mutex_unlock( mptr );
                  // cout <<"Produce "<<i<<" unlocked"<<endl;
               }

               else if (total->size >=7 ){
                     cout<<"Producer "<<i<<" Queue is full : Waiting"<<endl;
                     pthread_mutex_unlock( mptr );
                     usleep(1000000);

               }
               else
               {
                  // total->turn=(total->turn+1)%np;
                  pthread_mutex_unlock( mptr );
                  // cout <<"Produce "<<i<<" unlocked"<<endl;
                  cout<<"Exited Producer "<<i<<endl;
                  exit(0);
               }


               
               // total->turn=(total->turn+1)%np;
            }
            
         }
         else
         {
            while(1)
            {   
              // while(total->turn2 != i-np)
                  // usleep(1000000);

              

                // while(total->size<=0)
                // {
                //    cout<<"Consumer: "<<i-np<<" Queue is empty : Waiting"<<endl;
                //    // cout <<"Consumer "<<i-np<<" unlocked"<<endl;
                //    // pthread_mutex_unlock( mptr );
                //    usleep(100000);
                //    // continue; 
                    

                // }

              pthread_mutex_lock( mptr );
              // cout <<"Consumer "<<i-np<<" locked"<<endl;
              if(total->job_completed <jobs && total->size>0)
              {
                  if(total->job_completed>jobs)
                  {
                     cout<<"Exited Consumer "<<i-np<<endl;
                     // total->turn2=(total->turn2+1)%nc;
                     exit(0);
                  }
                  
                  // while(total->size<=0)
                  // {
                  //    cout<<"Consumer: "<<i-np<<" Queue is empty : Waiting"<<endl;
                  //    // cout <<"Consumer "<<i-np<<" unlocked"<<endl;
                  //    pthread_mutex_unlock( mptr );
                  //    usleep(100000);
                  //    // continue; 
                      

                  // }
                  process job = pop(total);
                  (total->job_completed)++;
                  cout<<"          Consumer: "<<i-np<<" PID: "<<getpid()<<" Producer: "<<job.prod_num<<" PID: "<<job.procID<<" Priority: "<<(job.priority)<<" Job ID: "<<job.jobID<<" Compute time: "<<job.comp_time<<"      "<<"Jobs Completed :"<<(total->job_completed)<<" Jobs Created :"<< total->job_created<<endl;
                  // cout <<"Consumer "<<i-np<<" unlocked"<<endl;
                  pthread_mutex_unlock( mptr );
                  usleep(job.comp_time*1000000);
              }
              else if ( total->job_completed >=jobs )
              {
                  cout<<"Exited Consumer "<<i-np<<endl;
                  // cout <<"Consumer "<<i-np<<" unlocked"<<endl;
                  pthread_mutex_unlock( mptr );

                  // total->turn2=(total->turn2+1)%nc;
                  exit(0);
              }

              else
              {
                  pthread_mutex_unlock( mptr );
                  cout<<"Consumer: "<<i-np<<" Queue is empty : Waiting"<<endl;
                  usleep(1000000);          
              }

              
              // total->turn2=(total->turn2+1)  %nc;
            }
         }
      }
    }
   for(i=0;i<nc+np;i++)
   {
      wait(NULL);
   }
   time_t final = time(NULL); 
   cout<<"Total Time elapsed: "<<final-initial<<endl;
   shmdt(total);  
   shmctl(shmid,IPC_RMID,NULL);
   exit(1); 
}
