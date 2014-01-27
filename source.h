#ifndef SOURCE_H
#define SOURCE_H

#include "grid.h"
#include "particle.h"
#include "point.h"
#include "vector.h"

/**
 * Defines a fluid source inside the grid.
 */
class Source {
public:
  // Marks which grid the source is located on
  Grid* grid;
  // Where in the grid the source is located, defaults to origin
  Point pos;
  // Initial velocity to emit fluid particles, defaults to no velocity
  Vector vel;
  // Scatter parameters to add randomness
  // Minimum scatter in x,y,z directions, defaults to -1,-1,-1
  float xMinScatter;
  float yMinScatter;
  float zMinScatter;
  // Maximum scatter in x,y,z directions, defaults to 1,1,1
  float xMaxScatter;
  float yMaxScatter;
  float zMaxScatter;
  // Distance parameters to add randomness
  // Minimum distance to stray from source, defaults to 0,0,0
  float xMinDistance;
  float yMinDistance;
  float zMinDistance;
  // Maximum distance to stray from source, defaults to 0,0,0
  float xMaxDistance;
  float yMaxDistance;
  float zMaxDistance;
  // Mass of particles to emit
  float mass;
  // How many particles to emit per timestep
  float rate;
  // Stores leftover rate, if there is any from each timestep
  float currentRate;
  // How many timesteps to emit particles before stopping
  int limit;
  // How many timesteps to wait before emitting particles
  int delay;
  // Seeds the random generator based on system time, only seeds once
  static void seed();

  Source(Grid *g, Point& p, Vector& v, float m);
  Source(Grid *g, Point* p, Vector* v, float m);
  // various set functions
  void setMinScatter(float x, float y, float z);
  void setMaxScatter(float x, float y, float z);
  void setMinDistance(float x, float y, float z);
  void setMaxDistance(float x, float y, float z);
  void setDelay(int d);
  void setLimit(int l);
  void setRate(float r);
  void setGrid(Grid* g);
  // to be called on each timestep, creates particles on the grid
  void update(vector<Particle>& particles);
  // get functions, for convenience
  Grid* getGrid();
};

class InitializeSource : public Source {
public:
  InitializeSource(Grid* g, float m, Point& p, float w, float h, float d, int n = 500);
};

#endif
