#include <iostream>
#include <unistd.h>
#include <mpi.h>
#include <vector>

//#include "squirrel_actor.h"
//#include "squirrel-functions.h"

#include "pool.h"
#include "actor.hpp"
#include "master_actor.hpp"


using namespace std;


/*
 * Grid Actor
 */
class GridActor: public Actor
{
public:
  GridActor();
  void act();
};

GridActor::GridActor(){
  //cout << "hello from grid on rank " << m_rank << endl;
}

void GridActor::act(){
  //cout << "grid acting will wait for squirrel to step on me" << endl;
}


/*
 * Squirrel Actor
 */
class SquirrelActor: public Actor
{
public:
  SquirrelActor(int N_grid);
  ~SquirrelActor();
  void act();
private:
  int N_grid;
  int* grid_ranks;
};

SquirrelActor::SquirrelActor(int N_grid){
  this->N_grid = N_grid;
  this->grid_ranks = new int[N_grid];
}
SquirrelActor::~SquirrelActor(){
  delete[] grid_ranks;
}

void SquirrelActor::act(){
  cout << "squirrel acting on rank " << m_rank << " will recieve grid ranks from master" << endl;

  MPI_Recv(grid_ranks,N_grid,MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  cout << "squirrel recieved grid ranks from master" << endl;
  for(int i=0;i<N_grid;i++){
    cout << grid_ranks[i] << endl;
  }
}

#define GRID_ACTOR 0
#define SQUIRREL_ACTOR 1

int main(){

  int grids = 3;
  int squirrels = 2;
  int inft_squirrels = 0;

  /*
   * Create actors
   */

  MPI_Init(NULL,NULL);

  int statuscode = processPoolInit();

  Actor *a;
  if(statuscode == 2){ //master

    //create grid cells
    int* grid_ranks = new int[grids];

    for(int i=0;i<grids;i++){
      int workerPid = startWorkerProcess(GRID_ACTOR);
      grid_ranks[i] = workerPid;
      //cout << "master created grid actor" << endl;
    }

    //create squirrels
    int* squirrel_ranks = new int[squirrels];
    for(int i=0;i<squirrels;i++){
      int workerPid = startWorkerProcess(SQUIRREL_ACTOR);
      squirrel_ranks[i] = workerPid;
      //cout << "master created squirrel_actor" << endl;
    }

    MasterActor m = MasterActor(grids,grid_ranks,squirrels,squirrel_ranks);
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
      SquirrelActor s = SquirrelActor(grids); //cout << "squirrel actor created" << endl;
      a = &s;
    }

    a->act();
  }


  processPoolFinalise();
  MPI_Finalize();

}
