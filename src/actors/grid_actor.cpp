#include <mpi.h>
#include <iostream>

#include "grid_actor.hpp"

int Cell::population_influx(){
  return pop_influx[0]+pop_influx[1]+pop_influx[2];
}

GridActor::GridActor(){
  n_cells = 16;
  month = 0;

  cell.pop_influx[0] = 0;
  cell.pop_influx[1] = 0;
  cell.pop_influx[2] = 0;
  //cells = new Cell[n_cells];

  //array of all grid cells
  gridValues = new float[n_cells*2];

  gridValues[0] = 3.5;
  gridValues[1] = 5.2;
}

GridActor::~GridActor(){
  //delete[] cells;
  delete[] gridValues;
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
        std::cout << "recieved shutdown message from master" << std::endl;
        break;
      }

    }
    else { //recieving from squirrel

      //squirrel stepped in grid tells me if it's infected
      int infected;
      MPI_Recv(&infected,1, MPI_INT, MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      std::cout << "grid received message from squirrel " <<status.MPI_SOURCE <<" with tag"<< status.MPI_TAG << std::endl;

      //I will update my stuff and send back population influx and infection level
      MPI_Ssend(gridValues,2,MPI_FLOAT, status.MPI_SOURCE,0, MPI_COMM_WORLD);
    }
  }

}

void GridActor::advanceMonth(){
  month++; //next month
  cell.pop_influx[month%3] = 0; //reset influx sum for this month
  std::cout << "grid advancing to month " << month << std::endl;

}
