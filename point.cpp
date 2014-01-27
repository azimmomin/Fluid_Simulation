#include <iostream>
#include <cmath>
#include <math.h>
#include "point.h"
#include "vector.h"

#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }

using namespace std;

Point::Point(const Vector& v) {
  x = v.x;
  y = v.y;
  z = v.z;
}

Point::Point(float x_val, float y_val, float z_val) {
  x = x_val;
  y = y_val;
  z = z_val;
}

float Point::operator*(const Point& p) const {
  return (x*p.x + y*p.y + z*p.z);
}

Point Point::operator*(float i) const {
  Point newPoint(i*x, i*y, i*z);
  return newPoint;
}

Vector Point::operator-(const Point& p) const {
  Vector newVector(x - p.x, y - p.y, z - p.z);
  return newVector;
}

Point Point::operator+(const Vector& v) const {
  Point newP(x + v.x, y + v.y, z + v.z);
  return newP;
}

Point Point::operator+(const Point& p) const {
  Point newP(x + p.x, y + p.y, z + p.z);
  return newP;
}

bool Point::operator!=(const Point& r) const{
  return(x != r.x || y != r.y || z != r.z);
}

void Point::show() const {
  cout << "Point: " << x << " " << y << " " << z << endl;
}
