#include "Matrix.hpp"

Matrix::Matrix(){
	data[0] = data[4] = data[8] = 1;
	data[1] = data[2] = data[3] = data[5] = data[6] = data[7] = 0;
}

Matrix::Matrix(const Vector& col0, const Vector& col1, const Vector& col2){
	data[0] = col0.x; data[1] = col1.x; data[2] = col2.x;
	data[3] = col0.y; data[4] = col1.y; data[5] = col2.y;
	data[6] = col0.z; data[7] = col1.z; data[8] = col2.z;
}

Matrix Matrix::OuterProduct(const Vector& left, const Vector& right){
	Matrix ret;
	ret.data[0] = left.x*right.x; ret.data[1] = left.x*right.y; ret.data[2] = left.x*right.z;
	ret.data[3] = left.y*right.x; ret.data[4] = left.y*right.y; ret.data[5] = left.y*right.z;
	ret.data[6] = left.z*right.x; ret.data[7] = left.z*right.y; ret.data[8] = left.z*right.z;
	return ret;
}

Matrix Matrix::SetColumn(int column, const Vector& vector) const{
	Matrix ret(*this);
	if(column < 3){
		ret.data[column + 0] = vector.x;
		ret.data[column + 3] = vector.y;
		ret.data[column + 6] = vector.z;
	}
	return(ret);
}

Vector Matrix::Solve(const Vector& constants) const{
	float det = Determinant();//TODO: Make this faster?
	float x = SetColumn(0, constants).Determinant() / det;
	float y = SetColumn(1, constants).Determinant() / det;
	float z = SetColumn(2, constants).Determinant() / det;
	return(Vector(x, y, z));
}

Vector Matrix::operator*(const Vector& right) const{
	Vector ret;
	ret.x = data[0]*right.x + data[1]*right.y + data[2]*right.z;
	ret.y = data[3]*right.x + data[4]*right.y + data[5]*right.z;
	ret.z = data[6]*right.x + data[7]*right.y + data[8]*right.z;
	return ret;
}

Matrix Matrix::operator*(float right) const{
	Matrix ret;
	for(int i = 0;i < 9;i++){
		ret.data[i] = data[i]*right;
	}
	return ret;
}

Matrix Matrix::operator+(const Matrix& right) const{
	Matrix ret;
	for(int i = 0;i < 9;i++){
		ret.data[i] = data[i] + right.data[i];
	}
	return ret;
}

float Matrix::Determinant() const{
	return(data[0]*data[4]*data[8] + data[1]*data[5]*data[6]
	     + data[2]*data[3]*data[7] - data[2]*data[4]*data[6]
	     - data[0]*data[5]*data[7] - data[1]*data[3]*data[8]);
}
