#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include "pool.h"
#include "squirrel_const.h"
#include "grid_actor.hpp"

using namespace std;

GridActor::GridActor(){
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

}

void GridActor::advanceMonth(){
  month++;
  cout << "grid "<<rank<<" advancing to month "<<month<<endl;
  //reset influx and infection level for current month
  pop_influx[month%3] = 0;
  infect_level[month%2] = 0;
}

void GridActor::handleMasterMessage(){
  //master will only send null messages with tags
  MPI_Status status;
  MPI_Recv(NULL,0,MPI_INT, 1,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
  if(status.MPI_TAG == GRID_NEW_MONTH){
    advanceMonth();
  }
  else if(status.MPI_TAG == GRID_SHUTDOWN){ //end of simultion, break out of recieve loop
    //std::cout << "GRID "<<rank<<" SHUTDOWN" <<std::endl;
    waitingForMessages = 0;
  }
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
 * Might not need this keep here anyway
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
  return pop_influx[0]+pop_influx[1]+pop_influx[2];
}
int GridActor::infectionLevel(){
  return infect_level[0]+infect_level[1];
}
