#include <mpi.h>
#include "actor.hpp"

Actor::Actor(){
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
}


void Actor::act(){
  return;
}
