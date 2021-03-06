#include <mpi.h>
#include <unistd.h>

#include "pool.h"
#include "squirrel_const.h"
#include "squirrel_actor.hpp"
#include "squirrel-functions.h"

using namespace std;

SquirrelActor::SquirrelActor(int infected, double squirrel_delay){
  this->squirrel_delay = squirrel_delay;
  this->infected = infected;
  this->x=0.0;
  this->y=0.0;
  contruct();
}
SquirrelActor::SquirrelActor(int infected,float x,float y, double squirrel_delay){
  this->squirrel_delay = squirrel_delay;
  this->infected = infected;
  this->x = x;
  this->y = y;
  contruct();
}

void SquirrelActor::contruct(){

  if(SQURL_LOG)printf("INIT - Squirrel actor created on rank %d\n",rank);

  this->steps = 0;
  this->state = -1-rank;
  this->acting = 0;

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
  MPI_Probe(1,0,MPI_COMM_WORLD,&status);

  MPI_Get_count(&status,MPI_INT,&N_grids);
  this->grid_ranks = new int[N_grids];
  MPI_Recv(grid_ranks,N_grids,MPI_INT, 1,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

}

void SquirrelActor::act(){

  //to prevent dead lock, should worker stop is called before every message sending operation
  int acting = shouldWorkerStop();
  while(acting == 0){
    usleep((int)(squirrel_delay*1000000));

    acting = shouldWorkerStop();
    if(acting == 0){
      //make one step
      stepIntoCell();

      //after every step squirrel may catch disease
      if(steps > 0 && infected == 0 && shouldWorkerStop() == 0){
        int catchDisease = willCatchDisease(averageInfectionLevel(),&state);
        if(catchDisease == 1){
          infected = 1;
          infected_step = steps;
          MPI_Send(NULL,0,MPI_INT, 1,SQUIRREL_INFECTED, MPI_COMM_WORLD);
          if(SQURL_LOG)printf("COMM - squirrel %d sent infected message to master actor\n",rank);
        }
      }

      //if squirrel is infected, might die after minimum of 50 steps
      if(infected == 1 && shouldWorkerStop() == 0){
        if(steps > infected_step+50){ //it has been 50 steps
            int die = willDie(&state);
            if(die == 1){
              MPI_Send(NULL,0,MPI_INT, 1,SQUIRREL_DEATH, MPI_COMM_WORLD);
              if(SQURL_LOG)printf("COMM - squirrel %d sent dead message to master actor\n",rank);
              break;
            }
        }
      }

      //after 50 timesteps, squirrel may give birth
      if(steps > 0 && steps%50 == 0 && shouldWorkerStop() == 0){
        int birth = willGiveBirth(averagePopInflux(), &state);
        if(birth == 1){
          float location[2];
          location[0] = x;
          location[1] = y;
          MPI_Send(location,2,MPI_FLOAT, 1,SQUIRREL_BIRTH, MPI_COMM_WORLD);
          if(SQURL_LOG)printf("COMM - squirrel %d sent location of child to master actor\n",rank);
        }
      }
      steps++;
    }
    else {
      MPI_Send(NULL,0,MPI_INT, 1,SQUIRREL_ENDING, MPI_COMM_WORLD);
    }

  }


  if(SQURL_LOG)printf("INIT - Squirrel actor on rank %d shutting down\n",rank);

}

/*
 * Squirrel steps into new random cell and update pop influx and infection level
 */

void SquirrelActor::stepIntoCell(){
  //send to grid the cell I am in using tag and tell grid if I'm infected/not infected
  //then receive back pop influx and infection level
  squirrelStep(x,y,&x,&y,&state);
  int cell = getCellFromPosition(x,y);

  if(SQURL_LOG)printf("COMM - squirrel %d sending to grid %d\n",rank,grid_ranks[cell]);

  MPI_Ssend(&infected,1, MPI_INT, grid_ranks[cell], cell, MPI_COMM_WORLD);

  int cellValues[2];
  MPI_Recv(cellValues,2,MPI_INT, grid_ranks[cell], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  if(SQURL_LOG)printf("COMM - squirrel %d received %d, %d from grid %d\n",rank,cellValues[0],cellValues[1],grid_ranks[cell]);

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
