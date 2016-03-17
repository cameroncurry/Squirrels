#include <iostream>
#include <mpi.h>

#include "pool.h"
#include "actor.hpp"
#include "grid_actor.hpp"
#include "master_actor.hpp"
#include "squirrel_actor.hpp"

#include "squirrel-functions.h"

using namespace std;


/*
  each process is inited as selected actor
  squirrel sends its infected status to grid
  grid sends back pop influx & infected level

 TODO:
    birth squirrels
    clock
*/

#define GRID_ACTOR 0
#define SQUIRREL_ACTOR 1
#define INFECTED_SQUIRREL_ACTOR 2

int main(){

  int grid_cells = 16;
  int squirrels = 8;
  int infect_squirrels = 4;

  /*
   * Create actors
   */

  MPI_Init(NULL,NULL);

  int statuscode = processPoolInit();

  Actor *a;

  /*
   * Master Allocates actors
   */
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

    MasterActor m = MasterActor(grid_cells, grid_ranks, (squirrels+infect_squirrels), squirrel_ranks);
    a = &m;
    a->act();
  }
  /*
   * Workers create themselves as actor type
   */
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
