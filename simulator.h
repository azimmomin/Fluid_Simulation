#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include "particle.h"
#include "grid.h"
#include "force.h"
#include "vector.h"
#include "point.h"
#include "BoundingTree.hpp"
#include <vector>

class Simulator {

 public:
  const float timestep;
  Grid* grid;
  BoundingTree* tree;
  Forces* forces;
  Kernel kernel;
  float w, h, d;
  Simulator(float width, float height, float depth, float div, float step, Grid& gridi, const vector<Object>& objects, const vector<FieldBox*>& gravity); 
  ~Simulator();
  void calculateDensity(vector<Particle>& particles);
  bool firstUpdate;
  void update(vector<Particle>& particles);
  void updateVelocity(Particle& p, const Vector& a);
  Point getNewPosition(const Particle& p);
  void rebuildTree(const vector<Object>& objectList);
};

#endif
