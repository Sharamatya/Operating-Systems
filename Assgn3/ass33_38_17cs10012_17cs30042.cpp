#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include<bits/stdc++.h>
#define MAX_CHAR 100
using namespace std;
const int  MAX_SIZE = 8;



// struct ComparePriority { 
//     bool operator()(process const& p1, process const& p2) 
//     { 
//         return p1.priority < p2.priority; 
//     } 
// }; 

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
} node;

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
               while(total->turn !=i )
                  usleep(100000);
               if((total->job_created) < jobs)
               {
                  process job = make_job(getpid(),i);
                  // cout<<"Job created by "<<i<<" priority "<<job.priority<<" "<<job.jobID<<endl;
                  usleep(rand()%3000000);
                  while((total->size)>=MAX_SIZE)
                  {
                     cout<<"Queue is full : Waiting"<<endl;
                     usleep(1000000);
                  }
                  insert(job,total);
                  (total->job_created)++;
                  cout<<"                        Producer: "<<i<<" PID: "<<getpid()<<" Priority: "<<(job.priority)<<" Job ID: "<<job.jobID<<" Compute time: "<<job.comp_time<<endl;
               }

               else
               {
                  total->turn=(total->turn+1)%np;
                  cout<<"Exited Producer "<<i<<endl;
                  exit(0);
               }
               total->turn=(total->turn+1)%np;
            }
            
         }
         else
         {
            while(1)
            {   
              while(total->turn2 != i-np)
                  usleep(1000000);
              if(total->job_completed <jobs)
              {
                  if(total->job_completed>jobs)
                  {
                     cout<<"Exited Consumer "<<i-np<<endl;
                     total->turn2=(total->turn2+1)%nc;
                     exit(0);
                  }
                  
                  while(total->size<=0)
                  {
                     cout<<"Queue is empty : Waiting"<<endl;
                     usleep(100000);
                  }
                  process job = pop(total);
                  (total->job_completed)++;
                  cout<<" Consumer: "<<i-np<<" PID: "<<getpid()<<" Producer: "<<job.prod_num<<" PID: "<<job.procID<<" Priority: "<<(job.priority)<<" Job ID: "<<job.jobID<<" Compute time: "<<job.comp_time<<endl;
                  usleep(job.comp_time*1000000);
              }
              else
              {
                  cout<<"Exited Consumer "<<i-np<<endl;
                  total->turn2=(total->turn2+1)%nc;
                  exit(0);
              }
              total->turn2=(total->turn2+1)%nc;
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