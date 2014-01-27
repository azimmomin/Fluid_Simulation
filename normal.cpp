#include "normal.h"
#include "vector.h"
using namespace std;

Normal::Normal() {
  x = 0.0f;
  y = 0.0f;
  z = 0.0f;
}

Normal::Normal(float newX, float newY, float newZ) {
  Vector v(newX, newY, newZ);
  v = v.normalize();
  x = v.x;
  y = v.y;
  z = v.z;
}

Normal::Normal(Vector v) {
  v = v.normalize();
  x = v.x;
  y = v.y;
  z = v.z;
}

Normal Normal::operator+(Normal n) {
  Vector v;
  v.x = x + n.x;
  v.y = y + n.y;
  v.z = z + n.z;
  Normal normal(v.normalize());
  return normal;
}

Normal Normal::operator-(Normal n) {
  Vector v;
  v.x = x - n.x;
  v.y = y - n.y;
  v.z = z - n.z;
  Normal normal(v.normalize());
  return normal;
}
