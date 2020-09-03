/*
    *** Column-Major 3x3 Matrix class
    *** src/math/matrix3.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more detailAs.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "math/matrix3.h"
#include "base/messenger.h"

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
    for (int n = 0; n < 9; ++n)
        AB.matrix_[n] = matrix_[n] * a;
    return AB;
}

Matrix3 Matrix3::operator+(const Matrix3 &B) const
{
    Matrix3 A;
    for (int n = 0; n < 9; ++n)
        A[n] = matrix_[n] + B.matrix_[n];
    return A;
}

Matrix3 Matrix3::operator-(const Matrix3 &B) const
{
    Matrix3 A;
    for (int n = 0; n < 9; ++n)
        A[n] = matrix_[n] - B.matrix_[n];
    return A;
}

Vec3<double> Matrix3::operator*(const Vec3<double> &v) const
{
    Vec3<double> result;
    result.x = v.x * matrix_[0] + v.y * matrix_[3] + v.z * matrix_[6];
    result.y = v.x * matrix_[1] + v.y * matrix_[4] + v.z * matrix_[7];
    result.z = v.x * matrix_[2] + v.y * matrix_[5] + v.z * matrix_[8];
    return result;
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
    matrix_[0] *= a;
    matrix_[1] *= a;
    matrix_[2] *= a;
    matrix_[3] *= a;
    matrix_[4] *= a;
    matrix_[5] *= a;
    matrix_[6] *= a;
    matrix_[7] *= a;
    matrix_[8] *= a;

    return *this;
}

// Array access (operator [])
double &Matrix3::operator[](int index) { return matrix_[index]; }

/*
 * General Routines
 */

// Reset to the identity matrix
void Matrix3::setIdentity()
{
    matrix_[0] = 1.0;
    matrix_[1] = 0.0;
    matrix_[2] = 0.0;
    matrix_[3] = 0.0;
    matrix_[4] = 1.0;
    matrix_[5] = 0.0;
    matrix_[6] = 0.0;
    matrix_[7] = 0.0;
    matrix_[8] = 1.0;
}

// Print matrix
void Matrix3::print() const
{
    Messenger::print("CMaj   [012]     [345]     [678]\n");
    Messenger::print("        {:8.4f} {:8.4f} {:8.4f}\n", matrix_[0], matrix_[3], matrix_[6]);
    Messenger::print("        {:8.4f} {:8.4f} {:8.4f}\n", matrix_[1], matrix_[4], matrix_[7]);
    Messenger::print("        {:8.4f} {:8.4f} {:8.4f}\n", matrix_[2], matrix_[5], matrix_[8]);
}

// Set zero matrix
void Matrix3::zero()
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
}

// Return matrix array
double *Matrix3::matrix() { return matrix_; }

// Return transpose of current matrix
Matrix3 &Matrix3::transpose()
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
    int pivotrows[3], pivotcols[3], pivotrow = 0, pivotcol = 0;
    bool pivoted[3];
    int row, col, n, m;
    double large, element;
    for (n = 0; n < 3; ++n)
    {
        pivotrows[n] = 0;
        pivotcols[n] = 0;
        pivoted[n] = false;
    }
    // Loop over columns to be reduced
    for (n = 0; n < 3; ++n)
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
    for (n = 2; n >= 0; --n)
    {
        if (pivotrows[n] != pivotcols[n])
            for (m = 0; m < 3; ++m)
            {
                element = matrix_[m * 3 + pivotrows[n]];
                matrix_[m * 3 + pivotrows[n]] = matrix_[m * 3 + pivotcols[n]];
                matrix_[m * 3 + pivotcols[n]] = element;
            }
    }
}

// Return nth value of matrix
double Matrix3::value(int n) const { return matrix_[n]; }

// Return maximal element
double Matrix3::max() const
{
    auto maxId = 0;
    for (int n = 1; n < 9; ++n)
        if (matrix_[n] > matrix_[maxId])
            maxId = n;
    return matrix_[maxId];
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
    for (int n = column * 3; n < column * 3 + 3; ++n)
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
    double mag = 1.0 / sqrt(matrix_[col * 3] * matrix_[col * 3] + matrix_[col * 3 + 1] * matrix_[col * 3 + 1] +
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
void Matrix3::createRotationAxis(double ax, double ay, double az, double angle, bool normalise)
{
    double cosx, sinx, theta = angle / DEGRAD, oneMcosx;
    if (normalise)
    {
        double mag = sqrt(ax * ax + ay * ay + az * az);
        ax /= mag;
        ay /= mag;
        az /= mag;
    }
    cosx = cos(theta);
    sinx = sin(theta);
    oneMcosx = 1.0 - cosx;
    matrix_[0] = ax * ax * oneMcosx + cosx;
    matrix_[1] = ax * ay * oneMcosx - az * sinx;
    matrix_[2] = ax * az * oneMcosx + ay * sinx;

    matrix_[3] = ax * ay * oneMcosx + az * sinx;
    matrix_[4] = ay * ay * oneMcosx + cosx;
    matrix_[5] = ay * az * oneMcosx - ax * sinx;

    matrix_[6] = ax * az * oneMcosx - ay * sinx;
    matrix_[7] = ay * az * oneMcosx + ax * sinx;
    matrix_[8] = az * az * oneMcosx + cosx;
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
Vec3<double> Matrix3::transform(Vec3<double> vec) const
{
    Vec3<double> result;
    result.x = vec.x * matrix_[0] + vec.y * matrix_[3] + vec.z * matrix_[6];
    result.y = vec.x * matrix_[1] + vec.y * matrix_[4] + vec.z * matrix_[7];
    result.z = vec.x * matrix_[2] + vec.y * matrix_[5] + vec.z * matrix_[8];
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
