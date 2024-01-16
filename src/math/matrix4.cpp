// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/matrix4.h"
#include "base/messenger.h"
#include "math/matrix3.h"
#include <array>

Matrix4::Matrix4() { setIdentity(); }

/*
 * Operators
 */

// Matrix4 multiply (operator *) (return new matrix)
Matrix4 Matrix4::operator*(const Matrix4 &B) const
{
    // [ row(A|this).column(B) ]
    Matrix4 AB;
    AB.matrix_[0] =
        matrix_[0] * B.matrix_[0] + matrix_[4] * B.matrix_[1] + matrix_[8] * B.matrix_[2] + matrix_[12] * B.matrix_[3];
    AB.matrix_[1] =
        matrix_[1] * B.matrix_[0] + matrix_[5] * B.matrix_[1] + matrix_[9] * B.matrix_[2] + matrix_[13] * B.matrix_[3];
    AB.matrix_[2] =
        matrix_[2] * B.matrix_[0] + matrix_[6] * B.matrix_[1] + matrix_[10] * B.matrix_[2] + matrix_[14] * B.matrix_[3];
    AB.matrix_[3] =
        matrix_[3] * B.matrix_[0] + matrix_[7] * B.matrix_[1] + matrix_[11] * B.matrix_[2] + matrix_[15] * B.matrix_[3];

    AB.matrix_[4] =
        matrix_[0] * B.matrix_[4] + matrix_[4] * B.matrix_[5] + matrix_[8] * B.matrix_[6] + matrix_[12] * B.matrix_[7];
    AB.matrix_[5] =
        matrix_[1] * B.matrix_[4] + matrix_[5] * B.matrix_[5] + matrix_[9] * B.matrix_[6] + matrix_[13] * B.matrix_[7];
    AB.matrix_[6] =
        matrix_[2] * B.matrix_[4] + matrix_[6] * B.matrix_[5] + matrix_[10] * B.matrix_[6] + matrix_[14] * B.matrix_[7];
    AB.matrix_[7] =
        matrix_[3] * B.matrix_[4] + matrix_[7] * B.matrix_[5] + matrix_[11] * B.matrix_[6] + matrix_[15] * B.matrix_[7];

    AB.matrix_[8] =
        matrix_[0] * B.matrix_[8] + matrix_[4] * B.matrix_[9] + matrix_[8] * B.matrix_[10] + matrix_[12] * B.matrix_[11];
    AB.matrix_[9] =
        matrix_[1] * B.matrix_[8] + matrix_[5] * B.matrix_[9] + matrix_[9] * B.matrix_[10] + matrix_[13] * B.matrix_[11];
    AB.matrix_[10] =
        matrix_[2] * B.matrix_[8] + matrix_[6] * B.matrix_[9] + matrix_[10] * B.matrix_[10] + matrix_[14] * B.matrix_[11];
    AB.matrix_[11] =
        matrix_[3] * B.matrix_[8] + matrix_[7] * B.matrix_[9] + matrix_[11] * B.matrix_[10] + matrix_[15] * B.matrix_[11];

    AB.matrix_[12] =
        matrix_[0] * B.matrix_[12] + matrix_[4] * B.matrix_[13] + matrix_[8] * B.matrix_[14] + matrix_[12] * B.matrix_[15];
    AB.matrix_[13] =
        matrix_[1] * B.matrix_[12] + matrix_[5] * B.matrix_[13] + matrix_[9] * B.matrix_[14] + matrix_[13] * B.matrix_[15];
    AB.matrix_[14] =
        matrix_[2] * B.matrix_[12] + matrix_[6] * B.matrix_[13] + matrix_[10] * B.matrix_[14] + matrix_[14] * B.matrix_[15];
    AB.matrix_[15] =
        matrix_[3] * B.matrix_[12] + matrix_[7] * B.matrix_[13] + matrix_[11] * B.matrix_[14] + matrix_[15] * B.matrix_[15];
    return AB;
}

Matrix4 Matrix4::operator*(const double a) const
{
    Matrix4 AB;
    for (auto n = 0; n < 16; ++n)
        AB.matrix_[n] = matrix_[n] * a;
    return AB;
}

Matrix4 Matrix4::operator+(const Matrix4 &B) const
{
    Matrix4 A;
    for (auto n = 0; n < 16; ++n)
        A[n] = matrix_[n] + B.matrix_[n];
    return A;
}

Matrix4 Matrix4::operator-(const Matrix4 &B) const
{
    Matrix4 A;
    for (auto n = 0; n < 16; ++n)
        A[n] = matrix_[n] - B.matrix_[n];
    return A;
}

Vec3<double> Matrix4::operator*(const Vec3<double> &v) const
{
    Vec3<double> result;
    result.x = v.x * matrix_[0] + v.y * matrix_[4] + v.z * matrix_[8] + matrix_[12];
    result.y = v.x * matrix_[1] + v.y * matrix_[5] + v.z * matrix_[9] + matrix_[13];
    result.z = v.x * matrix_[2] + v.y * matrix_[6] + v.z * matrix_[10] + matrix_[14];
    return result;
}

Vec4<double> Matrix4::operator*(const Vec4<double> &v) const
{
    Vec4<double> result;
    result.x = v.x * matrix_[0] + v.y * matrix_[4] + v.z * matrix_[8] + v.w * matrix_[12];
    result.y = v.x * matrix_[1] + v.y * matrix_[5] + v.z * matrix_[9] + v.w * matrix_[13];
    result.z = v.x * matrix_[2] + v.y * matrix_[6] + v.z * matrix_[10] + v.w * matrix_[14];
    result.w = v.x * matrix_[3] + v.y * matrix_[7] + v.z * matrix_[11] + v.w * matrix_[15];
    return result;
}

// Matrix4 multiply (operator *=)
Matrix4 &Matrix4::operator*=(const Matrix4 &B)
{
    // [ row(A|this).column(B) ]
    Matrix4 AB;
    AB.matrix_[0] =
        matrix_[0] * B.matrix_[0] + matrix_[4] * B.matrix_[1] + matrix_[8] * B.matrix_[2] + matrix_[12] * B.matrix_[3];
    AB.matrix_[1] =
        matrix_[1] * B.matrix_[0] + matrix_[5] * B.matrix_[1] + matrix_[9] * B.matrix_[2] + matrix_[13] * B.matrix_[3];
    AB.matrix_[2] =
        matrix_[2] * B.matrix_[0] + matrix_[6] * B.matrix_[1] + matrix_[10] * B.matrix_[2] + matrix_[14] * B.matrix_[3];
    AB.matrix_[3] =
        matrix_[3] * B.matrix_[0] + matrix_[7] * B.matrix_[1] + matrix_[11] * B.matrix_[2] + matrix_[15] * B.matrix_[3];

    AB.matrix_[4] =
        matrix_[0] * B.matrix_[4] + matrix_[4] * B.matrix_[5] + matrix_[8] * B.matrix_[6] + matrix_[12] * B.matrix_[7];
    AB.matrix_[5] =
        matrix_[1] * B.matrix_[4] + matrix_[5] * B.matrix_[5] + matrix_[9] * B.matrix_[6] + matrix_[13] * B.matrix_[7];
    AB.matrix_[6] =
        matrix_[2] * B.matrix_[4] + matrix_[6] * B.matrix_[5] + matrix_[10] * B.matrix_[6] + matrix_[14] * B.matrix_[7];
    AB.matrix_[7] =
        matrix_[3] * B.matrix_[4] + matrix_[7] * B.matrix_[5] + matrix_[11] * B.matrix_[6] + matrix_[15] * B.matrix_[7];

    AB.matrix_[8] =
        matrix_[0] * B.matrix_[8] + matrix_[4] * B.matrix_[9] + matrix_[8] * B.matrix_[10] + matrix_[12] * B.matrix_[11];
    AB.matrix_[9] =
        matrix_[1] * B.matrix_[8] + matrix_[5] * B.matrix_[9] + matrix_[9] * B.matrix_[10] + matrix_[13] * B.matrix_[11];
    AB.matrix_[10] =
        matrix_[2] * B.matrix_[8] + matrix_[6] * B.matrix_[9] + matrix_[10] * B.matrix_[10] + matrix_[14] * B.matrix_[11];
    AB.matrix_[11] =
        matrix_[3] * B.matrix_[8] + matrix_[7] * B.matrix_[9] + matrix_[11] * B.matrix_[10] + matrix_[15] * B.matrix_[11];

    AB.matrix_[12] =
        matrix_[0] * B.matrix_[12] + matrix_[4] * B.matrix_[13] + matrix_[8] * B.matrix_[14] + matrix_[12] * B.matrix_[15];
    AB.matrix_[13] =
        matrix_[1] * B.matrix_[12] + matrix_[5] * B.matrix_[13] + matrix_[9] * B.matrix_[14] + matrix_[13] * B.matrix_[15];
    AB.matrix_[14] =
        matrix_[2] * B.matrix_[12] + matrix_[6] * B.matrix_[13] + matrix_[10] * B.matrix_[14] + matrix_[14] * B.matrix_[15];
    AB.matrix_[15] =
        matrix_[3] * B.matrix_[12] + matrix_[7] * B.matrix_[13] + matrix_[11] * B.matrix_[14] + matrix_[15] * B.matrix_[15];
    *this = AB;
    return *this;
}

// Array access (operator [])
double &Matrix4::operator[](int index) { return matrix_[index]; }

// Convert Matrix3 to Matrix4
void Matrix4::operator=(const Matrix3 &B)
{
    setColumn(0, B.columnAsVec3(0), 0.0);
    setColumn(1, B.columnAsVec3(1), 0.0);
    setColumn(2, B.columnAsVec3(2), 0.0);
}

// Pre-multiply this matrix by the supplied matrix
void Matrix4::preMultiply(const Matrix4 &B)
{
    // [ row(B).column(A|this) ]
    Matrix4 AB;
    AB.matrix_[0] =
        matrix_[0] * B.matrix_[0] + matrix_[1] * B.matrix_[4] + matrix_[2] * B.matrix_[8] + matrix_[3] * B.matrix_[12];
    AB.matrix_[1] =
        matrix_[0] * B.matrix_[1] + matrix_[1] * B.matrix_[5] + matrix_[2] * B.matrix_[9] + matrix_[3] * B.matrix_[13];
    AB.matrix_[2] =
        matrix_[0] * B.matrix_[2] + matrix_[1] * B.matrix_[6] + matrix_[2] * B.matrix_[10] + matrix_[3] * B.matrix_[14];
    AB.matrix_[3] =
        matrix_[0] * B.matrix_[3] + matrix_[1] * B.matrix_[7] + matrix_[2] * B.matrix_[11] + matrix_[3] * B.matrix_[15];

    AB.matrix_[4] =
        matrix_[4] * B.matrix_[0] + matrix_[5] * B.matrix_[4] + matrix_[6] * B.matrix_[8] + matrix_[7] * B.matrix_[12];
    AB.matrix_[5] =
        matrix_[4] * B.matrix_[1] + matrix_[5] * B.matrix_[5] + matrix_[6] * B.matrix_[9] + matrix_[7] * B.matrix_[13];
    AB.matrix_[6] =
        matrix_[4] * B.matrix_[2] + matrix_[5] * B.matrix_[6] + matrix_[6] * B.matrix_[10] + matrix_[7] * B.matrix_[14];
    AB.matrix_[7] =
        matrix_[4] * B.matrix_[3] + matrix_[5] * B.matrix_[7] + matrix_[6] * B.matrix_[11] + matrix_[7] * B.matrix_[15];

    AB.matrix_[8] =
        matrix_[8] * B.matrix_[0] + matrix_[9] * B.matrix_[4] + matrix_[10] * B.matrix_[8] + matrix_[11] * B.matrix_[12];
    AB.matrix_[9] =
        matrix_[8] * B.matrix_[1] + matrix_[9] * B.matrix_[5] + matrix_[10] * B.matrix_[9] + matrix_[11] * B.matrix_[13];
    AB.matrix_[10] =
        matrix_[8] * B.matrix_[2] + matrix_[9] * B.matrix_[6] + matrix_[10] * B.matrix_[10] + matrix_[11] * B.matrix_[14];
    AB.matrix_[11] =
        matrix_[8] * B.matrix_[3] + matrix_[9] * B.matrix_[7] + matrix_[10] * B.matrix_[11] + matrix_[11] * B.matrix_[15];

    AB.matrix_[12] =
        matrix_[12] * B.matrix_[0] + matrix_[13] * B.matrix_[4] + matrix_[14] * B.matrix_[8] + matrix_[15] * B.matrix_[12];
    AB.matrix_[13] =
        matrix_[12] * B.matrix_[1] + matrix_[13] * B.matrix_[5] + matrix_[14] * B.matrix_[9] + matrix_[15] * B.matrix_[13];
    AB.matrix_[14] =
        matrix_[12] * B.matrix_[2] + matrix_[13] * B.matrix_[6] + matrix_[14] * B.matrix_[10] + matrix_[15] * B.matrix_[14];
    AB.matrix_[15] =
        matrix_[12] * B.matrix_[3] + matrix_[13] * B.matrix_[7] + matrix_[14] * B.matrix_[11] + matrix_[15] * B.matrix_[15];
    *this = AB;
}

/*
 * Basic Set/Get
 */

// Reset to the identity matrix
void Matrix4::setIdentity()
{
    matrix_[0] = 1.0;
    matrix_[1] = 0.0;
    matrix_[2] = 0.0;
    matrix_[3] = 0.0;
    matrix_[4] = 0.0;
    matrix_[5] = 1.0;
    matrix_[6] = 0.0;
    matrix_[7] = 0.0;
    matrix_[8] = 0.0;
    matrix_[9] = 0.0;
    matrix_[10] = 1.0;
    matrix_[11] = 0.0;
    matrix_[12] = 0.0;
    matrix_[13] = 0.0;
    matrix_[14] = 0.0;
    matrix_[15] = 1.0;
}

// Print matrix
void Matrix4::print() const
{
    Messenger::print("CMaj   [0123]    [4567]    [8901] Translate\n");
    Messenger::print("        {:8.4f} {:8.4f} {:8.4f} {:8.4f}\n", matrix_[0], matrix_[4], matrix_[8], matrix_[12]);
    Messenger::print("        {:8.4f} {:8.4f} {:8.4f} {:8.4f}\n", matrix_[1], matrix_[5], matrix_[9], matrix_[13]);
    Messenger::print("        {:8.4f} {:8.4f} {:8.4f} {:8.4f}\n", matrix_[2], matrix_[6], matrix_[10], matrix_[14]);
    Messenger::print("Scale   {:8.4f} {:8.4f} {:8.4f} {:8.4f}\n", matrix_[3], matrix_[7], matrix_[11], matrix_[15]);
}

// Set zero matrix
void Matrix4::zero()
{
    matrix_[0] = 0.0;
    matrix_[1] = 0.0;
    matrix_[2] = 0.0;
    matrix_[3] = 0.0;
    matrix_[4] = 0.0;
    matrix_[5] = 0.0;
    matrix_[6] = 0.0;
    matrix_[7] = 0.0;
    matrix_[8] = 0.0;
    matrix_[9] = 0.0;
    matrix_[10] = 0.0;
    matrix_[11] = 0.0;
    matrix_[12] = 0.0;
    matrix_[13] = 0.0;
    matrix_[14] = 0.0;
    matrix_[15] = 0.0;
}

// Return matrix array
const double *Matrix4::matrix() const { return matrix_; }

// Return transpose of current matrix
Matrix4 &Matrix4::transpose() const
{
    static Matrix4 A;
    A.matrix_[0] = matrix_[0];
    A.matrix_[1] = matrix_[4];
    A.matrix_[2] = matrix_[8];
    A.matrix_[3] = matrix_[12];
    A.matrix_[4] = matrix_[1];
    A.matrix_[5] = matrix_[5];
    A.matrix_[6] = matrix_[9];
    A.matrix_[7] = matrix_[13];
    A.matrix_[8] = matrix_[2];
    A.matrix_[9] = matrix_[6];
    A.matrix_[10] = matrix_[10];
    A.matrix_[11] = matrix_[14];
    A.matrix_[12] = matrix_[3];
    A.matrix_[13] = matrix_[7];
    A.matrix_[14] = matrix_[11];
    A.matrix_[15] = matrix_[15];
    return A;
}

// Calculate determinant
double Matrix4::determinant() const
{
    double a = matrix_[0] * (matrix_[5] * (matrix_[10] * matrix_[15] - matrix_[11] * matrix_[14]) -
                             matrix_[9] * (matrix_[6] * matrix_[15] - matrix_[7] * matrix_[14]) +
                             matrix_[13] * (matrix_[6] * matrix_[11] - matrix_[7] * matrix_[10]));
    double b = matrix_[4] * (matrix_[1] * (matrix_[10] * matrix_[15] - matrix_[11] * matrix_[14]) -
                             matrix_[9] * (matrix_[2] * matrix_[15] - matrix_[3] * matrix_[14]) +
                             matrix_[13] * (matrix_[2] * matrix_[11] - matrix_[3] * matrix_[10]));
    double c = matrix_[8] * (matrix_[1] * (matrix_[6] * matrix_[15] - matrix_[7] * matrix_[14]) -
                             matrix_[5] * (matrix_[2] * matrix_[15] - matrix_[3] * matrix_[14]) +
                             matrix_[13] * (matrix_[2] * matrix_[7] - matrix_[3] * matrix_[6]));
    double d = matrix_[12] * (matrix_[1] * (matrix_[6] * matrix_[11] - matrix_[7] * matrix_[10]) -
                              matrix_[5] * (matrix_[2] * matrix_[11] - matrix_[3] * matrix_[10]) +
                              matrix_[9] * (matrix_[2] * matrix_[7] - matrix_[3] * matrix_[6]));
    return (a - b + c - d);
}

// Invert matrix
void Matrix4::invert()
{
    // Gauss-Jordan Inversion
    // Invert the supplied matrix using Gauss-Jordan elimination
    std::array<int, 4> pivotcols{0}, pivotrows{0};
    std::array<bool, 4> pivoted{false};
    int pivotrow = 0, pivotcol = 0;
    int row, col, m;
    double large, element;

    // Loop over columns to be reduced
    for (auto n = 0; n < 4; ++n)
    {
        // Locate suitable pivot element - find largest value in the matrix A
        large = 0.0;
        for (row = 0; row < 4; ++row)
        {
            // Only search this row if it has not previously contained a pivot element
            if (pivoted[row])
                continue;
            for (col = 0; col < 4; ++col)
            {
                // Similarly, only look at the column element if the column hasn't been pivoted yet.
                if (pivoted[col])
                    continue;
                // Check the size of the element...
                element = fabs(matrix_[row * 4 + col]);
                if (element > large)
                {
                    large = element;
                    pivotrow = row;
                    pivotcol = col;
                }
            }
        }

        // Mark the pivot row/column as changed
        pivoted[pivotcol] = true;
        pivotrows[n] = pivotrow;
        pivotcols[n] = pivotcol;

        // Exchange rows to put pivot element on the diagonal
        if (pivotrow != pivotcol)
        {
            for (m = 0; m < 4; ++m)
            {
                element = matrix_[pivotrow * 4 + m];
                matrix_[pivotrow * 4 + m] = matrix_[pivotcol * 4 + m];
                matrix_[pivotcol * 4 + m] = element;
            }
        }

        // Now ready to divide through row elements.
        element = 1.0 / matrix_[pivotcol * 4 + pivotcol];
        matrix_[pivotcol * 4 + pivotcol] = 1.0;
        for (m = 0; m < 4; ++m)
            matrix_[pivotcol * 4 + m] *= element;

        // Divide through other rows by the relevant multiple of the pivot row
        for (row = 0; row < 4; ++row)
        {
            if (row == pivotcol)
                continue;
            element = matrix_[row * 4 + pivotcol];
            matrix_[row * 4 + pivotcol] = 0.0;
            for (m = 0; m < 4; ++m)
                matrix_[row * 4 + m] = matrix_[row * 4 + m] - matrix_[pivotcol * 4 + m] * element;
        }
    }
    // Rearrange columns to undo row exchanges performed earlier
    for (auto i = 3; i >= 0; --i)
    {
        if (pivotrows[i] != pivotcols[i])
            for (m = 0; m < 4; ++m)
            {
                element = matrix_[m * 4 + pivotrows[i]];
                matrix_[m * 4 + pivotrows[i]] = matrix_[m * 4 + pivotcols[i]];
                matrix_[m * 4 + pivotcols[i]] = element;
            }
    }
}

/*
 * Column Operations
 */

// Copy column contents to supplied Vec3
Vec3<double> Matrix4::columnAsVec3(int col) const
{
    Vec3<double> vec(matrix_[col * 4], matrix_[col * 4 + 1], matrix_[col * 4 + 2]);
    return vec;
}

// Copy column contents to supplied Vec4
Vec4<double> Matrix4::columnAsVec4(int col) const
{
    Vec4<double> vec(matrix_[col * 4], matrix_[col * 4 + 1], matrix_[col * 4 + 2], matrix_[col * 4 + 3]);
    return vec;
}

// Set specified row from supplied triplet of values
void Matrix4::setRow(int row, double x, double y, double z)
{
    matrix_[row] = x;
    matrix_[4 + row] = y;
    matrix_[8 + row] = z;
}

// Set specified row from supplied values
void Matrix4::setRow(int row, double x, double y, double z, double w)
{
    matrix_[row] = x;
    matrix_[4 + row] = y;
    matrix_[8 + row] = z;
    matrix_[12 + row] = w;
}

// Set specified column from supplied values
void Matrix4::setColumn(int col, double a, double b, double c, double d)
{
    matrix_[col * 4] = a;
    matrix_[col * 4 + 1] = b;
    matrix_[col * 4 + 2] = c;
    matrix_[col * 4 + 3] = d;
}

// Set specified column from supplied Vec3
void Matrix4::setColumn(int col, Vec3<double> vec, double w)
{
    matrix_[col * 4] = vec.x;
    matrix_[col * 4 + 1] = vec.y;
    matrix_[col * 4 + 2] = vec.z;
    matrix_[col * 4 + 3] = w;
}

// Set specified column from supplied Vec4
void Matrix4::setColumn(int col, Vec4<double> vec)
{
    matrix_[col * 4] = vec.x;
    matrix_[col * 4 + 1] = vec.y;
    matrix_[col * 4 + 2] = vec.z;
    matrix_[col * 4 + 3] = vec.w;
}

// Set specified column from supplied Vec3
void Matrix4::setColumn(int col, Vec3<double> vec)
{
    matrix_[col * 4] = vec.x;
    matrix_[col * 4 + 1] = vec.y;
    matrix_[col * 4 + 2] = vec.z;
}

// Adjust specified column from supplied values
void Matrix4::adjustColumn(int col, double a, double b, double c, double d)
{
    matrix_[col * 4] += a;
    matrix_[col * 4 + 1] += b;
    matrix_[col * 4 + 2] += c;
    matrix_[col * 4 + 3] += d;
}

// Adjust specified column from supplied Vec3
void Matrix4::adjustColumn(int col, Vec3<double> vec, double w)
{
    matrix_[col * 4] += vec.x;
    matrix_[col * 4 + 1] += vec.y;
    matrix_[col * 4 + 2] += vec.z;
    matrix_[col * 4 + 3] += w;
}

// Adjust specified column from supplied Vec4
void Matrix4::adjustColumn(int col, Vec4<double> vec)
{
    matrix_[col * 4] += vec.x;
    matrix_[col * 4 + 1] += vec.y;
    matrix_[col * 4 + 2] += vec.z;
    matrix_[col * 4 + 3] += vec.w;
}

// Calculate column magnitude
double Matrix4::columnMagnitude(int column) const
{
    double mag = 0.0;
    for (auto n = column * 4; n < column * 4 + 4; ++n)
        mag += (matrix_[n] * matrix_[n]);
    return sqrt(mag);
}

// Multiply column by single value
void Matrix4::columnMultiply(int col, double d)
{
    matrix_[col * 4] *= d;
    matrix_[col * 4 + 1] *= d;
    matrix_[col * 4 + 2] *= d;
    matrix_[col * 4 + 3] *= d;
}

// Multiply first three columns by values in supplied vector
void Matrix4::columnMultiply(Vec3<double> vec)
{
    columnMultiply(0, vec.x);
    columnMultiply(1, vec.y);
    columnMultiply(2, vec.z);
}

// Normalise specified column to 1
void Matrix4::columnNormalise(int col)
{
    auto mag = 1.0 / sqrt(matrix_[col * 4] * matrix_[col * 4] + matrix_[col * 4 + 1] * matrix_[col * 4 + 1] +
                          matrix_[col * 4 + 2] * matrix_[col * 4 + 2] + matrix_[col * 4 + 3] * matrix_[col * 4 + 3]);
    matrix_[col * 4] *= mag;
    matrix_[col * 4 + 1] *= mag;
    matrix_[col * 4 + 2] *= mag;
    matrix_[col * 4 + 3] *= mag;
}

// Orthogonalise rotation matrix column w.r.t. one (or two) other columns)
void Matrix4::orthogonaliseColumn(int targetcol, int orthocol1, int orthocol2)
{
    // Grab target column
    auto v = columnAsVec3(targetcol);
    // Orthogonalising w.r.t one or two other vectors?
    if (orthocol2 == -1)
    {
        auto source = columnAsVec3(orthocol1);
        double sourcemag = source.magnitude();
        double dpovermagsq = v.dp(source) / (sourcemag * sourcemag);
        v.x -= dpovermagsq * source.x;
        v.y -= dpovermagsq * source.y;
        v.z -= dpovermagsq * source.z;
    }
    else
    {
        // This routine actually generates the orthogonal vector via the cross-product
        // We also calculate the scalar resolute (dp) to ensure the new vector points in the same direction
        auto source1 = columnAsVec3(orthocol1), source2 = columnAsVec3(orthocol2);
        auto newvec = source1 * source2;
        newvec.normalise();
        double dp = newvec.dp(v);
        if (dp < 0.0)
            newvec *= -1.0;
        v = newvec;
    }
    setColumn(targetcol, v, matrix_[targetcol * 4 + 3]);
}

/*
 * Rotations
 */

// Create rotation matrix about X
void Matrix4::createRotationX(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD;
    cosx = cos(theta);
    sinx = sin(theta);
    matrix_[0] = 1.0;
    matrix_[1] = 0.0;
    matrix_[2] = 0.0;
    matrix_[3] = 0.0;
    matrix_[4] = 0.0;
    matrix_[5] = cosx;
    matrix_[6] = -sinx;
    matrix_[7] = 0.0;
    matrix_[8] = 0.0;
    matrix_[9] = sinx;
    matrix_[10] = cosx;
    matrix_[11] = 0.0;
    matrix_[12] = 0.0;
    matrix_[13] = 0.0;
    matrix_[14] = 0.0;
    matrix_[15] = 1.0;
}

// Create XY rotation matrix
void Matrix4::createRotationXY(double anglex, double angley)
{
    double cosx, sinx, cosy, siny, thetax = anglex / DEGRAD, thetay = angley / DEGRAD;
    cosx = cos(thetax);
    cosy = cos(thetay);
    sinx = sin(thetax);
    siny = sin(thetay);
    matrix_[0] = cosy;
    matrix_[1] = (-sinx) * (-siny);
    matrix_[2] = -siny * cosx;
    matrix_[3] = 0.0;
    matrix_[4] = 0.0;
    matrix_[5] = cosx;
    matrix_[6] = sinx;
    matrix_[7] = 0.0;
    matrix_[8] = siny;
    matrix_[9] = (-sinx) * cosy;
    matrix_[10] = cosx * cosy;
    matrix_[11] = 0.0;
    matrix_[12] = 0.0;
    matrix_[13] = 0.0;
    matrix_[14] = 0.0;
    matrix_[15] = 1.0;
}

// Create rotation matrix about Y
void Matrix4::createRotationY(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD;
    cosx = cos(theta);
    sinx = sin(theta);
    matrix_[0] = cosx;
    matrix_[1] = 0.0;
    matrix_[2] = sinx;
    matrix_[3] = 0.0;
    matrix_[4] = 0.0;
    matrix_[5] = 1.0;
    matrix_[6] = 0.0;
    matrix_[7] = 0.0;
    matrix_[8] = -sinx;
    matrix_[9] = 0.0;
    matrix_[10] = cosx;
    matrix_[11] = 0.0;
    matrix_[12] = 0.0;
    matrix_[13] = 0.0;
    matrix_[14] = 0.0;
    matrix_[15] = 1.0;
}

// Create rotation matrix about Z
void Matrix4::createRotationZ(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD;
    cosx = cos(theta);
    sinx = sin(theta);
    matrix_[0] = cosx;
    matrix_[1] = -sinx;
    matrix_[2] = 0.0;
    matrix_[3] = 0.0;
    matrix_[4] = sinx;
    matrix_[5] = cosx;
    matrix_[6] = 0.0;
    matrix_[7] = 0.0;
    matrix_[8] = 0.0;
    matrix_[9] = 0.0;
    matrix_[10] = 1.0;
    matrix_[11] = 0.0;
    matrix_[12] = 0.0;
    matrix_[13] = 0.0;
    matrix_[14] = 0.0;
    matrix_[15] = 1.0;
}

// Create axis rotation quaternion
void Matrix4::createRotationAxis(double ax, double ay, double az, double angle, bool normalise)
{
    double cosx, sinx, theta = angle / DEGRAD;
    if (normalise)
    {
        double mag = sqrt(ax * ax + ay * ay + az * az);
        ax /= mag;
        ay /= mag;
        az /= mag;
    }
    cosx = cos(theta);
    sinx = sin(theta);
    matrix_[0] = ax * ax * (1.0 - cosx) + cosx;
    matrix_[1] = ax * ay * (1.0 - cosx) - az * sinx;
    matrix_[2] = ax * az * (1.0 - cosx) + ay * sinx;
    matrix_[3] = 0.0;
    matrix_[4] = ax * ay * (1.0 - cosx) + az * sinx;
    matrix_[5] = ay * ay * (1.0 - cosx) + cosx;
    matrix_[6] = ay * az * (1.0 - cosx) - ax * sinx;
    matrix_[7] = 0.0;
    matrix_[8] = ax * az * (1.0 - cosx) - ay * sinx;
    matrix_[9] = ay * az * (1.0 - cosx) + ax * sinx;
    matrix_[10] = az * az * (1.0 - cosx) + cosx;
    matrix_[11] = 0.0;
    matrix_[12] = 0.0;
    matrix_[13] = 0.0;
    matrix_[14] = 0.0;
    matrix_[15] = 1.0;
}

// Apply rotation about X axis
void Matrix4::applyRotationX(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD, temp[4];
    cosx = cos(theta);
    sinx = sin(theta);

    // Recalculate second column and store in temp values
    temp[0] = matrix_[4] * cosx + matrix_[8] * -sinx;
    temp[1] = matrix_[5] * cosx + matrix_[9] * -sinx;
    temp[2] = matrix_[6] * cosx + matrix_[10] * -sinx;
    temp[3] = matrix_[7] * cosx + matrix_[11] * -sinx;

    matrix_[8] = matrix_[4] * sinx + matrix_[8] * cosx;
    matrix_[9] = matrix_[5] * sinx + matrix_[9] * cosx;
    matrix_[10] = matrix_[6] * sinx + matrix_[10] * cosx;
    matrix_[11] = matrix_[7] * sinx + matrix_[11] * cosx;

    matrix_[4] = temp[0];
    matrix_[5] = temp[1];
    matrix_[6] = temp[2];
    matrix_[7] = temp[3];
}

// Apply rotation about Y axis
void Matrix4::applyRotationY(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD, temp[4];
    cosx = cos(theta);
    sinx = sin(theta);

    temp[0] = matrix_[0] * cosx + matrix_[2] * -sinx;
    temp[1] = matrix_[4] * cosx + matrix_[6] * -sinx;
    temp[2] = matrix_[8] * cosx + matrix_[10] * -sinx;
    temp[3] = matrix_[12] * cosx + matrix_[14] * -sinx;

    matrix_[2] = matrix_[0] * sinx + matrix_[2] * cosx;
    matrix_[6] = matrix_[4] * sinx + matrix_[6] * cosx;
    matrix_[10] = matrix_[8] * sinx + matrix_[10] * cosx;
    matrix_[14] = matrix_[12] * sinx + matrix_[14] * cosx;

    matrix_[0] = temp[0];
    matrix_[4] = temp[1];
    matrix_[8] = temp[2];
    matrix_[12] = temp[3];
}

// Apply rotation about Z axis
void Matrix4::applyRotationZ(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD, temp[4];
    cosx = cos(theta);
    sinx = sin(theta);

    temp[0] = matrix_[0] * cosx + matrix_[1] * sinx;
    temp[1] = matrix_[4] * cosx + matrix_[5] * sinx;
    temp[2] = matrix_[8] * cosx + matrix_[9] * sinx;
    temp[3] = matrix_[12] * cosx + matrix_[13] * sinx;

    matrix_[1] = matrix_[0] * -sinx + matrix_[1] * cosx;
    matrix_[5] = matrix_[4] * -sinx + matrix_[5] * cosx;
    matrix_[9] = matrix_[8] * -sinx + matrix_[9] * cosx;
    matrix_[13] = matrix_[12] * -sinx + matrix_[13] * cosx;

    matrix_[0] = temp[0];
    matrix_[4] = temp[1];
    matrix_[8] = temp[2];
    matrix_[12] = temp[3];
}

// Apply axis rotation quaternion
void Matrix4::applyRotationAxis(double ax, double ay, double az, double angle, bool normalise)
{
    double cosx, sinx, theta = angle / DEGRAD, temp[8], multipliers[16];
    if (normalise)
    {
        double mag = sqrt(ax * ax + ay * ay + az * az);
        ax /= mag;
        ay /= mag;
        az /= mag;
    }
    cosx = cos(theta);
    sinx = sin(theta);
    multipliers[0] = ax * ax * (1.0 - cosx) + cosx;
    multipliers[1] = ax * ay * (1.0 - cosx) + az * sinx;
    multipliers[2] = ax * az * (1.0 - cosx) - ay * sinx;
    multipliers[4] = ax * ay * (1.0 - cosx) - az * sinx;
    multipliers[5] = ay * ay * (1.0 - cosx) + cosx;
    multipliers[6] = ay * az * (1.0 - cosx) + ax * sinx;
    multipliers[8] = ax * az * (1.0 - cosx) + ay * sinx;
    multipliers[9] = ay * az * (1.0 - cosx) - ax * sinx;
    multipliers[10] = az * az * (1.0 - cosx) + cosx;

    temp[0] = matrix_[0] * multipliers[0] + matrix_[4] * multipliers[1] + matrix_[8] * multipliers[2];
    temp[1] = matrix_[1] * multipliers[0] + matrix_[5] * multipliers[1] + matrix_[9] * multipliers[2];
    temp[2] = matrix_[2] * multipliers[0] + matrix_[6] * multipliers[1] + matrix_[10] * multipliers[2];
    temp[3] = matrix_[3] * multipliers[0] + matrix_[7] * multipliers[1] + matrix_[11] * multipliers[2];

    temp[4] = matrix_[0] * multipliers[4] + matrix_[4] * multipliers[5] + matrix_[8] * multipliers[6];
    temp[5] = matrix_[1] * multipliers[4] + matrix_[5] * multipliers[5] + matrix_[9] * multipliers[6];
    temp[6] = matrix_[2] * multipliers[4] + matrix_[6] * multipliers[5] + matrix_[10] * multipliers[6];
    temp[7] = matrix_[3] * multipliers[4] + matrix_[7] * multipliers[5] + matrix_[11] * multipliers[6];

    matrix_[8] = matrix_[0] * multipliers[8] + matrix_[4] * multipliers[9] + matrix_[8] * multipliers[10];
    matrix_[9] = matrix_[1] * multipliers[8] + matrix_[5] * multipliers[9] + matrix_[9] * multipliers[10];
    matrix_[10] = matrix_[2] * multipliers[8] + matrix_[6] * multipliers[9] + matrix_[10] * multipliers[10];
    matrix_[11] = matrix_[3] * multipliers[8] + matrix_[7] * multipliers[9] + matrix_[11] * multipliers[10];

    matrix_[0] = temp[0];
    matrix_[1] = temp[1];
    matrix_[2] = temp[2];
    matrix_[3] = temp[3];
    matrix_[4] = temp[4];
    matrix_[5] = temp[5];
    matrix_[6] = temp[6];
    matrix_[7] = temp[7];
}

// Apply rotation about X axis, premultiplying with current matrix
void Matrix4::applyPreRotationX(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD, temp[4];
    cosx = cos(theta);
    sinx = sin(theta);

    // Recalculate second column and store in temp array
    temp[0] = matrix_[1] * cosx + matrix_[2] * sinx;
    temp[1] = matrix_[5] * cosx + matrix_[6] * sinx;
    temp[2] = matrix_[9] * cosx + matrix_[10] * sinx;
    temp[3] = matrix_[13] * cosx + matrix_[14] * sinx;

    // Set third column immediately
    matrix_[2] = matrix_[1] * -sinx + matrix_[2] * cosx;
    matrix_[6] = matrix_[5] * -sinx + matrix_[6] * cosx;
    matrix_[10] = matrix_[9] * -sinx + matrix_[10] * cosx;
    matrix_[14] = matrix_[13] * -sinx + matrix_[14] * cosx;

    // Put temporary values back into first column
    matrix_[1] = temp[0];
    matrix_[5] = temp[1];
    matrix_[9] = temp[2];
    matrix_[13] = temp[3];
}

// Apply rotation about Y axis, premultiplying with current matrix
void Matrix4::applyPreRotationY(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD, temp[4];
    cosx = cos(theta);
    sinx = sin(theta);

    temp[0] = matrix_[0] * cosx + matrix_[2] * -sinx;
    temp[1] = matrix_[4] * cosx + matrix_[6] * -sinx;
    temp[2] = matrix_[8] * cosx + matrix_[10] * -sinx;
    temp[3] = matrix_[12] * cosx + matrix_[14] * -sinx;

    matrix_[2] = matrix_[0] * sinx + matrix_[2] * cosx;
    matrix_[6] = matrix_[4] * sinx + matrix_[6] * cosx;
    matrix_[10] = matrix_[8] * sinx + matrix_[10] * cosx;
    matrix_[14] = matrix_[12] * sinx + matrix_[14] * cosx;

    matrix_[0] = temp[0];
    matrix_[4] = temp[1];
    matrix_[8] = temp[2];
    matrix_[12] = temp[3];
}

// Apply rotation about Z axis, premultiplying with current matrix
void Matrix4::applyPreRotationZ(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD, temp[4];
    cosx = cos(theta);
    sinx = sin(theta);

    temp[0] = matrix_[0] * cosx + matrix_[1] * sinx;
    temp[1] = matrix_[4] * cosx + matrix_[5] * sinx;
    temp[2] = matrix_[8] * cosx + matrix_[9] * sinx;
    temp[3] = matrix_[12] * cosx + matrix_[13] * sinx;

    matrix_[1] = matrix_[0] * -sinx + matrix_[1] * cosx;
    matrix_[5] = matrix_[4] * -sinx + matrix_[5] * cosx;
    matrix_[9] = matrix_[8] * -sinx + matrix_[9] * cosx;
    matrix_[13] = matrix_[12] * -sinx + matrix_[13] * cosx;

    matrix_[0] = temp[0];
    matrix_[4] = temp[1];
    matrix_[8] = temp[2];
    matrix_[12] = temp[3];
}

// Apply axis rotation quaternion, premultiplying with current matrix
void Matrix4::applyPreRotationAxis(double ax, double ay, double az, double angle, bool normalise)
{
    double cosx, sinx, theta = angle / DEGRAD, temp[8], multipliers[16];
    if (normalise)
    {
        double mag = sqrt(ax * ax + ay * ay + az * az);
        ax /= mag;
        ay /= mag;
        az /= mag;
    }
    cosx = cos(theta);
    sinx = sin(theta);
    multipliers[0] = ax * ax * (1.0 - cosx) + cosx;
    multipliers[1] = ax * ay * (1.0 - cosx) + az * sinx;
    multipliers[2] = ax * az * (1.0 - cosx) - ay * sinx;
    multipliers[4] = ax * ay * (1.0 - cosx) - az * sinx;
    multipliers[5] = ay * ay * (1.0 - cosx) + cosx;
    multipliers[6] = ay * az * (1.0 - cosx) + ax * sinx;
    multipliers[8] = ax * az * (1.0 - cosx) + ay * sinx;
    multipliers[9] = ay * az * (1.0 - cosx) - ax * sinx;
    multipliers[10] = az * az * (1.0 - cosx) + cosx;

    temp[0] = matrix_[0] * multipliers[0] + matrix_[1] * multipliers[4] + matrix_[2] * multipliers[8];
    temp[1] = matrix_[4] * multipliers[0] + matrix_[5] * multipliers[4] + matrix_[6] * multipliers[8];
    temp[2] = matrix_[8] * multipliers[0] + matrix_[9] * multipliers[4] + matrix_[10] * multipliers[8];
    temp[3] = matrix_[12] * multipliers[0] + matrix_[13] * multipliers[4] + matrix_[14] * multipliers[8];

    temp[4] = matrix_[0] * multipliers[1] + matrix_[1] * multipliers[5] + matrix_[2] * multipliers[9];
    temp[5] = matrix_[4] * multipliers[1] + matrix_[5] * multipliers[5] + matrix_[6] * multipliers[9];
    temp[6] = matrix_[8] * multipliers[1] + matrix_[9] * multipliers[5] + matrix_[10] * multipliers[9];
    temp[7] = matrix_[12] * multipliers[1] + matrix_[13] * multipliers[5] + matrix_[14] * multipliers[9];

    matrix_[2] = matrix_[0] * multipliers[2] + matrix_[1] * multipliers[6] + matrix_[2] * multipliers[10];
    matrix_[6] = matrix_[4] * multipliers[2] + matrix_[5] * multipliers[6] + matrix_[6] * multipliers[10];
    matrix_[10] = matrix_[8] * multipliers[2] + matrix_[9] * multipliers[6] + matrix_[10] * multipliers[10];
    matrix_[14] = matrix_[12] * multipliers[2] + matrix_[13] * multipliers[6] + matrix_[14] * multipliers[10];

    matrix_[0] = temp[0];
    matrix_[4] = temp[1];
    matrix_[8] = temp[2];
    matrix_[12] = temp[3];
    matrix_[1] = temp[4];
    matrix_[5] = temp[5];
    matrix_[9] = temp[6];
    matrix_[13] = temp[7];
}

// Set rotation matrix
void Matrix4::setRotation(const Matrix3 &rotation)
{
    setColumn(0, rotation.columnAsVec3(0));
    setColumn(1, rotation.columnAsVec3(1));
    setColumn(2, rotation.columnAsVec3(2));
}

// Apply rotation matrix
void Matrix4::applyRotation(const Matrix3 &B)
{
    // [ row(A|this).column(B) ]
    Matrix4 AB;

    AB.matrix_[0] = matrix_[0] * B.value(0) + matrix_[4] * B.value(1) + matrix_[8] * B.value(2);
    AB.matrix_[1] = matrix_[1] * B.value(0) + matrix_[5] * B.value(1) + matrix_[9] * B.value(2);
    AB.matrix_[2] = matrix_[2] * B.value(0) + matrix_[6] * B.value(1) + matrix_[10] * B.value(2);
    AB.matrix_[3] = matrix_[3] * B.value(0) + matrix_[7] * B.value(1) + matrix_[11] * B.value(2);

    AB.matrix_[4] = matrix_[0] * B.value(3) + matrix_[4] * B.value(4) + matrix_[8] * B.value(5);
    AB.matrix_[5] = matrix_[1] * B.value(3) + matrix_[5] * B.value(4) + matrix_[9] * B.value(5);
    AB.matrix_[6] = matrix_[2] * B.value(3) + matrix_[6] * B.value(4) + matrix_[10] * B.value(5);
    AB.matrix_[7] = matrix_[3] * B.value(3) + matrix_[7] * B.value(4) + matrix_[11] * B.value(5);

    AB.matrix_[8] = matrix_[0] * B.value(6) + matrix_[4] * B.value(7) + matrix_[8] * B.value(8);
    AB.matrix_[9] = matrix_[1] * B.value(6) + matrix_[5] * B.value(7) + matrix_[9] * B.value(8);
    AB.matrix_[10] = matrix_[2] * B.value(6) + matrix_[6] * B.value(7) + matrix_[10] * B.value(8);
    AB.matrix_[11] = matrix_[3] * B.value(6) + matrix_[7] * B.value(7) + matrix_[11] * B.value(8);

    AB.matrix_[12] = matrix_[12];
    AB.matrix_[13] = matrix_[13];
    AB.matrix_[14] = matrix_[14];
    AB.matrix_[15] = matrix_[15];

    *this = AB;
}

/*
 * Translations
 */

// Create a translation matrix (as glTranslated would do)
void Matrix4::createTranslation(double dx, double dy, double dz)
{
    matrix_[0] = 1.0;
    matrix_[1] = 0.0;
    matrix_[2] = 0.0;
    matrix_[3] = 0.0;
    matrix_[4] = 0.0;
    matrix_[5] = 1.0;
    matrix_[6] = 0.0;
    matrix_[7] = 0.0;
    matrix_[8] = 0.0;
    matrix_[9] = 0.0;
    matrix_[10] = 1.0;
    matrix_[11] = 0.0;
    matrix_[12] = dx;
    matrix_[13] = dy;
    matrix_[14] = dz;
    matrix_[15] = 1.0;
}

// Create a translation matrix (as glTranslated would do)
void Matrix4::createTranslation(Vec3<double> delta) { createTranslation(delta.x, delta.y, delta.z); }

// Apply a translation to the matrix (as glTranslated would do)
void Matrix4::applyTranslation(double dx, double dy, double dz)
{
    matrix_[12] += matrix_[0] * dx + matrix_[4] * dy + matrix_[8] * dz;
    matrix_[13] += matrix_[1] * dx + matrix_[5] * dy + matrix_[9] * dz;
    matrix_[14] += matrix_[2] * dx + matrix_[6] * dy + matrix_[10] * dz;
}

// Apply a translation to the matrix (as glTranslated would to)
void Matrix4::applyTranslation(Vec3<double> vec)
{
    matrix_[12] += matrix_[0] * vec.x + matrix_[4] * vec.y + matrix_[8] * vec.z;
    matrix_[13] += matrix_[1] * vec.x + matrix_[5] * vec.y + matrix_[9] * vec.z;
    matrix_[14] += matrix_[2] * vec.x + matrix_[6] * vec.y + matrix_[10] * vec.z;
}

// Apply a translation, premultiplying with current matrix
void Matrix4::applyPreTranslation(double dx, double dy, double dz)
{
    matrix_[0] += dx * matrix_[3];
    matrix_[1] += dy * matrix_[3];
    matrix_[2] += dz * matrix_[3];

    matrix_[4] += dx * matrix_[7];
    matrix_[5] += dy * matrix_[7];
    matrix_[6] += dz * matrix_[7];

    matrix_[8] += dx * matrix_[11];
    matrix_[9] += dy * matrix_[11];
    matrix_[10] += dz * matrix_[11];

    matrix_[12] += dx;
    matrix_[13] += dy;
    matrix_[14] += dz;
}

// Apply a translation, premultiplying with current matrix
void Matrix4::applyPreTranslation(Vec3<double> vec) { applyPreTranslation(vec.x, vec.y, vec.z); }

// Apply an X-translation to the matrix (as glTranslated would do)
void Matrix4::applyTranslationX(double dx)
{
    matrix_[12] += matrix_[0] * dx;
    matrix_[13] += matrix_[1] * dx;
    matrix_[14] += matrix_[2] * dx;
}

// Apply an Y-translation to the matrix (as glTranslated would do)
void Matrix4::applyTranslationY(double dy)
{
    matrix_[12] += matrix_[4] * dy;
    matrix_[13] += matrix_[5] * dy;
    matrix_[14] += matrix_[6] * dy;
}

// Apply an Z-translation to the matrix (as glTranslated would do)
void Matrix4::applyTranslationZ(double dz)
{
    matrix_[12] += matrix_[8] * dz;
    matrix_[13] += matrix_[9] * dz;
    matrix_[14] += matrix_[10] * dz;
}

// Add a translation to the matrix
void Matrix4::addTranslation(Vec3<double> v)
{
    matrix_[12] += v.x;
    matrix_[13] += v.y;
    matrix_[14] += v.z;
}

// Add a translation to the matrix
void Matrix4::addTranslation(double dx, double dy, double dz)
{
    matrix_[12] += dx;
    matrix_[13] += dy;
    matrix_[14] += dz;
}

// Set translation in the matrix
void Matrix4::setTranslation(Vec3<double> v)
{
    matrix_[12] = v.x;
    matrix_[13] = v.y;
    matrix_[14] = v.z;
}

// Set translation in the matrix
void Matrix4::setTranslation(double x, double y, double z)
{
    matrix_[12] = x;
    matrix_[13] = y;
    matrix_[14] = z;
}

/*
 * Scaling
 */

// Apply a general scaling to the matrix
void Matrix4::applyScaling(double scalex, double scaley, double scalez)
{
    applyScalingX(scalex);
    applyScalingY(scaley);
    applyScalingZ(scalez);
}

// Apply a general scaling to the matrix
void Matrix4::applyScaling(Vec3<double> scaling)
{
    applyScalingX(scaling.x);
    applyScalingY(scaling.y);
    applyScalingZ(scaling.z);
}

// Apply a general scaling to the matrix
void Matrix4::applyScaling(double scale)
{
    applyScalingX(scale);
    applyScalingY(scale);
    applyScalingZ(scale);
}

// Apply an xy-scaling to the matrix
void Matrix4::applyScalingXY(double scalex, double scaley)
{
    applyScalingX(scalex);
    applyScalingY(scaley);
}

// Apply an x scaling to the matrix
void Matrix4::applyScalingX(double scale)
{
    matrix_[0] *= scale;
    matrix_[1] *= scale;
    matrix_[2] *= scale;
    matrix_[3] *= scale;
}

// Apply a y scaling to the matrix
void Matrix4::applyScalingY(double scale)
{
    matrix_[4] *= scale;
    matrix_[5] *= scale;
    matrix_[6] *= scale;
    matrix_[7] *= scale;
}

// Apply a z scaling to the matrix
void Matrix4::applyScalingZ(double scale)
{
    matrix_[8] *= scale;
    matrix_[9] *= scale;
    matrix_[10] *= scale;
    matrix_[11] *= scale;
}

/*
 * Transforms
 */

// Transform coordinates supplied and return as Vec3<double>
Vec3<double> Matrix4::transform(double x, double y, double z) const
{
    Vec3<double> result;
    result.x = x * matrix_[0] + y * matrix_[4] + z * matrix_[8] + matrix_[12];
    result.y = x * matrix_[1] + y * matrix_[5] + z * matrix_[9] + matrix_[13];
    result.z = x * matrix_[2] + y * matrix_[6] + z * matrix_[10] + matrix_[14];
    return result;
}

// Transform coordinates supplied and return as Vec3<double>
Vec4<double> Matrix4::transform(double x, double y, double z, double w) const
{
    Vec4<double> result;
    result.x = x * matrix_[0] + y * matrix_[4] + z * matrix_[8] + w * matrix_[12];
    result.y = x * matrix_[1] + y * matrix_[5] + z * matrix_[9] + w * matrix_[13];
    result.z = x * matrix_[2] + y * matrix_[6] + z * matrix_[10] + w * matrix_[14];
    result.w = x * matrix_[3] + y * matrix_[7] + z * matrix_[11] + w * matrix_[15];
    return result;
}

// Transform coordinates supplied and return as Vec3<double>
Vec3<double> Matrix4::transform(Vec3<double> vec) const
{
    Vec3<double> result;
    result.x = vec.x * matrix_[0] + vec.y * matrix_[4] + vec.z * matrix_[8] + matrix_[12];
    result.y = vec.x * matrix_[1] + vec.y * matrix_[5] + vec.z * matrix_[9] + matrix_[13];
    result.z = vec.x * matrix_[2] + vec.y * matrix_[6] + vec.z * matrix_[10] + matrix_[14];
    return result;
}

// Multiply against other matrix, but only rotational part, keeping translation/scaling intact
void Matrix4::multiplyRotation(Matrix4 B)
{
    Matrix4 AB;
    AB.matrix_[0] = matrix_[0] * B.matrix_[0] + matrix_[4] * B.matrix_[1] + matrix_[8] * B.matrix_[2];
    AB.matrix_[1] = matrix_[1] * B.matrix_[0] + matrix_[5] * B.matrix_[1] + matrix_[9] * B.matrix_[2];
    AB.matrix_[2] = matrix_[2] * B.matrix_[0] + matrix_[6] * B.matrix_[1] + matrix_[10] * B.matrix_[2];

    AB.matrix_[4] = matrix_[0] * B.matrix_[4] + matrix_[4] * B.matrix_[5] + matrix_[8] * B.matrix_[6];
    AB.matrix_[5] = matrix_[1] * B.matrix_[4] + matrix_[5] * B.matrix_[5] + matrix_[9] * B.matrix_[6];
    AB.matrix_[6] = matrix_[2] * B.matrix_[4] + matrix_[6] * B.matrix_[5] + matrix_[10] * B.matrix_[6];

    AB.matrix_[8] = matrix_[0] * B.matrix_[8] + matrix_[4] * B.matrix_[9] + matrix_[8] * B.matrix_[10];
    AB.matrix_[9] = matrix_[1] * B.matrix_[8] + matrix_[5] * B.matrix_[9] + matrix_[9] * B.matrix_[10];
    AB.matrix_[10] = matrix_[2] * B.matrix_[8] + matrix_[6] * B.matrix_[9] + matrix_[10] * B.matrix_[10];

    matrix_[0] = AB.matrix_[0];
    matrix_[1] = AB.matrix_[1];
    matrix_[2] = AB.matrix_[2];
    matrix_[4] = AB.matrix_[4];
    matrix_[5] = AB.matrix_[5];
    matrix_[6] = AB.matrix_[6];
    matrix_[8] = AB.matrix_[8];
    matrix_[9] = AB.matrix_[9];
    matrix_[10] = AB.matrix_[10];
}

// Apply rotational part of matrix to supplied vector
Vec3<double> Matrix4::rotateVector(Vec3<double> &v) const
{
    Vec3<double> result;
    result.x = v.x * matrix_[0] + v.y * matrix_[4] + v.z * matrix_[8];
    result.y = v.x * matrix_[1] + v.y * matrix_[5] + v.z * matrix_[9];
    result.z = v.x * matrix_[2] + v.y * matrix_[6] + v.z * matrix_[10];
    return result;
}

// Apply rotational part of matrix to supplied vector coordinates
Vec3<double> Matrix4::rotateVector(double x, double y, double z) const
{
    Vec3<double> result;
    result.x = x * matrix_[0] + y * matrix_[4] + z * matrix_[8];
    result.y = x * matrix_[1] + y * matrix_[5] + z * matrix_[9];
    result.z = x * matrix_[2] + y * matrix_[6] + z * matrix_[10];
    return result;
}

// Remove translation and scaling parts, leaving rotation only
void Matrix4::removeTranslationAndScaling()
{
    matrix_[3] = 0.0;
    matrix_[7] = 0.0;
    matrix_[11] = 0.0;
    matrix_[15] = 1.0;
    matrix_[12] = 0.0;
    matrix_[13] = 0.0;
    matrix_[14] = 0.0;
}

// Copy translation and scaling parts from specified matrix
void Matrix4::copyTranslationAndScaling(Matrix4 &source)
{
    matrix_[3] = source.matrix_[3];
    matrix_[7] = source.matrix_[7];
    matrix_[11] = source.matrix_[11];
    matrix_[15] = source.matrix_[15];
    matrix_[12] = source.matrix_[12];
    matrix_[13] = source.matrix_[13];
    matrix_[14] = source.matrix_[14];
}

/*
 * Shearing
 */

// Apply a shearing along X
void Matrix4::applyShearX(double shearx)
{
    matrix_[4] += shearx * matrix_[0];
    matrix_[5] += shearx * matrix_[1];
    matrix_[6] += shearx * matrix_[2];
    matrix_[7] += shearx * matrix_[3];
}
