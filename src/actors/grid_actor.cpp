#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include "pool.h"
#include "squirrel_const.h"
#include "grid_actor.hpp"

using namespace std;

GridActor::GridActor(){

  if(SQURL_LOG)printf("INIT - Grid actor created on rank %d\n",rank);

  month = 0;
  waitingForMessages = 1;

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
    int flag;
    MPI_Status status;
    MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);

    if(flag == 1){
      if(status.MPI_SOURCE == 1){
        handleMasterMessage();
      }
      else{
        handleSqurrielMessage(status.MPI_SOURCE);
      }

    }
  }

  //cout << "grid "<<rank<<"shutting down"<<endl;
  gridShutdown();
  int flag;
  MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,MPI_STATUS_IGNORE);
  printf("grid %d finished with flag %d\n",rank,flag);
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
  //send back to master influx and infect level for output
  int data[2];
  data[0] = pop_influx[month%3];
  data[1] = infect_level[month%2];
  MPI_Send(data,2,MPI_INT, 1,0, MPI_COMM_WORLD);
  if(SQURL_LOG)printf("COMM - Grid actor on rank %d sent %d %d to master actor\n",rank,data[0],data[1]);

  //reset influx and infection level for current month
  month++;
  pop_influx[month%3] = 0;
  infect_level[month%2] = 0;
}

void GridActor::handleSqurrielMessage(int source){

  int infected;
  MPI_Recv(&infected,1,MPI_INT, source,MPI_ANY_TAG, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

  int cellValues[2];
  cellValues[0] = populationInflux();
  cellValues[1] = infectionLevel();
  MPI_Send(cellValues,2,MPI_INT, source,0, MPI_COMM_WORLD);

  //increment influx and infection level after sending back to squirrel
  pop_influx[month%3]++;
  infect_level[month%2] += infected;
  //std::cout<<"grid rank "<<rank<<" received from "<<source<<std::endl;
  //std::cout<<"grid rank "<<rank<<" influx: "<<populationInflux()<<" infection: "<<infectionLevel()<<std::endl;
}

/*
 * Squirrel messages may be in flight when grid is told to shutdown
 * handle these messages before shutting down to avoid deadlock
 */
void GridActor::gridShutdown(){
  int flag;
  do{
    MPI_Status status;
    MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
    if(flag == 1){
      if(status.MPI_SOURCE != 0){
        std::cout << "grid "<<rank<<" shutting down with flag "<<flag<<" from source " << status.MPI_SOURCE<<std::endl;
        handleSqurrielMessage(status.MPI_SOURCE);
      }
      else {
        //std::cout << "grid "<<rank<<" source 0 tag: "<<status.MPI_SOURCE<<std::endl;
        flag = 0;
      }
    }
  }while(flag == 1);
}


int GridActor::populationInflux(){
  if(month == 0)return 0;
  else if(month == 1)return pop_influx[0];
  else if(month == 2)return pop_influx[0]+pop_influx[1];
  else{
    return pop_influx[0]+pop_influx[1]+pop_influx[2];
  }
}

int GridActor::infectionLevel(){
  if(month == 0)return 0;
  else if(month == 1)return infect_level[0];
  else{
    return infect_level[0]+infect_level[1];
  }
}
