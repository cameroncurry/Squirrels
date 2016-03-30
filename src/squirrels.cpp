#include <mpi.h>

#include "pool.h"
#include "squirrel_const.h"
#include "actor.hpp"
#include "grid_actor.hpp"
#include "master_actor.hpp"
#include "squirrel_actor.hpp"

#include "squirrel-functions.h"

using namespace std;


int main(){

  int months = 24;
  int grids = 16;
  int squirrels = 30;
  int infect_squirrels = 4;
  int max_squirrels = 180;

  double month_delay = 0.5; //seconds
  double squirrel_delay = 0.01;

  MPI_Init(NULL,NULL);
  int statuscode = processPoolInit();

  if(statuscode == 2){ //master - rank 0

    /*
     * Start one worker process that will be the "master actor"
     */
    int workerPid = startWorkerProcess();
    int actor_code = MASTER_ACTOR;
    MPI_Ssend(&actor_code,1,MPI_INT, workerPid,0, MPI_COMM_WORLD);

    do{
      //monitor workering/sleeping processes in process pool code
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
        MasterActor m = MasterActor(months,month_delay,grids,squirrels,infect_squirrels,max_squirrels);
        a = &m;
      }
      else if(actor_code == GRID_ACTOR){
        GridActor g = GridActor();
        a = &g;
      }
      else if(actor_code == SQUIRREL_ACTOR){
        SquirrelActor s = SquirrelActor(0, squirrel_delay);
        a = &s;
      }
      else if(actor_code == INFECTED_SQUIRREL_ACTOR){
        SquirrelActor s = SquirrelActor(1, squirrel_delay);
        a = &s;
      }
      else if(actor_code == NEWBORN_SQUIRREL_ACTOR){
        //newborn squirrel will recieve its coordinates
        float location[2];
        MPI_Recv(location,2,MPI_FLOAT, 1,SQUIRREL_BIRTH, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        SquirrelActor s = SquirrelActor(0,location[0],location[1], squirrel_delay);
        a = &s;
      }

      a->act();
    }while(workerSleep());

  }

  processPoolFinalise();
  MPI_Finalize();

}
