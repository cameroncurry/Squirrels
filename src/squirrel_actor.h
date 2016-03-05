#include <vector>

using namespace std;

class SquirrelActor
{
public:
  SquirrelActor(bool infected, float initial_x, float inital_y);
  void step();

private:
  bool m_infected;
  float m_x,m_y;
  int m_steps;
  vector<float> m_pop_influx;
  
  long m_state;

  float avgPopInflux();

};
