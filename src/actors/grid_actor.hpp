#ifndef GRID_H
#define GRID_H

#include "actor.hpp"

class GridActor: public Actor
{
public:
  GridActor();
  void act();
private:
  int month;
  int waitingForMessages;
  int current_month_influx;
  int current_month_infection;
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
