#ifndef MASTER_H
#define MASTER_H

#include "actor.hpp"

class MasterActor: public Actor
{
public:
  MasterActor(int months, int grids, int squirrels, int infect_squirrels);
  ~MasterActor();
  void act();
private:
  int months;

  int N_grids;
  int N_squirrels;

  int* grid_ranks;


  void shutdownGridCells();
  void createNewSquirrel();


  /*
  double month_time;
  int N_grids;
  int* grid_ranks;
  int N_squirrels;
  int* squirrel_ranks;
  int squirrel_max;*/

  //void distributeGridRanks();
  //void advanceMonth();
  //void createNewSquirrel(int source);
};
#endif
