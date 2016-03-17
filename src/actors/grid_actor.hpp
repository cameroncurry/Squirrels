#ifndef GRID_H
#define GRID_H

#include "actor.hpp"


#ifndef GRID_SHUTDOWN
#define GRID_SHUTDOWN 184
#endif
#ifndef GRID_NEW_MONTH
#define GRID_NEW_MONTH 185
#endif


class GridActor: public Actor
{
public:
  GridActor();
  void act();
private:
  int month;
  int acting;
  int pop_influx[3]; //population influx of the past 3 months
  int infect_level[2]; //infection level of the past 2 months

  void advanceMonth();
  void handleMasterMessage();
  void handleSqurrielMessage(int source);
  void gridShutdown();
  int populationInflux(); //total influx over the past 3 months
  int infectionLevel();
};

#endif
