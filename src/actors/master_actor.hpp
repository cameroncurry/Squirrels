#ifndef MASTER_H
#define MASTER_H

#include "actor.hpp"

class MasterActor: public Actor
{
public:
  MasterActor(int months, double month_time, int grids, int squirrels, int infect_squirrels,int max_squirrels);
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

  void handleSquirrelInfected(int source);
  int handleSquirrelDeath(int source);
  int handleSquirrelBirth(int source);

  void shutdownGridCells();
  int createNewSquirrel(int squirrel_type);
  void advanceMonth(int month);
  void endSimulation();

  int testall(int count, MPI_Request* request);
};
#endif
