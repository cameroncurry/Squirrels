#ifndef MASTER_H
#define MASTER_H

#include "actor.hpp"

class MasterActor: public Actor
{
public:
  MasterActor(int N_grid, int* grid_ranks, int N_squirrels, int* squirrel_ranks);
  ~MasterActor();
  void act();
private:
  int N_grid;
  int* grid_ranks;
  int N_squirrels;
  int* squirrel_ranks;
};
#endif
