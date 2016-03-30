#ifndef SQUIRREL_H
#define SQUIRREL_H

#include "actor.hpp"

class SquirrelActor: public Actor
{
public:
  SquirrelActor(int infected, double squirrel_delay);
  SquirrelActor(int infected,float x,float y, double squirrel_delay);
  ~SquirrelActor();
  void act();
private:
  double squirrel_delay;
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

  void contruct();
  void waitForGridRanks();
  void stepIntoCell();
  float averagePopInflux();
  float averageInfectionLevel();
};

#endif
