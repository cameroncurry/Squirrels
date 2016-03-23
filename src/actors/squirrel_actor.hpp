#ifndef SQUIRREL_H
#define SQUIRREL_H

#include "actor.hpp"

class SquirrelActor: public Actor
{
public:
  SquirrelActor(int infected);
  ~SquirrelActor();
  void act();
private:
  long state;
  int acting;
  int steps;
  int infected;
  int infected_step; //step at which squirrel became infected
  int N_grids;
  int* grid_ranks;
  float x,y;

  int pop_influxes[50];
  int infect_levels[50];

  void waitForGridRanks();
  void stepIntoCell();
  float averagePopInflux();
  float averageInfectionLevel();
};

#endif
