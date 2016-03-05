#include <iostream>
#include <mpi.h>


#include "squirrel-functions.h"

using namespace std;

int main(){

  cout << "hello world\n";
  cout << getCellFromPosition(1.524, 1.234) << "\n";
  long l = 12345L;


  MPI_Init(NULL,NULL);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  int size;
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  cout << "hello from " << rank << " of " << size << "\n";

  if(rank == 0){
    int buf = 4;
    MPI_Ssend(&buf,1,MPI_INT,1,0,MPI_COMM_WORLD);
  }
  if(rank == 1){
    int recv;
    MPI_Recv(&recv,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    cout << "rank " << rank << " recieved " << recv << "\n";
  }

  MPI_Finalize();
}
