#include <mpi.h>
#include "actor.hpp"

Actor::Actor(){
  MPI_Comm_rank(MPI_COMM_WORLD,&m_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&m_size);
}


void Actor::act(){
  //cout << "acting on rank " << m_rank << endl;
}
