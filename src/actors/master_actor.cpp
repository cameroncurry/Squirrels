#include <mpi.h>
#include <unistd.h>
#include "pool.h"
#include "master_actor.hpp"

#include <iostream>

#ifndef GRID_SHUTDOWN
#define GRID_SHUTDOWN 184
#endif
#ifndef GRID_NEW_MONTH
#define GRID_NEW_MONTH 185
#endif

MasterActor::MasterActor(int N_grids, int* grid_ranks, int N_squirrels, int* squirrel_ranks){
  //cout << "hello from master actor on rank " << m_rank << endl;
  this->N_grids = N_grids;
  this->grid_ranks = grid_ranks;
  this->N_squirrels = N_squirrels;
  this->squirrel_ranks = squirrel_ranks;

}

MasterActor::~MasterActor(){
  delete[] grid_ranks;
  delete[] squirrel_ranks;
}

void MasterActor::act(){
  //send grid to squirrels first
  distributeGridRanks();

  for(int i=0;i<10;i++){
    std::cout<< "starting month "<<i<<std::endl;
    usleep(1000000);
    advanceMonth();
  }

  //tell all squirrels to shutdown

  //then tell grid to shutdown
  for(int i=0;i<N_grids;i++){
    MPI_Send(NULL,0,MPI_INT, grid_ranks[i], GRID_SHUTDOWN, MPI_COMM_WORLD);
  }

  std::cout << " master finished counting months - exit" << std::endl;
}

//send all grid ranks to squirrels
void MasterActor::distributeGridRanks(){
  for(int i=0;i<N_squirrels;i++){
    MPI_Send(grid_ranks,N_grids, MPI_INT, squirrel_ranks[i],0, MPI_COMM_WORLD);
  }
}

void MasterActor::advanceMonth(){
  for(int i=0;i<N_grids;i++){
    MPI_Send(NULL,0,MPI_INT, grid_ranks[i],GRID_NEW_MONTH, MPI_COMM_WORLD);
  }
}
