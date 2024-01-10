// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/matrix3.h"
#include "base/messenger.h"
#include <array>

Matrix3::Matrix3() { setIdentity(); }

/*
 * Operators
 */

// Matrix3 multiply (operator *) (return new matrix)
Matrix3 Matrix3::operator*(const Matrix3 &B) const
{
    // [ row(A|this).column(B) ]
    Matrix3 AB;

    AB.matrix_[0] = matrix_[0] * B.matrix_[0] + matrix_[3] * B.matrix_[1] + matrix_[6] * B.matrix_[2];
    AB.matrix_[1] = matrix_[1] * B.matrix_[0] + matrix_[4] * B.matrix_[1] + matrix_[7] * B.matrix_[2];
    AB.matrix_[2] = matrix_[2] * B.matrix_[0] + matrix_[5] * B.matrix_[1] + matrix_[8] * B.matrix_[2];

    AB.matrix_[3] = matrix_[0] * B.matrix_[3] + matrix_[3] * B.matrix_[4] + matrix_[6] * B.matrix_[5];
    AB.matrix_[4] = matrix_[1] * B.matrix_[3] + matrix_[4] * B.matrix_[4] + matrix_[7] * B.matrix_[5];
    AB.matrix_[5] = matrix_[2] * B.matrix_[3] + matrix_[5] * B.matrix_[4] + matrix_[8] * B.matrix_[5];

    AB.matrix_[6] = matrix_[0] * B.matrix_[6] + matrix_[3] * B.matrix_[7] + matrix_[6] * B.matrix_[8];
    AB.matrix_[7] = matrix_[1] * B.matrix_[6] + matrix_[4] * B.matrix_[7] + matrix_[7] * B.matrix_[8];
    AB.matrix_[8] = matrix_[2] * B.matrix_[6] + matrix_[5] * B.matrix_[7] + matrix_[8] * B.matrix_[8];

    return AB;
}

Matrix3 Matrix3::operator*(const double a) const
{
    Matrix3 AB;
    std::transform(matrix_.begin(), matrix_.end(), AB.matrix_.begin(), [a](const auto &el) { return el * a; });
    return AB;
}

Matrix3 Matrix3::operator+(const Matrix3 &B) const
{
    Matrix3 A;
    for (auto n = 0; n < 9; ++n)
        A[n] = matrix_[n] + B.matrix_[n];
    return A;
}

Matrix3 Matrix3::operator-(const Matrix3 &B) const
{
    Matrix3 A;
    for (auto n = 0; n < 9; ++n)
        A[n] = matrix_[n] - B.matrix_[n];
    return A;
}

Vec3<double> Matrix3::operator*(const Vec3<double> &v) const
{
    return {v.x * matrix_[0] + v.y * matrix_[3] + v.z * matrix_[6], v.x * matrix_[1] + v.y * matrix_[4] + v.z * matrix_[7],
            v.x * matrix_[2] + v.y * matrix_[5] + v.z * matrix_[8]};
}

// Matrix3 multiply (operator *=)
Matrix3 &Matrix3::operator*=(const Matrix3 &B)
{
    // [ row(A|this).column(B) ]
    Matrix3 AB;

    AB.matrix_[0] = matrix_[0] * B.matrix_[0] + matrix_[3] * B.matrix_[1] + matrix_[6] * B.matrix_[2];
    AB.matrix_[1] = matrix_[1] * B.matrix_[0] + matrix_[4] * B.matrix_[1] + matrix_[7] * B.matrix_[2];
    AB.matrix_[2] = matrix_[2] * B.matrix_[0] + matrix_[5] * B.matrix_[1] + matrix_[8] * B.matrix_[2];

    AB.matrix_[3] = matrix_[0] * B.matrix_[3] + matrix_[3] * B.matrix_[4] + matrix_[6] * B.matrix_[5];
    AB.matrix_[4] = matrix_[1] * B.matrix_[3] + matrix_[4] * B.matrix_[4] + matrix_[7] * B.matrix_[5];
    AB.matrix_[5] = matrix_[2] * B.matrix_[3] + matrix_[5] * B.matrix_[4] + matrix_[8] * B.matrix_[5];

    AB.matrix_[6] = matrix_[0] * B.matrix_[6] + matrix_[3] * B.matrix_[7] + matrix_[6] * B.matrix_[8];
    AB.matrix_[7] = matrix_[1] * B.matrix_[6] + matrix_[4] * B.matrix_[7] + matrix_[7] * B.matrix_[8];
    AB.matrix_[8] = matrix_[2] * B.matrix_[6] + matrix_[5] * B.matrix_[7] + matrix_[8] * B.matrix_[8];

    *this = AB;
    return *this;
}

Matrix3 &Matrix3::operator*=(const double a)
{
    std::transform(matrix_.begin(), matrix_.end(), matrix_.begin(), [a](const auto &el) { return el * a; });

    return *this;
}

double &Matrix3::operator[](int index) { return matrix_[index]; }

/*
 * General Routines
 */

// Return the current matrix array
const std::array<double, 9> &Matrix3::matrix() const { return matrix_; }

// Reset to the identity matrix
void Matrix3::setIdentity() { matrix_ = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}; }

// Print matrix
void Matrix3::print() const
{
    Messenger::print("CMaj   [012]     [345]     [678]\n");
    Messenger::print("        {:8.4f} {:8.4f} {:8.4f}\n", matrix_[0], matrix_[3], matrix_[6]);
    Messenger::print("        {:8.4f} {:8.4f} {:8.4f}\n", matrix_[1], matrix_[4], matrix_[7]);
    Messenger::print("        {:8.4f} {:8.4f} {:8.4f}\n", matrix_[2], matrix_[5], matrix_[8]);
}

// Set zero matrix
void Matrix3::zero() { std::fill(matrix_.begin(), matrix_.end(), 0.0); }

// Return transpose of current matrix
Matrix3 &Matrix3::transpose() const
{
    static Matrix3 A;
    A.matrix_[0] = matrix_[0];
    A.matrix_[1] = matrix_[3];
    A.matrix_[2] = matrix_[6];
    A.matrix_[3] = matrix_[1];
    A.matrix_[4] = matrix_[4];
    A.matrix_[5] = matrix_[7];
    A.matrix_[6] = matrix_[2];
    A.matrix_[7] = matrix_[5];
    A.matrix_[8] = matrix_[8];
    return A;
}

// Transform the supplied vector by the transpose of the current matrix
Vec3<double> Matrix3::transposeMultiply(const Vec3<double> &v) const
{
    return {v.x * matrix_[0] + v.y * matrix_[1] + v.z * matrix_[2], v.x * matrix_[3] + v.y * matrix_[4] + v.z * matrix_[5],
            v.x * matrix_[6] + v.y * matrix_[7] + v.z * matrix_[8]};
}

// Calculate determinant
double Matrix3::determinant() const
{
    double det =
        matrix_[0] * matrix_[4] * matrix_[8] + matrix_[1] * matrix_[5] * matrix_[6] + matrix_[2] * matrix_[3] * matrix_[7];
    det -= matrix_[2] * matrix_[4] * matrix_[6] + matrix_[1] * matrix_[3] * matrix_[8] + matrix_[0] * matrix_[5] * matrix_[7];
    return det;
}

// Invert matrix
void Matrix3::invert()
{
    // Gauss-Jordan Inversion
    // Invert the supplied matrix using Gauss-Jordan elimination
    std::array<int, 3> pivotcols{0}, pivotrows{0};
    std::array<bool, 3> pivoted{false};
    int pivotrow = 0, pivotcol = 0;
    int row, col, m;
    double large, element;

    // Loop over columns to be reduced
    for (auto n = 0; n < 3; ++n)
    {
        // Locate suitable pivot element - find largest value in the matrix A
        large = 0.0;
        for (row = 0; row < 3; ++row)
        {
            // Only search this row if it has not previously contained a pivot element
            if (pivoted[row])
                continue;
            for (col = 0; col < 3; ++col)
            {
                // Similarly, only look at the column element if the column hasn't been pivoted yet.
                if (pivoted[col])
                    continue;
                // Check the size of the element...
                element = fabs(matrix_[row * 3 + col]);
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
            for (m = 0; m < 3; ++m)
            {
                element = matrix_[pivotrow * 3 + m];
                matrix_[pivotrow * 3 + m] = matrix_[pivotcol * 3 + m];
                matrix_[pivotcol * 3 + m] = element;
            }
        }

        // Now ready to divide through row elements.
        element = 1.0 / matrix_[pivotcol * 3 + pivotcol];
        matrix_[pivotcol * 3 + pivotcol] = 1.0;
        for (m = 0; m < 3; ++m)
            matrix_[pivotcol * 3 + m] *= element;

        // Divide through other rows by the relevant multiple of the pivot row
        for (row = 0; row < 3; ++row)
        {
            if (row == pivotcol)
                continue;
            element = matrix_[row * 3 + pivotcol];
            matrix_[row * 3 + pivotcol] = 0.0;
            for (m = 0; m < 3; ++m)
                matrix_[row * 3 + m] = matrix_[row * 3 + m] - matrix_[pivotcol * 3 + m] * element;
        }
    }
    // Rearrange columns to undo row exchanges performed earlier
    for (auto i = 2; i >= 0; --i)
    {
        if (pivotrows[i] != pivotcols[i])
            for (m = 0; m < 3; ++m)
            {
                element = matrix_[m * 3 + pivotrows[i]];
                matrix_[m * 3 + pivotrows[i]] = matrix_[m * 3 + pivotcols[i]];
                matrix_[m * 3 + pivotcols[i]] = element;
            }
    }
}

// Return nth value of matrix
double Matrix3::value(int n) const { return matrix_[n]; }

// Return maximal element
double Matrix3::max() const { return *std::max_element(matrix_.begin(), matrix_.end()); }

// Return maximal absolute element
double Matrix3::maxAbs() const
{
    return std::fabs(
        *std::max_element(matrix_.begin(), matrix_.end(), [](auto a, auto b) { return std::fabs(a) < std::fabs(b); }));
}

/*
 * Column Operations
 */

// Copy column contents to supplied Vec3
Vec3<double> Matrix3::columnAsVec3(int col) const
{
    Vec3<double> vec(matrix_[col * 3], matrix_[col * 3 + 1], matrix_[col * 3 + 2]);
    return vec;
}

// Set specified row from supplied triplet of values
void Matrix3::setRow(int row, double x, double y, double z)
{
    matrix_[row] = x;
    matrix_[3 + row] = y;
    matrix_[6 + row] = z;
}

// Set specified column from supplied values
void Matrix3::setColumn(int col, double a, double b, double c)
{
    matrix_[col * 3] = a;
    matrix_[col * 3 + 1] = b;
    matrix_[col * 3 + 2] = c;
}

// Set specified column from supplied Vec3
void Matrix3::setColumn(int col, const Vec3<double> vec)
{
    matrix_[col * 3] = vec.x;
    matrix_[col * 3 + 1] = vec.y;
    matrix_[col * 3 + 2] = vec.z;
}

// Adjust specified column from supplied values
void Matrix3::adjustColumn(int col, double a, double b, double c)
{
    matrix_[col * 3] += a;
    matrix_[col * 3 + 1] += b;
    matrix_[col * 3 + 2] += c;
}

// Adjust specified column from supplied Vec3
void Matrix3::adjustColumn(int col, const Vec3<double> vec)
{
    matrix_[col * 3] += vec.x;
    matrix_[col * 3 + 1] += vec.y;
    matrix_[col * 3 + 2] += vec.z;
}

// Calculate column magnitude
double Matrix3::columnMagnitude(int column) const
{
    double mag = 0.0;
    for (auto n = column * 3; n < column * 3 + 3; ++n)
        mag += (matrix_[n] * matrix_[n]);
    return sqrt(mag);
}

// Multiply column by single value
void Matrix3::columnMultiply(int col, double d)
{
    matrix_[col * 3] *= d;
    matrix_[col * 3 + 1] *= d;
    matrix_[col * 3 + 2] *= d;
}

// Multiply first three columns by values in supplied vector
void Matrix3::columnMultiply(Vec3<double> vec)
{
    columnMultiply(0, vec.x);
    columnMultiply(1, vec.y);
    columnMultiply(2, vec.z);
}

// Normalise specified column to 1
void Matrix3::columnNormalise(int col)
{
    auto mag = 1.0 / sqrt(matrix_[col * 3] * matrix_[col * 3] + matrix_[col * 3 + 1] * matrix_[col * 3 + 1] +
                          matrix_[col * 3 + 2] * matrix_[col * 3 + 2]);
    matrix_[col * 3] *= mag;
    matrix_[col * 3 + 1] *= mag;
    matrix_[col * 3 + 2] *= mag;
}

// Orthogonalise rotation matrix column w.r.t. one (or two) other columns)
void Matrix3::orthogonaliseColumn(int targetcol, int orthocol1, int orthocol2)
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
    setColumn(targetcol, v);
}

/*
 * Rotations
 */

// Create rotation matrix about X
void Matrix3::createRotationX(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD;
    cosx = cos(theta);
    sinx = sin(theta);
    matrix_[0] = 1.0;
    matrix_[1] = 0.0;
    matrix_[2] = 0.0;

    matrix_[3] = 0.0;
    matrix_[4] = cosx;
    matrix_[5] = -sinx;

    matrix_[6] = 0.0;
    matrix_[7] = sinx;
    matrix_[8] = cosx;
}

// Create XY rotation matrix
void Matrix3::createRotationXY(double anglex, double angley)
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
    matrix_[4] = cosx;
    matrix_[5] = sinx;

    matrix_[6] = siny;
    matrix_[7] = (-sinx) * cosy;
    matrix_[8] = cosx * cosy;
}

// Create rotation matrix about Y
void Matrix3::createRotationY(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD;
    cosx = cos(theta);
    sinx = sin(theta);
    matrix_[0] = cosx;
    matrix_[1] = 0.0;
    matrix_[2] = sinx;

    matrix_[3] = 0.0;
    matrix_[4] = 1.0;
    matrix_[5] = 0.0;

    matrix_[6] = -sinx;
    matrix_[7] = 0.0;
    matrix_[8] = cosx;
}

// Create rotation matrix about Z
void Matrix3::createRotationZ(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD;
    cosx = cos(theta);
    sinx = sin(theta);
    matrix_[0] = cosx;
    matrix_[1] = -sinx;
    matrix_[2] = 0.0;

    matrix_[3] = sinx;
    matrix_[4] = cosx;
    matrix_[5] = 0.0;

    matrix_[6] = 0.0;
    matrix_[7] = 0.0;
    matrix_[8] = 1.0;
}

// Create axis rotation quaternion
void Matrix3::createRotationAxis(Vec3<double> axis, double angle, bool normalise)
{
    double cosx, sinx, theta = angle / DEGRAD, oneMcosx;
    if (normalise)
    {
        double mag = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
        axis.x /= mag;
        axis.y /= mag;
        axis.z /= mag;
    }
    cosx = cos(theta);
    sinx = sin(theta);
    oneMcosx = 1.0 - cosx;
    matrix_[0] = axis.x * axis.x * oneMcosx + cosx;
    matrix_[1] = axis.x * axis.y * oneMcosx - axis.z * sinx;
    matrix_[2] = axis.x * axis.z * oneMcosx + axis.y * sinx;

    matrix_[3] = axis.x * axis.y * oneMcosx + axis.z * sinx;
    matrix_[4] = axis.y * axis.y * oneMcosx + cosx;
    matrix_[5] = axis.y * axis.z * oneMcosx - axis.x * sinx;

    matrix_[6] = axis.x * axis.z * oneMcosx - axis.y * sinx;
    matrix_[7] = axis.y * axis.z * oneMcosx + axis.x * sinx;
    matrix_[8] = axis.z * axis.z * oneMcosx + cosx;
}

// Apply rotation about X axis
void Matrix3::applyRotationX(double angle)
{
    double cosx, sinx, theta = angle / DEGRAD, temp[3];
    cosx = cos(theta);
    sinx = sin(theta);

    // Recalculate second column and store in temp values
    temp[0] = matrix_[3] * cosx + matrix_[6] * -sinx;
    temp[1] = matrix_[4] * cosx + matrix_[7] * -sinx;
    temp[2] = matrix_[5] * cosx + matrix_[8] * -sinx;

    matrix_[6] = matrix_[3] * sinx + matrix_[6] * cosx;
    matrix_[7] = matrix_[4] * sinx + matrix_[7] * cosx;
    matrix_[8] = matrix_[5] * sinx + matrix_[8] * cosx;

    matrix_[3] = temp[0];
    matrix_[4] = temp[1];
    matrix_[5] = temp[2];
}

// Apply axis rotation quaternion
void Matrix3::applyRotationAxis(double ax, double ay, double az, double angle, bool normalise)
{
    double cosx, sinx, theta = angle / DEGRAD, temp[6], multipliers[9];
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
    multipliers[3] = ax * ay * (1.0 - cosx) - az * sinx;
    multipliers[4] = ay * ay * (1.0 - cosx) + cosx;
    multipliers[5] = ay * az * (1.0 - cosx) + ax * sinx;
    multipliers[6] = ax * az * (1.0 - cosx) + ay * sinx;
    multipliers[7] = ay * az * (1.0 - cosx) - ax * sinx;
    multipliers[8] = az * az * (1.0 - cosx) + cosx;

    temp[0] = matrix_[0] * multipliers[0] + matrix_[3] * multipliers[1] + matrix_[6] * multipliers[2];
    temp[1] = matrix_[1] * multipliers[0] + matrix_[4] * multipliers[1] + matrix_[7] * multipliers[2];
    temp[2] = matrix_[2] * multipliers[0] + matrix_[5] * multipliers[1] + matrix_[8] * multipliers[2];

    temp[3] = matrix_[0] * multipliers[3] + matrix_[3] * multipliers[4] + matrix_[6] * multipliers[5];
    temp[4] = matrix_[1] * multipliers[3] + matrix_[4] * multipliers[4] + matrix_[7] * multipliers[5];
    temp[5] = matrix_[2] * multipliers[3] + matrix_[5] * multipliers[4] + matrix_[8] * multipliers[5];

    matrix_[6] = matrix_[0] * multipliers[6] + matrix_[3] * multipliers[7] + matrix_[6] * multipliers[8];
    matrix_[7] = matrix_[1] * multipliers[6] + matrix_[4] * multipliers[7] + matrix_[7] * multipliers[8];
    matrix_[8] = matrix_[2] * multipliers[6] + matrix_[5] * multipliers[7] + matrix_[8] * multipliers[8];

    matrix_[0] = temp[0];
    matrix_[1] = temp[1];
    matrix_[2] = temp[2];
    matrix_[3] = temp[3];
    matrix_[4] = temp[4];
    matrix_[5] = temp[5];
}

/*
 * Scaling
 */

// Apply a general scaling to the matrix (as glScaled would do)
void Matrix3::applyScaling(double scalex, double scaley, double scalez)
{
    applyScalingX(scalex);
    applyScalingY(scaley);
    applyScalingZ(scalez);
}

// Apply a general scaling to the matrix (as glScaled would to)
void Matrix3::applyScaling(Vec3<double> scaling)
{
    applyScalingX(scaling.x);
    applyScalingY(scaling.y);
    applyScalingZ(scaling.z);
}

// Apply an xy-scaling to the matrix
void Matrix3::applyScalingXY(double scalex, double scaley)
{
    applyScalingX(scalex);
    applyScalingY(scaley);
}

// Apply an x scaling to the matrix
void Matrix3::applyScalingX(double scale)
{
    matrix_[0] *= scale;
    matrix_[1] *= scale;
    matrix_[2] *= scale;
}

// Apply a y scaling to the matrix
void Matrix3::applyScalingY(double scale)
{
    matrix_[3] *= scale;
    matrix_[4] *= scale;
    matrix_[5] *= scale;
}

// Apply a z scaling to the matrix
void Matrix3::applyScalingZ(double scale)
{
    matrix_[6] *= scale;
    matrix_[7] *= scale;
    matrix_[8] *= scale;
}

/*
 * Transforms
 */

// Transform coordinates supplied and return as Vec3<double>
Vec3<double> Matrix3::transform(double x, double y, double z) const
{
    Vec3<double> result;
    result.x = x * matrix_[0] + y * matrix_[3] + z * matrix_[6];
    result.y = x * matrix_[1] + y * matrix_[4] + z * matrix_[7];
    result.z = x * matrix_[2] + y * matrix_[5] + z * matrix_[8];
    return result;
}

// Transform coordinates supplied and return as Vec3<double>
Vec3<double> Matrix3::transform(Vec3<double> r) const
{
    Vec3<double> result;
    result.x = r.x * matrix_[0] + r.y * matrix_[3] + r.z * matrix_[6];
    result.y = r.x * matrix_[1] + r.y * matrix_[4] + r.z * matrix_[7];
    result.z = r.x * matrix_[2] + r.y * matrix_[5] + r.z * matrix_[8];
    return result;
}

/*
 * Special Functions
 */

// Construct 'cross-product' matrix of the supplied vector using cyclic permutations
void Matrix3::makeCrossProductMatrix(const Vec3<double> &v)
{
    for (auto n = 0; n < 3; ++n)
        setColumn(n, Vec3<double>::unit(DissolveMath::cp3(n + 1)) * v.get(DissolveMath::cp3(n + 2)) -
                         Vec3<double>::unit(DissolveMath::cp3(n + 2)) * v.get(DissolveMath::cp3(n + 1)));
}
