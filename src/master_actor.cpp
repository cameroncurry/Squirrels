#include <mpi.h>
#include "master_actor.hpp"

MasterActor::MasterActor(int N_grid, int* grid_ranks, int N_squirrels, int* squirrel_ranks){
  //cout << "hello from master actor on rank " << m_rank << endl;
  this->N_grid = N_grid;
  this->grid_ranks = grid_ranks;
  this->N_squirrels = N_squirrels;
  this->squirrel_ranks = squirrel_ranks;
}

MasterActor::~MasterActor(){
  delete[] grid_ranks;
  delete[] squirrel_ranks;
}

void MasterActor::act(){
  //send grid ranks to all squirrels
  for(int i=0;i<N_squirrels;i++){
    MPI_Ssend(grid_ranks,N_grid, MPI_INT, squirrel_ranks[i],0, MPI_COMM_WORLD);
  }

}
