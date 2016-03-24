#include <mpi.h>
#include <unistd.h>
#include "pool.h"
#include "squirrel_const.h"
#include "master_actor.hpp"

#include <iostream>

using namespace std;


MasterActor::MasterActor(int months, int grids, int squirrels, int infect_squirrels, int max_squirrels){
  this->months = months;
  this->month_time = 1.0; //real time seconds
  this->N_grids = grids;
  this->N_squirrels = 0;
  this->max_squirrels = max_squirrels;


  //initialise grid actors
  this->grid_ranks = new int[grids];
  for(int i=0;i<grids;i++){
    int workerPid = startWorkerProcess();
    grid_ranks[i] = workerPid;
    int actor_code = GRID_ACTOR;
    MPI_Ssend(&actor_code,1,MPI_INT, workerPid,0, MPI_COMM_WORLD);
  }

  //initialise squirrel actors
  for(int i=0;i<squirrels;i++){
    createNewSquirrel(SQUIRREL_ACTOR);
  }
  //initialise infected squirrel actors
  for(int i=0;i<infect_squirrels;i++){
    createNewSquirrel(INFECTED_SQUIRREL_ACTOR);
  }



}

MasterActor::~MasterActor(){
  delete[] grid_ranks;
  //delete[] squirrel_ranks;
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
        if(status.MPI_TAG == SQUIRREL_DEATH){
          MPI_Recv(NULL,0,MPI_INT, status.MPI_SOURCE,SQUIRREL_DEATH, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
          N_squirrels--;
          //cout << "master recieved that squirrel "<<status.MPI_SOURCE<<" died"<<endl;
        }
        else if(status.MPI_TAG == SQUIRREL_BIRTH){
          float location[2];
          MPI_Recv(location,2,MPI_FLOAT, status.MPI_SOURCE,SQUIRREL_BIRTH, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
          createNewSquirrel(SQUIRREL_ACTOR);
        }
      }

    }while(MPI_Wtime()-start_time < month_time); //end month
    advanceMonth(i);
  }

  cout << "master actor ending month, telling grid to shutdown"<<endl;
  //shutdown pool - effectively tells all squirrels to stop
  shutdownPool();
  //grid cells must be told to shutdown because they wait in a blocking receive
  shutdownGridCells();
  checkSquirrels();
}



void MasterActor::createNewSquirrel(int squirrel_type){
  if(N_squirrels < max_squirrels){
    int workerPid = startWorkerProcess();
    //tell new process that it will be a squirrel
    MPI_Send(&squirrel_type,1,MPI_INT, workerPid,0, MPI_COMM_WORLD);
    //now that new process knows its a squirrel, send all the grid ranks
    MPI_Send(grid_ranks,N_grids,MPI_INT, workerPid,0, MPI_COMM_WORLD);
    //increment squirrel numbers
    N_squirrels++;
  }
  else {
    cout << "too many squirrels"<<endl;
  }

}

//tell grid cells to move to next month
void MasterActor::advanceMonth(int month){
  cout << "For month "<<month<<" population influx & infection level are:"<<endl;
  for(int i=0;i<N_grids;i++){
    MPI_Send(NULL,0,MPI_INT, grid_ranks[i],GRID_NEW_MONTH, MPI_COMM_WORLD);
    int grid_data[2];
    MPI_Recv(grid_data,2,MPI_INT, grid_ranks[i],0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    cout<< "Grid: "<<i<<" influx: "<<grid_data[0]<<" infection: "<<grid_data[1]<<endl;
  }
}

void MasterActor::shutdownGridCells(){
  for(int i=0;i<N_grids;i++){
    //cout << "master shutting down grid "<<grid_ranks[i]<<endl;
    MPI_Send(NULL,0,MPI_INT, grid_ranks[i], GRID_SHUTDOWN, MPI_COMM_WORLD);
  }
}

void MasterActor::checkSquirrels(){
  int flag;
  MPI_Status status;
  MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_SOURCE,MPI_COMM_WORLD,&flag,&status);

  if(flag == 1){
    cout << "squirrel "<<status.MPI_SOURCE<<" waiting"<<endl;
  }
  else{
    cout << "no squirrels waiting for master "<<endl;
  }

}
