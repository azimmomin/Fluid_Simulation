#ifndef VECTOR_H
#define VECTOR_H

class Vector {
	
public:
	float x;
	float y;
	float z;

	Vector(float x_val = 0, float y_val = 0, float z_val = 0);

	Vector normalize() const;

	float magnitude() const;

	float abs() const;

	Vector cross(const Vector& v1) const;

	float operator*(const Vector& p) const;

	Vector operator*(float i) const;

	Vector operator-(const Vector& p) const;
	
	Vector operator+(const Vector& p) const;

	Vector reflect(const Vector& normal) const;//Reflect this vector about the given normal.

	Vector negate() const;

	void show() const;
};

#endif
