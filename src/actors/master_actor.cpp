#include <mpi.h>
#include <unistd.h>
#include "master_actor.hpp"

#include <iostream>

#ifndef GRID_SHUTDOWN
#define GRID_SHUTDOWN 184
#endif
#ifndef GRID_NEW_MONTH
#define GRID_NEW_MONTH 185
#endif

MasterActor::MasterActor(int grid_rank, int N_squirrels, int* squirrel_ranks){
  //cout << "hello from master actor on rank " << m_rank << endl;
  this->grid_rank = grid_rank;
  this->N_squirrels = N_squirrels;
  this->squirrel_ranks = squirrel_ranks;
}

MasterActor::~MasterActor(){
  delete[] squirrel_ranks;
}

void MasterActor::act(){
  //send grid rank to all squirrels
  for(int i=0;i<N_squirrels;i++){
    MPI_Ssend(&grid_rank,1, MPI_INT, squirrel_ranks[i],0, MPI_COMM_WORLD);
  }

  usleep(500000);
  MPI_Ssend(NULL,0,MPI_INT, grid_rank, GRID_NEW_MONTH, MPI_COMM_WORLD);
  usleep(500000);
  MPI_Ssend(NULL,0,MPI_INT, grid_rank, GRID_NEW_MONTH, MPI_COMM_WORLD);
  usleep(500000);
  MPI_Ssend(NULL,0,MPI_INT, grid_rank, GRID_SHUTDOWN, MPI_COMM_WORLD);
  std::cout << " master finished counting months - exit" << std::endl;

}
