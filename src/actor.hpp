#ifndef ACTOR_H
#define ACTOR_H

class Actor
{
public:
  Actor();
  virtual void act();
protected:
  int m_rank;
  int m_size;
};

#endif
