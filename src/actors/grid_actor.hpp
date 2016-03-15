#ifndef GRID_H
#define GRID_H

#include "actor.hpp"


#ifndef GRID_SHUTDOWN
#define GRID_SHUTDOWN 184
#endif
#ifndef GRID_NEW_MONTH
#define GRID_NEW_MONTH 185
#endif

/*
 * Single grid cell
 */
class Cell {
public:
  int pop_influx[3]; //population influx for the past 3 months
  int inft_level[2]; //infection level for the past 2 months
  int population_influx();
  int infection_level();
};

class GridActor: public Actor
{
public:
  GridActor();
  void act();
private:
  int n_cells = 16;
  int month;
  Cell cells[16];

  int** pop_influx;

  void initialiseCells();
  void advanceMonth();
  void handleSqurrielMessage();

};

#endif
