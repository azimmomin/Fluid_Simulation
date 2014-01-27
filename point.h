#pragma once
#ifndef POINT_H
#define POINT_H

#include "vector.h"

class Point {
  
public:
  float x;
  float y;
  float z;

  Point(const Vector& v);

  Point(float x_val = 0, float y_val = 0, float z_val = 0);

  float operator*(const Point& p) const;//Dot product

  Point operator*(float i) const;

  Vector operator-(const Point& p) const;
  
  Point operator+(const Vector& v) const;

  Point operator+(const Point& p) const;

  bool operator!=(const Point& r) const;

  void show() const;

};

#endif
