// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/vector3.h"
#include <array>

// Column-major 3x3 matrix
class Matrix3
{
    public:
    Matrix3();
    Matrix3(const Vector3 &x, const Vector3 &y, const Vector3 &z);

    private:
    // Matrix
    std::array<double, 9> matrix_;

    /*
     * Operators
     */
    public:
    Matrix3 operator*(const Matrix3 &B) const;
    Matrix3 operator*(const double a) const;
    Matrix3 operator+(const Matrix3 &B) const;
    Matrix3 operator-(const Matrix3 &B) const;
    Vector3 operator*(const Vector3 &v) const;
    Matrix3 &operator*=(const Matrix3 &B);
    Matrix3 &operator*=(const double a);
    double &operator[](int);
    bool isApproximately(const Matrix3 &other, double tolerance = 1.0e-6) const;

    /*
     * General Routines
     */
    public:
    // Return the current matrix array
    const std::array<double, 9> &matrix() const;
    // Reset the matrix to the identity
    void setIdentity();
    // Prints the matrix to stdout
    void print() const;
    // Set the zero matrix
    void zero();
    // Create orthogonal matrix around supplied single column vector
    void createFromVector(const Vector3 &v, int columnIndex);
    // Return transpose of current matrix
    Matrix3 &transpose() const;
    // Transform the supplied vector by the transpose of the current matrix
    Vector3 transposeMultiply(const Vector3 &v) const;
    // Calculate determinant
    double determinant() const;
    // Invert matrix
    void invert();
    // Return nth value of matrix
    double value(int n) const;
    // Return maximal element
    double max() const;
    // Return maximal absolute element
    double maxAbs() const;

    /*
     * Column Operations
     */
    public:
    // Copy column contents to supplied Vec3
    Vector3 columnAsVec3(int col) const;
    // Set specified row from supplied triplet of values
    void setRow(int row, double x, double y, double z);
    // Set specified column from supplied values
    void setColumn(int col, double a, double b, double c);
    // Set specified column from supplied Vec3
    void setColumn(int col, const Vector3 vec);
    // Adjust specified column from supplied values
    void adjustColumn(int col, double a, double b, double c);
    // Adjust specified column from supplied Vec3
    void adjustColumn(int col, const Vector3 vec);
    // Calculate column magnitude
    double columnMagnitude(int column) const;
    // Multiply single column by single value
    void columnMultiply(int col, double d);
    // Multiply columns by values in supplied vector
    void columnMultiply(const Vector3 vec);
    // Normalise specified column to 1
    void columnNormalise(int column);
    // Orthogonalise rotation matrix column w.r.t. one (or two) other columns)
    void orthogonaliseColumn(int targetcol, int orthcol1, int orthocol2 = -1);

    /*
     * Rotations
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
    void createRotationAxis(Vector3 axis, double angle, bool normalise);
    // Apply rotation about X axis
    void applyRotationX(double angle);
    // Apply axis rotation quaternion
    void applyRotationAxis(double ax, double ay, double az, double angle, bool normalise);

    /*
     * Scaling
     */
    public:
    // Apply a general scaling to the matrix (as glScaled would to)
    void applyScaling(double scalex, double scaley, double scalez);
    // Apply a general scaling to the matrix (as glScaled would to)
    void applyScaling(Vector3 scaling);
    // Apply an xy-scaling to the matrix
    void applyScalingXY(double scalex, double scaley);
    // Apply an x-scaling to the matrix
    void applyScalingX(double scale);
    // Apply a y-scaling to the matrix
    void applyScalingY(double scale);
    // Apply a z-scaling to the matrix
    void applyScalingZ(double scale);

    /*
     * Transforms
     */
    public:
    // Transform coordinates supplied and return as Vector3
    Vector3 transform(double x, double y, double z) const;
    // Transform coordinates supplied and return as Vector3
    Vector3 transform(const Vector3 r) const;

    /*
     * Special Functions
     */
    public:
    // Construct 'cross-product' matrix of the supplied vector using cyclic permutations
    void makeCrossProductMatrix(const Vector3 &v);
};
