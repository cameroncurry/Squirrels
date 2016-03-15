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
  int pop_influx[3];
  int population_influx();
};

class GridActor: public Actor
{
public:
  GridActor();
  ~GridActor();
  void act();
private:
  int n_cells;
  int month;
  Cell cell;

  float* gridValues; //gridValues = {population_influx, infection_level}
  void advanceMonth(); //move grid cells to next month
};



#endif
