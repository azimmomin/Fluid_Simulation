#include <iostream>
#include <cmath>
#include "vector.h"

inline float sqr(float x) { return x*x; }

using namespace std;

Vector::Vector(float x_val, float y_val, float z_val) {
	x = x_val;
	y = y_val;
	z = z_val;
}

Vector Vector::normalize() const {
	float magnitude = this->magnitude();
	if (magnitude <= 0.0000001) { return Vector(0, 0, 0); }
	return Vector(x / magnitude, y / magnitude, z / magnitude);
}

float Vector::magnitude() const {
	return sqrt(sqr(x) + sqr(y) + sqr(z));
}

float Vector::abs() const {
	return (sqr(x) + sqr(y) + sqr(z));
}

Vector Vector::cross(const Vector& v1) const {
	Vector crossV;
	crossV.x = (y * v1.z) - (z * v1.y);
	crossV.y = (z * v1.x) - (x * v1.z);
	crossV.z = (x * v1.y) - (y * v1.x);
	return crossV;
}

float Vector::operator*(const Vector& p) const {
	return (x*p.x + y*p.y + z*p.z);
}

Vector Vector::operator*(float i) const {
	return Vector(i*x, i*y, i*z);
}

Vector Vector::operator-(const Vector& p) const {
	return Vector(x - p.x, y - p.y, z - p.z);
}

Vector Vector::operator+(const Vector& p) const {
	return Vector(x + p.x, y + p.y, z + p.z);
}

Vector Vector::reflect(const Vector& normal) const{
	return(normal*((*this)*normal)*2.0f - (*this));
}

Vector Vector::negate() const {
	return Vector(-this->x, -this->y, -this->z);
}

void Vector::show() const {
	cout << "Vector: " << x << " " << y << " " << z << endl;
}
