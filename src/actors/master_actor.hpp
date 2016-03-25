#ifndef MASTER_H
#define MASTER_H

#include "actor.hpp"

class MasterActor: public Actor
{
public:
  MasterActor(int months, int grids, int squirrels, int infect_squirrels,int max_squirrels);
  ~MasterActor();
  void act();
private:
  int months;
  double month_time; //in seconds
  int N_squirrels;
  int N_infected;
  int max_squirrels;
  int N_grids;
  int* grid_ranks;

  void shutdownGridCells();
  int createNewSquirrel(int squirrel_type);
  void advanceMonth(int month);
  //void checkSquirrels();


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
