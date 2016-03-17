#include <mpi.h>
#include "pool.h"
#include "squirrel_actor.hpp"
#include "squirrel-functions.h"
#include <iostream>
#include <unistd.h>

SquirrelActor::SquirrelActor(int infected){
  this->infected =infected;
  this->steps = 0;
  this->state = -1-rank;
  this->acting = 0;
  this->x=0.0;
  this->y=0.0;

  for(int i=0;i<50;i++){
    pop_influxes[i] = 0;
    infect_levels[i] = 0;
  }

  initialiseRNG(&state);
  waitForGridRanks();
}

SquirrelActor::~SquirrelActor(){
  delete[] grid_ranks;
}

void SquirrelActor::waitForGridRanks(){
  //receive dynamic amount of grid cells
  MPI_Status status;
  MPI_Probe(0,0,MPI_COMM_WORLD,&status);

  MPI_Get_count(&status,MPI_INT,&N_grids);
  this->grid_ranks = new int[N_grids];
  MPI_Recv(grid_ranks,N_grids,MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

}

void SquirrelActor::act(){


  while(acting == 0){
    //usleep(50000);
    //std::cout << "squirrel " << rank << " acting" << std::endl;
    acting = shouldWorkerStop();

    if(acting == 0){
      //step into next cell to update influx and infection level
      stepIntoCell();

      //after every step squirrel may catch disease
      if(steps > 0){
        int catchDisease = willCatchDisease(averageInfectionLevel(),&state);
        if(catchDisease == 1){
          this->infected = 1;
          this->infected_step = steps;
          //std::cout << "squirrel "<<rank<<" will catch disease"<<std::endl;
        }
      }

      if(infected == 1){ //if squirrel is infected, might die after minimum of 50 steps
        if(steps > infected_step+50){ //it has been 50 steps
            int die = willDie(&state);
        }
      }

      //after 50 timesteps, squirrel may give birth
      if(steps > 0 && steps%50 == 0){
        int birth = willGiveBirth(averagePopInflux(), &state);
        //std::cout << "squirrel "<<rank<< " will give birth " << birth << std::endl;
      }
      steps++;
    }
  }
  //std::cout << "squirrel " << rank << " shutting down" << std::endl;
}

/*
 * Squirrel steps into new random cell and update pop influx and infection level
 */

void SquirrelActor::stepIntoCell(){
  //send to grid the cell I am in using tag and tell grid if I'm infected/not infected
  //then receive back pop influx and infection level
  squirrelStep(x,y,&x,&y,&state);
  int cell = getCellFromPosition(x,y);
  //std::cout <<"squirrel "<<rank<<" stepped into cell "<<cell<<" sending to "<<grid_ranks[cell]<<std::endl;
  MPI_Ssend(&infected,1, MPI_INT, grid_ranks[cell], cell, MPI_COMM_WORLD);

  int cellValues[2];
  MPI_Recv(cellValues,2,MPI_INT, grid_ranks[cell], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  //std::cout << "squirrel "<<rank<<" received "<<cellValues[0]<<" "<<cellValues[1] << std::endl;

  pop_influxes[steps%50] = cellValues[0];
  infect_levels[steps%50] = cellValues[1];

}

float SquirrelActor::averagePopInflux(){
  int n = 50;
  if(steps < 50) n = steps;

  int sum = 0;
  for(int i=0;i<n;i++){
    sum += pop_influxes[i];
  }
  return (float)sum / (float)n;
}

float SquirrelActor::averageInfectionLevel(){
  int n = 50;
  if(steps < 50)n = steps;

  int sum = 0;
  for(int i=0;i<n;i++){
    sum += infect_levels[i];
  }
  return (float)sum / (float)n;
}
