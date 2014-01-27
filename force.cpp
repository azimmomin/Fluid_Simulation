#include <cmath>
#include "force.h"
#include <iostream>

float PRESSURE_K = 0.5f;
float PRESSURE_REST_DENSITY = 40.0f;
float VISCOSITY_MU = 0.001f;

InternalForce::~InternalForce(){
  delete kernel;
}

Vector InternalForce::applyTo(const vector<Particle*>& particles, Particle* particle) {
  vector<Particle*>::const_iterator iter = particles.begin();
  Vector sum;
  while(iter != particles.end()){
    sum = sum + particleInteract(*iter, particle);
    iter++;
  }
  return sum;
}

ViscosityForce::ViscosityForce(float h) {
  kernel = new ViscosityKernel(h);
}

Vector ViscosityForce::particleInteract(Particle* from, Particle* to) {
if(from->density==0.0f){return Vector(0.0f,0.0f,0.0f);}
  return (from->vel - to->vel) * (kernel->laplacian(to->pos - from->pos) * VISCOSITY_MU * from->mass / (from->density));
}

PressureForce::PressureForce(float h) {
  kernel = new PressureKernel(h);
}

float pressure(float density) {
  return PRESSURE_K * (density - PRESSURE_REST_DENSITY);
  //return PRESSURE_K * (pow(density / PRESSURE_REST_DENSITY, 3.0f) - 1.0f);
}

Vector PressureForce::particleInteract(Particle* from, Particle* to) {
  return kernel->gradient(to->pos - from->pos) * (pressure(from->density) + pressure(to->density)) * (-from->mass / (2 * from->density));
  //return kernel->gradient(to->pos - from->pos) * (from->mass) * (pressure(from->density) + pressure(to->density) / (-2.0f * to->density * to->density));
}

FieldForce::FieldForce(Field *f) {
  field = f;
}

Vector FieldForce::applyTo(const vector<Particle*>& particles, Particle* particle) {
  return field->applyTo(particle);
}

// Implement Force Equations for Forces
// http://matthiasmueller.info/publications/sca03.pdf

Forces::Forces(float h, const vector<FieldBox*>& gravity) {
  forces.push_back(new PressureForce(h));
  forces.push_back(new ViscosityForce(h));
  if (gravity.empty()) {
    Vector gravity(0.0f, -1.0f, 0.0f);
    float gravity_strength = 9.8;
    forces.push_back(new FieldForce(new AccelerationField(gravity, gravity_strength)));
  } else {
    forces.push_back(new FieldForce(new BoxField(gravity)));
  }
}

Vector Forces::applyTo(const vector<Particle*>& particles, Particle* particle) {
  vector<Force*>::const_iterator iter = forces.begin();
  Vector sum;
  while(iter != forces.end()){
    sum = sum + (*iter)->applyTo(particles, particle);
    iter++;
  }
  return sum;
}
