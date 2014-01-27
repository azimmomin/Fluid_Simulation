#ifndef FIELD_H
#define FIELD_H

#include "particle.h"
#include "vector.h"
#include "kernel.h"
#include <vector>

using namespace std;

class Field {
public:
  virtual Vector applyTo(Particle* p) = 0;
};

class DirectionField : public Field {
public:
  float str;
  Vector dir;
  DirectionField(const Vector& v, float f);
  virtual Vector applyTo(Particle* p);
};

class AccelerationField : public Field {
public:
  float str;
  Vector dir;
  AccelerationField(const Vector& v, float f);
  virtual Vector applyTo(Particle* p);
};

class PointField : public Field {
public:
  bool repel;
  Point pos;
  float str;
  Kernel *kernel;
  PointField(const Point& p, float f, bool b, Kernel *k);
  virtual Vector applyTo(Particle* p);
};

class FieldVector {
public:
  Point pos;
  Vector dir;
  FieldVector();
  FieldVector(Point p, Vector v);
};

class VectorField : public Field {
public:
  vector<FieldVector*> vectors;
  Kernel* kernel;
  VectorField(const vector<FieldVector*>& v, Kernel *k);
  VectorField(const vector<FieldVector*>& v, float h);
  virtual Vector applyTo(Particle* p);
};

class FieldBox {
public:
  Point min;
  Point max;
  AccelerationField* accel;
  bool isWithin(const Point& p);
  FieldBox(Point p1, Vector p2, Vector v, float str);
};


class BoxField : public Field {
public:
  vector<FieldBox*> boxes;
  virtual Vector applyTo(Particle* p);
  BoxField(const vector<FieldBox*>& b);
};
#endif
