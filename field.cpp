#include "field.h"

DirectionField::DirectionField(const Vector& v, float f) {
  dir = v;
  str = f;
}

Vector DirectionField::applyTo(Particle* p) {
  return dir * str;
}

AccelerationField::AccelerationField(const Vector& v, float f) {
  dir = v;
  str = f;
}

Vector AccelerationField::applyTo(Particle* p) {
  return dir * str * p->density;
}

PointField::PointField(const Point& p, float f, bool b, Kernel* k) {
  pos = p;
  str = f;
  repel = b;
  kernel = k;
}

Vector PointField::applyTo(Particle* p) {
  Vector v = p->pos - pos;
  return v * ((repel ? 1.0f : -1.0f) * (*kernel)(pos - p->pos) * str);
}

FieldVector::FieldVector() {
}

FieldVector::FieldVector(Point p, Vector v) {
  pos = p;
  dir = v;
}

VectorField::VectorField(const vector<FieldVector*>& v, float h) {
  vectors = v;
  kernel = new Kernel(h);
}

VectorField::VectorField(const vector<FieldVector*>& v, Kernel* k) {
  vectors = v;
  kernel = k;
}

Vector VectorField::applyTo(Particle* p) {
  Vector sum;
  for(unsigned int i = 0; i < vectors.size(); i++) {
    FieldVector* fv = vectors[i];
    sum = sum + fv->dir * (fv->pos - p->pos).abs();
  }
  return sum;
}

FieldBox::FieldBox(Point p1, Vector p2, Vector v, float str) {
  min = p1;
  max = p1 + p2;
  accel = new AccelerationField(v, str);
}

bool FieldBox::isWithin(const Point& p) {
  return (p.x > min.x && p.y > min.y && p.z > min.z && p.x < max.x && p.y < max.y && p.z < max.z); 
}

BoxField::BoxField(const vector<FieldBox*>& b) {
  boxes = b;
}

Vector BoxField::applyTo(Particle* p) {
  Point pos = p->pos;
  Vector sum;
  vector<FieldBox*>::iterator iter = boxes.begin();
  while (iter != boxes.end()) {
    if ((*iter)->isWithin(pos)) {
      sum = sum + (*iter)->accel->applyTo(p);
    }
    iter++;
  }
  return sum;
}
