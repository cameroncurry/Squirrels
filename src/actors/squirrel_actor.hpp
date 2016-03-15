#ifndef SQUIRREL_H
#define SQUIRREL_H

#include "actor.hpp"

class SquirrelActor: public Actor
{
public:
  SquirrelActor(int infected);
  void act();
private:
  long state;
  int infected; //0 - not infected, 1 - infected
  int grid_rank; //rank of grid actor to send messages to
  float x,y;
};

#endif
