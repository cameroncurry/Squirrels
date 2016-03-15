#ifndef ACTOR_H
#define ACTOR_H

class Actor
{
public:
  Actor();
  virtual void act();
protected:
  int rank;
  int size;
};

#endif
