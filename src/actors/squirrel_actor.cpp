#include <mpi.h>
#include "squirrel_actor.hpp"
#include "squirrel-functions.h"
#include <iostream>

SquirrelActor::SquirrelActor(int infected){
  this->infected = infected;
  this->state = -1-rank;
  initialiseRNG(&state);

  //squirrels in this contructor start from random position
  squirrelStep(0.0,0.0,&x,&y,&state);

  //recieve rank of grid actor from master before starting
  MPI_Recv(&grid_rank,1,MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

}

void SquirrelActor::act(){

  //send to grid cell I am in if I'm infected/not infected
  //then receive back pop influx and infection level
  int cell = getCellFromPosition(x,y);
  //use tag to show which cell I'm stepping into
  MPI_Ssend(&infected,1, MPI_INT, grid_rank, cell, MPI_COMM_WORLD);

  float cellValues[2]; // {pop influx, infection level}
  MPI_Recv(cellValues,2,MPI_FLOAT, grid_rank, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  std::cout << "squirrel received values from cell: " << cellValues[0] << " " << cellValues[1] << std::endl;

  /*
  if(rank == 4){
    //stepping into grid 0
    int grid_rank_stepping_into = this->grid_ranks[0];

    //squirrel send to grid to signify stepping into cell
    //sends whether the squirrel is infected or not
    std::cout << "Squirrel rank 4 sending " << infected << " to grid 1" << std::endl;
    MPI_Ssend(&infected,1, MPI_INT, grid_rank_stepping_into,0, MPI_COMM_WORLD);

    //squirrel recieves back population influx and infection level of grid cell stepped in
    float gridValues[2];
    MPI_Recv(gridValues,2,MPI_FLOAT, grid_rank_stepping_into,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::cout << "Squrriel rank 4 recieved back " << gridValues[0] << " " << gridValues[1] << std::endl;
  }*/
  //MPI_Ssend()

}
