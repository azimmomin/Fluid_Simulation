#include "kernel.h"
#define _USE_MATH_DEFINES
#include <cmath>

Kernel::Kernel(float effectiveRadius)
      : h(effectiveRadius) { }

float Kernel::operator() (const Vector& v) const {
  float h2 = h * h;
  float r2 = v.abs();
  if (r2 >= h2) {
    return 0.0f;
  }
  return (315.0f / (64.0f * M_PI * pow(h, 9.0f))) * pow(h2 - r2, 3.0f);
}

Vector Kernel::gradient(const Vector& v) const {
  float h2 = h * h;
  float r2 = v.abs();
  if (r2 >= h2) {
    return 0.0f;
  }
  float t = (-945.0f / (32.0f * M_PI * pow(h, 9.0f))) * pow(h2 - r2, 2.0f);
  return v * t;
}

float Kernel::laplacian(const Vector& v) const {
  float h2 = h * h;
  float r2 = v.abs();
  if (r2 < 0.0f || r2 >= h2) {
    return 0.0f;
  }
  float t = (-945.0f / (32.0f * M_PI * pow(h, 9.0f)));
  float hr = h2 - r2;
  return (t * hr * (3.0f * hr - 4.0f * r2));
}

PressureKernel::PressureKernel(float effectiveRadius)
              : Kernel(effectiveRadius) { }

float PressureKernel::operator() (const Vector& v) const {
  float r = v.magnitude();
  if (r >= h) {
    return 0.0f;
  }
  return (15.0f / (M_PI * pow(h, 6.0f))) * pow(h - r, 3.0f);
}

Vector PressureKernel::gradient(const Vector& v) const {
  float r = v.magnitude();
  if (r >= h) {
    return 0.0f;
  }
  float t = (-45.0f / (M_PI * pow(h, 6.0f))) * pow(h - r, 2.0f);
  return (v.normalize() * t);
}

float PressureKernel::laplacian(const Vector& v) const {
  float r = v.magnitude();
  if (r <= 0.0f || r >= h) {
    return 0.0f;
  }
  float t = (-45.0f / (M_PI * pow(h, 6.0f)));
  float hr = h - r;
  return (t * hr * (3.0f / r - hr * r + 2.0f));
}

ViscosityKernel::ViscosityKernel(float effectiveRadius)
               : Kernel(effectiveRadius) { }

float ViscosityKernel::operator() (const Vector& v) const {
  float r = v.magnitude();
  if (r >= h) {
    return 0.0f;
  }
  float rh = r / h;
  return (15.0f / (2.0f * M_PI * pow(h, 3.0f))) * (-0.5f*pow(rh, 3.0f) + pow(rh, 2.0f) + pow(2.0f*rh, -1.0f) - 1.0f);
}

Vector ViscosityKernel::gradient(const Vector& v) const {
  float r = v.magnitude();
  if (r <= 0.0f || r >= h) {
    return 0.0f;
  }
  float h2 = h * h;
  float a1 = -3.0f * r / (h2 * h2);
  float a2 = 1.0f / (h2 * h);
  float a3 = -0.5f / (r * r * r);
  float t = (15.0f / (2.0f * M_PI * h2)) * (a1 + a2 + a3);
  return v * t;
}

float ViscosityKernel::laplacian(const Vector& v) const {
  float r = v.magnitude();
  if (r >= h) {
    return 0.0f;
  }
  return (45.0f / (M_PI * pow(h, 6.0f))) * (h - r);
}

/* Obselete

float Del::d = 1e-8f;
Vector Del::xShift = Vector(d, 0.0f, 0.0f);
Vector Del::yShift = Vector(0.0f, d, 0.0f);
Vector Del::zShift = Vector(0.0f, 0.0f, d);

Vector Del::gradient(Kernel* kernel, const Vector& v) {
  Kernel W = *kernel;
  Vector u;
  u.x = W(v + xShift) - W(v - xShift);
  u.y = W(v + yShift) - W(v - yShift);
  u.z = W(v + zShift) - W(v - zShift);
  u = u * (0.5f / d);
  return u;
}

float Del::divergence(Kernel* kernel, const Vector& v) {
  Kernel W = *kernel;
  return (W(v + xShift) + W(v + yShift) + W(v + zShift) - W(v - xShift) - W(v - yShift) - W(v - zShift)) / (2.0f * d);
}

float Del::laplacian(Kernel* kernel, const Vector& v) {
  Kernel W = *kernel;
  return (W(v + xShift) + W(v + yShift) + W(v + zShift) + W(v - xShift) + W(v - yShift) + W(v - zShift) - 6 * W(v)) / (d * d);
}

*/
