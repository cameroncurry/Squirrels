#include <iostream>
#include <mpi.h>
#include <unistd.h>

#include "pool.h"
#include "squirrel_const.h"
#include "actor.hpp"
#include "grid_actor.hpp"
#include "master_actor.hpp"
#include "squirrel_actor.hpp"

#include "squirrel-functions.h"

using namespace std;


int main(){

  int months = 4;
  int grids = 16;
  int squirrels = 2;
  int infect_squirrels = 1;
  int max_squirrels = 10;

  MPI_Init(NULL,NULL);
  int statuscode = processPoolInit();

  if(statuscode == 2){ //master - rank 0


    int workerPid = startWorkerProcess();
    int actor_code = MASTER_ACTOR;
    MPI_Ssend(&actor_code,1,MPI_INT, workerPid,0, MPI_COMM_WORLD);

    do{
      //monitor workering/sleeping processes in background
    }while(masterPoll());
  }
  /*
   * Worker Code
   */
  else if(statuscode == 1){


    do{
      int actor_code;
      MPI_Recv(&actor_code,1,MPI_INT, MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

      Actor *a;

      if(actor_code == MASTER_ACTOR){
        MasterActor m = MasterActor(months,grids,squirrels,infect_squirrels,max_squirrels);
        a = &m;
        a->act();
      }
      else if(actor_code == GRID_ACTOR){
        GridActor g = GridActor();
        a = &g;
        a->act();
      }
      else if(actor_code == SQUIRREL_ACTOR){
        SquirrelActor s = SquirrelActor(0);
        a = &s;
        a->act();
      }
      else if(actor_code == INFECTED_SQUIRREL_ACTOR){
        SquirrelActor s = SquirrelActor(1);
        a = &s;
        a->act();
      }

    }while(workerSleep());

  }


  processPoolFinalise();
  MPI_Finalize();

}






/*
int main(){

  int grid_cells = 16;
  int squirrels = 1;
  int infect_squirrels = 0;


  MPI_Init(NULL,NULL);

  int statuscode = processPoolInit();

  Actor *a;


  if(statuscode == 2){

    //create grid cells
    int* grid_ranks = new int[grid_cells];
    for(int i=0;i<grid_cells;i++){
      int workerPid = startWorkerProcess(GRID_ACTOR);
      grid_ranks[i] = workerPid;
    }

    //create squirrels
    int* squirrel_ranks = new int[squirrels+infect_squirrels];
    for(int i=0;i<squirrels+infect_squirrels;i++){
      int workerPid;
      if(i < squirrels){
        workerPid = startWorkerProcess(SQUIRREL_ACTOR);
      }
      else {
        workerPid = startWorkerProcess(INFECTED_SQUIRREL_ACTOR);
      }
      squirrel_ranks[i] = workerPid;
    }

    MasterActor m = MasterActor(24,grid_cells, grid_ranks, (squirrels+infect_squirrels), squirrel_ranks);
    a = &m;
    a->act();
  }

  else if(statuscode == 1){ //worker
    int actor_type = getCommandData();

    if(actor_type == GRID_ACTOR){
      GridActor g = GridActor();//cout << "grid actor created" << endl;
      a = &g;
    }
    else if(actor_type == SQUIRREL_ACTOR){
      SquirrelActor s = SquirrelActor(0);
      a = &s;
    }
    else if(actor_type == INFECTED_SQUIRREL_ACTOR){
      SquirrelActor s = SquirrelActor(1);
      a = &s;
    }

    a->act();
  }


  processPoolFinalise();
  MPI_Finalize();

}
*/
