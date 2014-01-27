#ifndef KERNEL_H
#define KERNEL_H

#include "vector.h"

/* Matthias's Poly6 Kernel */
class Kernel {
public:
  Kernel(float effectiveRadius);
  virtual float operator() (const Vector& v) const;
  virtual Vector gradient(const Vector& v) const;
  virtual float laplacian(const Vector& v) const;
protected:
  const float h;
};

/* Desbrun's Spiky Pressure Kernel */
class PressureKernel : public Kernel {
public:
  PressureKernel(float effectiveRadius);
  virtual float operator() (const Vector& v) const;
  virtual Vector gradient(const Vector& v) const;
  virtual float laplacian(const Vector& v) const;
};

/* Matthias's Viscosity Kernel */
class ViscosityKernel : public Kernel {
public:
  ViscosityKernel(float effectiveRadius);
  virtual float operator() (const Vector& v) const;
  virtual Vector gradient(const Vector& v) const;
  virtual float laplacian(const Vector& v) const;
};

/* Obselete

// Gradient Operator Finite Approximations
// http://http.developer.nvidia.com/GPUGems/gpugems_ch38.html

class Del {
public:
  static float d;
  static Vector xShift;
  static Vector yShift;
  static Vector zShift;
  static Vector gradient(Kernel* kernel, const Vector& v);
  static float divergence(Kernel* kernel, const Vector& v);
  static float laplacian(Kernel* kernel, const Vector& v);
};

*/

#endif
