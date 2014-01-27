#ifndef FORCE_H
#define FORCE_H

#include <vector>
#include "particle.h"
#include "kernel.h"
#include "field.h"

using namespace std;

/*
#define PRESSURE_K 0.5f
#define PRESSURE_REST_DENSITY 40.0f
#define VISCOSITY_MU 0.001f*/

/*
#define PRESSURE_K 0.0001f
#define PRESSURE_REST_DENSITY 0.003f
#define PRESSURE_EXP 2.5f
#define VISCOSITY_MU 0.01f
*/
class Force {
public:
  virtual Vector applyTo(const vector<Particle*>& particles, Particle* particle) = 0;
};

class InternalForce : public Force {
public:
  ~InternalForce();//Deletes kernel.
  Kernel* kernel;
  virtual Vector applyTo(const vector<Particle*>& particles, Particle* particle);
  virtual Vector particleInteract(Particle* from, Particle* to) = 0;
};

class ViscosityForce : public InternalForce {
public:
  ViscosityForce(float h);//h is the kernel's radius of effect.
  virtual Vector particleInteract(Particle* from, Particle* to);
};

class PressureForce : public InternalForce {
public:
  PressureForce(float h);//h is the kernel's radius of effect.
  virtual Vector particleInteract(Particle* from, Particle* to);
};

class FieldForce : public Force {
public:
  Field* field;
  FieldForce(Field* f);
  virtual Vector applyTo(const vector<Particle*>& particles, Particle* particle);
};

class Forces : public Force {
public:
  Forces(float h, const vector<FieldBox*>& gravity);//h is the kernel's radius of effect.
  vector<Force*> forces;
  Vector applyTo(const vector<Particle*>& particles, Particle* particle);
};

#endif
