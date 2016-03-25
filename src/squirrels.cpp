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
  int squirrels = 5;
  int infect_squirrels = 1;
  int max_squirrels = 10;

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
      else if(actor_code == NEWBORN_SQUIRREL_ACTOR){
        //newborn squirrel will recieve its coordinates
        float location[2];
        MPI_Recv(location,2,MPI_FLOAT, 1,SQUIRREL_BIRTH, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        cout << "new being born at "<<location[0]<<" "<<location[1]<<endl;
        //cout << "new being born"<<endl;
        SquirrelActor s = SquirrelActor(0,location[0],location[1]);
        a = &s;
        a->act();
      }

    }while(workerSleep());

  }

  processPoolFinalise();
  MPI_Finalize();

}
