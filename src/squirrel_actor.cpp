#include <stdio.h>
#include <vector>
#include "squirrel_actor.h"
#include "squirrel-functions.h"

SquirrelActor::SquirrelActor(bool infected, float initial_x, float initial_y){
  m_infected = infected;
  m_x = initial_x;
  m_y = initial_y;

  m_steps = 0;


  m_state = -1;
  initialiseRNG(&m_state);
}

/**
One squirrel step:
1.moves position
2.
*/
void SquirrelActor::step(){
  //printf("squirrel at %f %f\n",m_x,m_y);
  squirrelStep(m_x,m_y,&m_x,&m_y,&m_state);
  int pos = getCellFromPosition(m_x,m_y);
  printf("squirrel at %f %f in position %d \n",m_x,m_y,pos);

  m_pop_influx.push_back(m_steps);
  //printf("average pop influx %f\n",avgPopInflux());

  /*
  if(m_steps < 50){
    m_pop_influx[m_steps] = m_steps; //population influx of grid element
  }

  if(m_steps % 50 == 0){
    //reproduced
  }
  */
}

float SquirrelActor::avgPopInflux(){
  float sum = 0;
  for(int i=0;i<m_pop_influx.size();i++){
    sum += m_pop_influx[i];
  }
  return sum / (float)m_pop_influx.size();
}
