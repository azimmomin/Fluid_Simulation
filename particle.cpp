#include "particle.h"
#include <iostream>

Particle::Particle() {
  accel = 0.0f;
  mass = 0.0f;
  density = 0.0f;
}

Particle::Particle(Point p, Vector v, float m) {
  pos = p;
  vel = v;
  mass = m;
  accel = 0.0f;
  density = 0.0f;
}

void Particle::show() {
  cout << "Particle:" << endl;
  pos.show();
  vel.show();
  cout << "Mass: " << mass << "; Density: " << density << endl;
}
