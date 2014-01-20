/*
	*** Column-Major 3x3 Matrix class
	*** src/lib/math/matrix3.h
	Copyright T. Youngs 2012-2014

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_MATRIX3_H
#define DUQ_MATRIX3_H

#include "templates/vector3.h"

// Column-major 3x3 matrix
class Matrix3
{
	public:
	// Constructor
	Matrix3();
	
	private:
	// Matrix
	double matrix_[9];


	/*
	// Operators
	*/
	public:
	Matrix3 operator*(const Matrix3& B) const;
	Matrix3 operator*(const double a) const;
	Matrix3 operator+(const Matrix3& B) const;
	Matrix3 operator-(const Matrix3& B) const;
	Vec3<double> operator*(const Vec3<double>& v) const;
	Matrix3& operator*=(const Matrix3& B);
	double& operator[](int);


	/*
	// General Routines
	*/
	public:
	// Reset the matrix to the identity
	void setIdentity();
	// Prints the matrix to stdout
	void print() const;
	// Set the zero matrix
	void zero();
	// Return matrix array
	double* matrix();
	// Return transpose of current matrix
	Matrix3& transpose();
	// Calculate determinant
	double determinant() const;
	// Invert matrix
	void invert();
	// Return maximal element
	double max() const;


	/*
	// Column Operations
	*/
	public:
	// Copy column contents to supplied Vec3
	Vec3<double> columnAsVec3(int col) const;
	// Set specified row from supplied triplet of values
	void setRow(int row, double x, double y, double z);
	// Set specified column from supplied values
	void setColumn(int col, double a, double b, double c);
	// Set specified column from supplied Vec3
	void setColumn(int col, const Vec3<double> vec);
	// Adjust specified column from supplied values
	void adjustColumn(int col, double a, double b, double c);
	// Adjust specified column from supplied Vec3
	void adjustColumn(int col, const Vec3<double> vec);
	// Calculate column magnitude
	double columnMagnitude(int column) const;
	// Multiply single column by single value
	void columnMultiply(int col, double d);
	// Multiply first three columns by values in supplied vector
	void columnMultiply(const Vec3<double> vec);
	// Normalise specified column to 1
	void columnNormalise(int column);
	// Orthogonalise rotation matrix column w.r.t. one (or two) other columns)
	void orthogonaliseColumn(int targetcol, int orthcol1, int orthocol2 = -1);


	/*
	// Rotations
	*/
	public:
	// Create rotation matrix about X
	void createRotationX(double angle);
	// Create XY rotation matrix
	void createRotationXY(double anglex, double angley);
	// Create rotation matrix about Y
	void createRotationY(double angle);
	// Create rotation matrix about Z
	void createRotationZ(double angle);
	// Create axis rotation quaternion
	void createRotationAxis(double ax, double ay, double az, double angle, bool normalise);
	// Apply rotation about X axis
	void applyRotationX(double angle);
	// Apply axis rotation quaternion
	void applyRotationAxis(double ax, double ay, double az, double angle, bool normalise);


	/*
	// Scaling
	*/
	public:
	// Apply a general scaling to the matrix (as glScaled would to)
	void applyScaling(double scalex, double scaley, double scalez);
	// Apply a general scaling to the matrix (as glScaled would to)
	void applyScaling(Vec3<double> scaling);
	// Apply an xy-scaling to the matrix
	void applyScalingXY(double scalex, double scaley);
	// Apply an x-scaling to the matrix
	void applyScalingX(double scale);
	// Apply a y-scaling to the matrix
	void applyScalingY(double scale);
	// Apply a z-scaling to the matrix
	void applyScalingZ(double scale);


	/*
	// Transforms
	*/
	public:
	// Transform coordinates supplied and return as Vec3<double>
	Vec3<double> transform(double x, double y, double z) const;
	// Transform coordinates supplied and return as Vec3<double>
	Vec3<double> transform(const Vec3<double> vec) const;
};

#endif

