#ifndef NORMAL_H
#define NORMAL_H
#include "vector.h"

using namespace std;

class Normal : public Vector {
 public:
  Normal();
  Normal(float newX, float newY, float newZ);
  Normal(Vector v);
  Normal operator+(Normal n);
  Normal operator-(Normal n);
};

#endif
