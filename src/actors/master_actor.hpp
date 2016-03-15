#ifndef MASTER_H
#define MASTER_H

#include "actor.hpp"

class MasterActor: public Actor
{
public:
  MasterActor(int grid_rank, int N_squirrels, int* squirrel_ranks);
  ~MasterActor();
  void act();
private:
  int grid_rank;
  int N_squirrels;
  int* squirrel_ranks;
};
#endif
