#include <mpi.h>
#include <iostream>

#include "grid_actor.hpp"

int Cell::population_influx(){
  return pop_influx[0]+pop_influx[1]+pop_influx[2];
}
int Cell::infection_level(){
  return inft_level[0]+inft_level[1];
}

GridActor::GridActor(){
  month = 0;
  initialiseCells();
}

void GridActor::act(){

  //wait for either squirrels or master to send message
  MPI_Status status;
  while(true){
    MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

    if(status.MPI_SOURCE == 0){ //receiving from master

      //master will only send null messages with tags
      MPI_Recv(NULL,0,MPI_INT, 0,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
      if(status.MPI_TAG == GRID_NEW_MONTH){
        advanceMonth();
      }
      else if(status.MPI_TAG == GRID_SHUTDOWN){ //end of simultion, break out of recieve loop
        break;
      }

    }
    else { //recieving from squirrel
      handleSqurrielMessage();
    }
  }

}

void GridActor::initialiseCells(){
  for(int i=0;i<n_cells;i++){
    cells[i].pop_influx[0] = 0;
    cells[i].pop_influx[1] = 0;
    cells[i].pop_influx[2] = 0;
    cells[i].inft_level[0] = 0;
    cells[i].inft_level[1] = 0;
  }
}


void GridActor::advanceMonth(){

  std::cout << "After Month: " << month << "\npopulation influx & infection level are:" << std::endl;
  for(int i=0;i<n_cells;i++){
    std::cout << "Cell " << i << ": " << cells[i].population_influx() << " " << cells[i].infection_level() << std::endl;
  }

  month++; //next month
  for(int i=0;i<n_cells;i++){
    //reset influx and infection level for this month
    cells[i].pop_influx[month%3] = 0;
    cells[i].inft_level[month%2] = 0;
  }

}

void GridActor::handleSqurrielMessage(){
  MPI_Status status;

  //squirrel sends its infected state and cell it has stepped in as tag
  int infected;
  MPI_Recv(&infected,1, MPI_INT, MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  int squirrel_cell = status.MPI_TAG;

  //find population influx and infection of appropriate cell and send back to squirrel
  int cellValues[2];
  cellValues[0] = cells[squirrel_cell].population_influx();
  cellValues[1] = cells[squirrel_cell].infection_level();
  MPI_Ssend(cellValues,2,MPI_INT, status.MPI_SOURCE,0, MPI_COMM_WORLD);

  //increase influx of appropriate cell
  cells[status.MPI_TAG].pop_influx[month%3]++;
  //infected is either 0 or 1 (1 represents infected), so simply add
  cells[status.MPI_TAG].inft_level[month%2] += infected;
}
