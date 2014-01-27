#ifndef PARTICLE_H
#define PARTICLE_H

#include "vector.h"
#include "point.h"
#include "normal.h"

class Particle {

 public:
  Point pos;
  Vector vel;
  Vector accel;
  float mass;
  float density;
  Particle();
  Particle(Point p, Vector v, float m);
  void show(); // DEBUG INFO
};

#endif
