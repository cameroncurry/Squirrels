#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "pool.h"
#include "squirrel_const.h"
#include "master_actor.hpp"

using namespace std;


MasterActor::MasterActor(int months, double month_time, int grids, int squirrels, int infect_squirrels, int max_squirrels){

  printf("\nStarting simulation with %d squirrels, %d of which are infected\n\n",squirrels+infect_squirrels, infect_squirrels);
  if(SQURL_LOG)printf("INIT - Master actor created on rank %d\n",rank);


  this->months = months;
  this->month_time = month_time; //real time seconds
  this->N_grids = grids;
  this->N_squirrels = 0; //these are updated in create squirrel function
  this->N_infected = 0;
  this->max_squirrels = max_squirrels;


  //initialise grid actors
  this->grid_ranks = new int[grids];
  for(int i=0;i<grids;i++){
    int workerPid = startWorkerProcess();
    grid_ranks[i] = workerPid;
    int actor_code = GRID_ACTOR;
    MPI_Ssend(&actor_code,1,MPI_INT, workerPid,0, MPI_COMM_WORLD);

    if(SQURL_LOG)printf("INIT - Master actor created grid actor on rank %d\n",workerPid);
  }

  //initialise squirrel actors
  for(int i=0;i<squirrels;i++){
    int workerPid = createNewSquirrel(SQUIRREL_ACTOR);
    if(SQURL_LOG)printf("INIT - Master actor created squirrel actor on rank %d\n",workerPid);
  }
  //initialise infected squirrel actors
  for(int i=0;i<infect_squirrels;i++){
    int workerPid = createNewSquirrel(INFECTED_SQUIRREL_ACTOR);
    N_infected++;
    if(SQURL_LOG)printf("INIT - Master actor created infected squirrel actor on rank %d\n",workerPid);
  }



}

MasterActor::~MasterActor(){
  delete[] grid_ranks;
}

void MasterActor::act(){

  for(int i=0;i<months;i++){
    //cout << "master starting month "<<i << endl;
    double start_time = MPI_Wtime();
    do{

      int flag;
      MPI_Status status;
      MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
      if(flag == 1){
        if(status.MPI_TAG == SQUIRREL_INFECTED){
          handleSquirrelInfected(status.MPI_SOURCE);
        }
        else if(status.MPI_TAG == SQUIRREL_DEATH){
          int err = handleSquirrelDeath(status.MPI_SOURCE);
          if(err != 0){
            i = err;
            break;
          }
        }
        else if(status.MPI_TAG == SQUIRREL_BIRTH){
          int err = handleSquirrelBirth(status.MPI_SOURCE);
          if(err != 0){
            i = err;
            break;
          }

        }
      }

    }while(MPI_Wtime()-start_time < month_time); //end month
    if(i==-1)break;

    advanceMonth(i);
  }


  endSimulation();
  /*
  //shutdown pool - effectively tells all squirrels to stop through process pool code
  shutdownPool();

  for(int i=0;i<N_squirrels;i++){
    MPI_Recv(NULL,0,MPI_INT, MPI_ANY_SOURCE,SQUIRREL_ENDING, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  //grid cells must be told to shutdown because they wait in a blocking receive
  shutdownGridCells();
  */

  if(SQURL_LOG){printf("INIT - Master actor on rank %d shutting down\n",rank);}
}


/*
 * Methods for handling all squirrel interactions
 */

void MasterActor::handleSquirrelInfected(int source){
  MPI_Recv(NULL,0,MPI_INT, source,SQUIRREL_INFECTED, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  N_infected++;
}

int MasterActor::handleSquirrelDeath(int source){
  MPI_Recv(NULL,0,MPI_INT, source,SQUIRREL_DEATH, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  N_squirrels--;
  N_infected--; //dead squirrel must be infected, so decrement count
  if(N_squirrels <= 0){
    printf("\nAll squrriels have died, ending simulation\n\n");
    return -1;
  }
  else{
    return 0;
  }
}

int MasterActor::handleSquirrelBirth(int source){
  float location[2];
  MPI_Recv(location,2,MPI_FLOAT, source,SQUIRREL_BIRTH, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

  int newSqurlID = createNewSquirrel(NEWBORN_SQUIRREL_ACTOR);
  if(newSqurlID != -1){ //birth was successful, send parent's coordinates
    MPI_Send(location,2,MPI_FLOAT, newSqurlID,SQUIRREL_BIRTH, MPI_COMM_WORLD);
    return 0;
  }
  else{ //not successful - kill simulation
    printf("\nMaximum Number of Squirrels Reached: %d, ending simulation\n\n",N_squirrels);
    return -1;
  }
}



/*
 * Method which creates new squirrel
 * Will return the rank of the new squirrel if successful
 * or return -1 if squirrel population is too high and didn't create squirrel
 */
int MasterActor::createNewSquirrel(int squirrel_type){
  if(N_squirrels < max_squirrels){
    int workerPid = startWorkerProcess();
    //tell new process that it will be a squirrel
    MPI_Send(&squirrel_type,1,MPI_INT, workerPid,0, MPI_COMM_WORLD);
    //now that new process knows its a squirrel, send all the grid ranks
    MPI_Send(grid_ranks,N_grids,MPI_INT, workerPid,0, MPI_COMM_WORLD);
    //increment squirrel numbers
    N_squirrels++;
    return workerPid;
  }
  else {
    return -1;
  }

}

//tell grid cells to move to next month
void MasterActor::advanceMonth(int month){

  printf("\nAfter month %d:\nLive Squirrels: %d\nOf which are infected:%d\n",month,N_squirrels,N_infected);
  printf("Population influx & Infection level for grid cells are:\n");

  for(int i=0;i<N_grids;i++){
    MPI_Send(NULL,0,MPI_INT, grid_ranks[i],GRID_NEW_MONTH, MPI_COMM_WORLD);
    int grid_data[2];
    MPI_Recv(grid_data,2,MPI_INT, grid_ranks[i],0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    //cout<< "Grid: "<<i<<" influx: "<<grid_data[0]<<" infection: "<<grid_data[1]<<endl;
    printf("Grid %d influx: %d, infection level: %d\n",i,grid_data[0],grid_data[1]);
  }
}

void MasterActor::endSimulation(){
  //shutdown process pool - effectively tells all squirrels that they should stop
  shutdownPool();

  //squirrel may still be in flight, post non blocking recieve for each squirrel still alive
  MPI_Request requests[N_squirrels];
  //MPI_Status statuses[N_squirrels];

  for(int i=0;i<N_squirrels;i++){
    MPI_Irecv(NULL,0,MPI_INT, MPI_ANY_SOURCE,SQUIRREL_ENDING, MPI_COMM_WORLD, &requests[i]);
  }


  while(testall(N_squirrels,requests) != 0){
    int flag;
    MPI_Status status;
    MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD,&flag,&status);
    //printf("probing squirrel");
    if(flag == 1){
      //printf("squirrel probed\n");
    if(status.MPI_TAG == SQUIRREL_INFECTED){
      MPI_Recv(NULL,0,MPI_INT, status.MPI_SOURCE,SQUIRREL_INFECTED, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      //handleSquirrelInfected(status.MPI_SOURCE);
    }
    else if(status.MPI_TAG == SQUIRREL_DEATH){
      MPI_Recv(NULL,0,MPI_INT, status.MPI_SOURCE,SQUIRREL_DEATH, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      //handleSquirrelDeath(status.MPI_SOURCE);

    }
    else if(status.MPI_TAG == SQUIRREL_BIRTH){
      float location[2];
      MPI_Recv(location,2,MPI_FLOAT, status.MPI_SOURCE,SQUIRREL_BIRTH, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      //handleSquirrelBirth(status.MPI_SOURCE);
    }
    }
  }
  printf("finished probing squirrels\n");
  //grid cells must be told to shutdown because they wait in a blocking receive
  shutdownGridCells();
}

void MasterActor::shutdownGridCells(){
  for(int i=0;i<N_grids;i++){
    //cout << "master shutting down grid "<<grid_ranks[i]<<endl;
    MPI_Send(NULL,0,MPI_INT, grid_ranks[i], GRID_SHUTDOWN, MPI_COMM_WORLD);
  }
}

//returns the number of requests still outstanding
int MasterActor::testall(int count, MPI_Request* request){
  int result = 0;
  for(int i=0;i<count;i++){
    int flag;
    MPI_Test(&request[i],&flag,MPI_STATUS_IGNORE);
    if(flag == 0)result++;
  }
  return result;
}
