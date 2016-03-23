#include <mpi.h>
#include <unistd.h>
#include "pool.h"
#include "squirrel_const.h"
#include "master_actor.hpp"

#include <iostream>

using namespace std;


MasterActor::MasterActor(int months, int grids, int squirrels, int infect_squirrels){
  this->months = months;
  this->N_grids = grids;
  this->N_squirrels = (squirrels+infect_squirrels);

  //initialise grid actors
  this->grid_ranks = new int[grids];
  for(int i=0;i<grids;i++){
    int workerPid = startWorkerProcess();
    grid_ranks[i] = workerPid;
    int actor_code = GRID_ACTOR;
    MPI_Send(&actor_code,1,MPI_INT, workerPid,0, MPI_COMM_WORLD);
  }

  //initialise squirrel actors
  for(int i=0;i<2;i++){
    int workerPid = startWorkerProcess();
    //send to new process tell it that it will be a squirrel
    int actor_code = SQUIRREL_ACTOR;
    MPI_Send(&actor_code,1,MPI_INT, workerPid,0, MPI_COMM_WORLD);

    //now that new process knows its a squirrel, send all the grid ranks
    MPI_Send(grid_ranks,N_grids,MPI_INT, workerPid,0, MPI_COMM_WORLD);
  }



}

MasterActor::~MasterActor(){
  delete[] grid_ranks;
  //delete[] squirrel_ranks;
}

void MasterActor::act(){
  //send grid to squirrels first
  //distributeGridRanks();

  for(int i=0;i<months;i++){
    cout << "master starting month "<<i << endl;
    usleep(50000);
    //cout << "master ending month 0" << endl;
  }

  createNewSquirrel();
  sleep(1);


  cout << "master actor ending month, telling grid to shutdown"<<endl;

  //shutdown effectively tells all squirrels to stop
  shutdownPool();
  //grid cells must be told to shutdown because they wait in a blocking receive
  shutdownGridCells();

  /*
  //change to months
  for(int i=0;i<4;i++){
    std::cout<< "starting month "<<i<<std::endl;
    double month_start_time = MPI_Wtime();

    do{
      MPI_Status status;
      int flag;
      MPI_Iprobe(MPI_ANY_SOURCE,SQUIRREL_BIRTH_TAG,MPI_COMM_WORLD,&flag,&status);
      if(flag == 1){
        createNewSquirrel(status.MPI_SOURCE);

      }
    }while(MPI_Wtime() - month_start_time < month_time);

    //advanceMonth();
  }
  */
  /*
  //tell grid to shutdown
  for(int i=0;i<N_grids;i++){
    MPI_Send(NULL,0,MPI_INT, grid_ranks[i], GRID_SHUTDOWN, MPI_COMM_WORLD);
  }

  std::cout << " master finished counting months - exit" << std::endl;*/
}

void MasterActor::shutdownGridCells(){
  for(int i=0;i<N_grids;i++){
    //cout << "master shutting down grid "<<grid_ranks[i]<<endl;
    MPI_Send(NULL,0,MPI_INT, grid_ranks[i], GRID_SHUTDOWN, MPI_COMM_WORLD);
  }
}

void MasterActor::createNewSquirrel(){
  int workerPid = startWorkerProcess();
  //send to new process tell it that it will be a squirrel
  int actor_code = SQUIRREL_ACTOR;
  MPI_Send(&actor_code,1,MPI_INT, workerPid,0, MPI_COMM_WORLD);

  //now that new process knows its a squirrel, send all the grid ranks
  MPI_Send(grid_ranks,N_grids,MPI_INT, workerPid,0, MPI_COMM_WORLD);
}




//send all grid ranks to squirrels
/*
void MasterActor::distributeGridRanks(){

  for(int i=0;i<N_squirrels;i++){
    MPI_Send(grid_ranks,N_grids, MPI_INT, squirrel_ranks[i],0, MPI_COMM_WORLD);
  }
}*/

/*
void MasterActor::advanceMonth(){

  for(int i=0;i<N_grids;i++){
    MPI_Send(NULL,0,MPI_INT, grid_ranks[i],GRID_NEW_MONTH, MPI_COMM_WORLD);
  }
}*/

/*
void MasterActor::createNewSquirrel(int source){
  float squirrelLoc[2];
  MPI_Recv(squirrelLoc,2, MPI_FLOAT, source,SQUIRREL_BIRTH_TAG, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  N_squirrels++;
  if(N_squirrels <= squirrel_max){
    //int newSquirrelid = startWorkerProcess(SQUIRREL_ACTOR);
    //MPI_Send(grid_ranks,N_grids,MPI_INT,newSquirrelid,0, MPI_COMM_WORLD);
  }
}*/
