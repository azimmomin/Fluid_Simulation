#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "vector.h"

class Matrix{//A 3x3 matrix class.
public:
	Matrix();//Creates identity matrix.
	Matrix(const Vector& col0, const Vector& col1, const Vector& col2);//Vectors are columns.

	//Returns matrix defined by left*transpose(right)
	static Matrix OuterProduct(const Vector& left, const Vector& right);
	Matrix SetColumn(int column, const Vector& vector) const;
	Vector Solve(const Vector& constants) const;//Returns x that solves equation constants = this*x

	Vector operator*(const Vector& right) const;
	Matrix operator*(float right) const;
	Matrix operator+(const Matrix& right) const;
	
	float Determinant() const;
private:
	float data[9];//Stored index = row + column*3
};

#endif//MATRIX_HPP
