#include <mpi.h>
#include <stdio.h>

#include "pool.h"
#include "squirrel_const.h"
#include "grid_actor.hpp"

using namespace std;

GridActor::GridActor(){

  if(SQURL_LOG)printf("INIT - Grid actor created on rank %d\n",rank);

  month = 0;
  waitingForMessages = 1;

  current_month_influx = 0;
  current_month_infection = 0;

  //initialise influx and infection level
  pop_influx[0] = 0;
  pop_influx[1] = 0;
  pop_influx[2] = 0;
  infect_level[0] = 0;
  infect_level[1] = 0;
}

void GridActor::act(){

  //wait for squirrels or master actor to send a message
  while(waitingForMessages){

    MPI_Status status;
    MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

    if(status.MPI_SOURCE == 1){
      handleMasterMessage();
    }
    else{
      handleSqurrielMessage(status.MPI_SOURCE);
    }

  }

  if(SQURL_LOG)printf("INIT - Grid actor on rank %d shutting down\n",rank);
}

void GridActor::handleMasterMessage(){
  MPI_Status status;
  MPI_Recv(NULL,0,MPI_INT, 1,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
  if(status.MPI_TAG == GRID_NEW_MONTH){
    advanceMonth();
    if(SQURL_LOG)printf("COMM - Grid actor on rank %d received advance month message from master actor\n",rank);
  }
  else if(status.MPI_TAG == GRID_SHUTDOWN){
    waitingForMessages = 0;
    if(SQURL_LOG)printf("COMM - Grid actor on rank %d received shutdown message from master actor\n",rank);
  }
}

void GridActor::advanceMonth(){
  //reset influx and infection level for current month and update old months
  pop_influx[month%3] = current_month_influx;
  infect_level[month%2] = current_month_infection;
  current_month_influx = 0;
  current_month_infection = 0;
  month++;

  //send influx and infect level to master for output after update of previous month
  int data[2];
  data[0] = pop_influx[(month-1)%3];
  data[1] = infect_level[(month-1)%2];
  MPI_Send(data,2,MPI_INT, 1,0, MPI_COMM_WORLD);
  if(SQURL_LOG)printf("COMM - Grid actor on rank %d sent %d %d to master actor\n",rank,data[0],data[1]);
}

void GridActor::handleSqurrielMessage(int source){

  int infected;
  MPI_Recv(&infected,1,MPI_INT, source,MPI_ANY_TAG, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

  int cellValues[2];
  cellValues[0] = populationInflux();
  cellValues[1] = infectionLevel();
  MPI_Send(cellValues,2,MPI_INT, source,0, MPI_COMM_WORLD);

  //increment influx and infection level after sending back to squirrel
  current_month_influx++;
  current_month_infection += infected;
}

int GridActor::populationInflux(){
  return pop_influx[0]+pop_influx[1]+pop_influx[2];
}

int GridActor::infectionLevel(){
  return infect_level[0]+infect_level[1];
}
