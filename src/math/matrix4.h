/*
	*** Column-Major (OpenGL-friendly) 4x4 Matrix class
	*** src/math/matrix4.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_MATRIX4_H
#define DUQ_MATRIX4_H

#include "templates/vector3.h"
#include "templates/vector4.h"
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef _MAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// Column-major 4x4 matrix
class Matrix4
{
	public:
	// Constructor
	Matrix4();
	
	private:
	// Matrix4
	double matrix_[16];


	/*
	// Operators
	 */
	public:
	Matrix4 operator*(const Matrix4& B) const;
	Matrix4 operator*(const double a) const;
	Matrix4 operator+(const Matrix4& B) const;
	Matrix4 operator-(const Matrix4& B) const;
	Vec3<double> operator*(const Vec3<double>& v) const;
	Vec4<double> operator*(const Vec4<double> &v) const;
	Matrix4& operator*=(const Matrix4& B);
	double &operator[](int);


	/*
	// Basic Set/Get
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
	Matrix4& transpose();
	// Calculate determinant
	double determinant();
	// Invert matrix
	void invert();


	/*
	// Column Operations
	 */
	public:
	// Copy column contents to supplied Vec3
	Vec3<double> columnAsVec3(int col);
	// Copy column contents to supplied Vec4
	Vec4<double> columnAsVec4(int col);
	// Set specified row from supplied triplet of values
	void setRow(int row, double x, double y, double z);
	// Set specified row from supplied values
	void setRow(int row, double x, double y, double z, double w);
	// Set specified column from supplied values
	void setColumn(int col, double a, double b, double c, double d);
	// Set specified column from supplied Vec3
	void setColumn(int col, Vec3<double> vec, double w);
	// Set specified column from supplied Vec4
	void setColumn(int col, Vec4<double> vec);
	// Adjust specified column from supplied values
	void adjustColumn(int col, double a, double b, double c, double d);
	// Adjust specified column from supplied Vec3
	void adjustColumn(int col, Vec3<double> vec, double w);
	// Adjust specified column from supplied Vec4
	void adjustColumn(int col, Vec4<double> vec);
	// Calculate column magnitude
	double columnMagnitude(int column);
	// Multiply single column by single value
	void columnMultiply(int col, double d);
	// Multiply first three columns by values in supplied vector
	void columnMultiply(Vec3<double> vec);
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
	// Translations
	 */
	public:
	// Create a translation to the matrix (as glTranslated would do)
	void createTranslation(double dx, double dy, double dz);
	// Apply a translation to the matrix (as glTranslated would do)
	void applyTranslation(double dx, double dy, double dz);
	// Apply a translation to the matrix (as glTranslated would do)
	void applyTranslation(Vec3<double> vec);
	// Apply an X-translation to the matrix (as glTranslated would do)
	void applyTranslationX(double dx);
	// Apply an Y-translation to the matrix (as glTranslated would do)
	void applyTranslationY(double dy);
	// Apply an Z-translation to the matrix (as glTranslated would do)
	void applyTranslationZ(double dz);
	// Add a translation to the matrix
	void addTranslation(Vec3<double> v);
	// Add a translation to the matrix
	void addTranslation(double dx, double dy, double dz);
	// Set translation in the matrix
	void setTranslation(Vec3<double> v);
	// Set translation in the matrix
	void setTranslation(double x, double y, double z);


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
	Vec4<double> transform(double x, double y, double z, double w) const;
	// Transform coordinates supplied and return as Vec3<double>
	Vec3<double> transform(Vec3<double> vec) const;
	// Apply rotational part of matrix to supplied Vec3
	Vec3<double> rotateVector(Vec3<double> &v) const;
	// Apply rotational part of matrix to supplied vector coordinates
	Vec3<double> rotateVector(double x, double y, double z) const;
	// Multiply against coordinates provided (in GLfloats)
	void multiply(GLfloat* r, GLfloat* transformed) const;
	// Multiply against other matrix, but only rotational part, keeping translation/scaling intact
	void multiplyRotation(Matrix4 B);
	// Remove translation and scaling parts, leaving rotation only
	void removeTranslationAndScaling();
	// Copy translation and scaling parts from specified matrix
	void copyTranslationAndScaling(Matrix4& source);
};

#endif

