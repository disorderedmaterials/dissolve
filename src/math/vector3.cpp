// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/vector3.h"
#include "base/messenger.h"
#include "math/mathFunc.h"
#include <cmath>
#include <format>
#include <iostream>
#include <stdexcept>

Vector3::Vector3(const double xx, const double yy, const double zz) : x(xx), y(yy), z(zz) {};

/*
 * Set / adjust / retrieve
 */

// Set the vector to 0,0,0
void Vector3::zero()
{
    x = 0;
    y = 0;
    z = 0;
}

// Set the specific element to value
void Vector3::set(int index, double value)
{
    if (index == 0)
        x = value;
    else if (index == 1)
        y = value;
    else if (index == 2)
        z = value;
}

// Set all three values simultaneously
void Vector3::set(double newX, double newY, double newZ)
{
    x = newX;
    y = newY;
    z = newZ;
}

// Add value to single component
void Vector3::add(int index, double delta)
{
    if (index == 0)
        x += delta;
    else if (index == 1)
        y += delta;
    else if (index == 2)
        z += delta;
}

// Add values to all three values simultaneously
void Vector3::add(double dx, double dy, double dz)
{
    x += dx;
    y += dy;
    z += dz;
}

// Returns the value of the specified element
double Vector3::get(int index) const
{
    if (index == 0)
        return x;
    else if (index == 1)
        return y;
    else if (index == 2)
        return z;

    Messenger::exception("Vec3 - element index {} is out of bounds.", index);
}

/*
 * Operators
 */

bool Vector3::operator==(const Vector3 &value) const { return x == value.x && y == value.y && z == value.z; }

bool Vector3::operator!=(const Vector3 &value) const { return !(*this == value); }

bool Vector3::operator==(const double &value) const { return x == value && y == value && z == value; }

bool Vector3::operator!=(const double &value) const { return !(*this == value); }

void Vector3::operator=(const double value)
{
    x = value;
    y = value;
    z = value;
}

void Vector3::operator+=(const double value)
{
    x += value;
    y += value;
    z += value;
}

void Vector3::operator+=(const Vector3 &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
}

Vector3 Vector3::operator+(const double value) const
{
    Vector3 result;
    result.x = x + value;
    result.y = y + value;
    result.z = z + value;
    return result;
}

Vector3 Vector3::operator+(const Vector3 &v) const
{
    Vector3 result;
    result.x = x + v.x;
    result.y = y + v.y;
    result.z = z + v.z;
    return result;
}

Vector3 Vector3::operator-() const { return Vector3(-x, -y, -z); }

void Vector3::operator-=(const double value)
{
    x -= value;
    y -= value;
    z -= value;
}

void Vector3::operator-=(const Vector3 &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

Vector3 Vector3::operator-(const double value) const { return Vector3(x - value, y - value, z - value); }

Vector3 Vector3::operator-(const Vector3 &v) const { return Vector3(x - v.x, y - v.y, z - v.z); }

void Vector3::operator/=(const double value)
{
    x /= value;
    y /= value;
    z /= value;
}

void Vector3::operator/=(const Vector3 &v)
{
    x /= v.x;
    y /= v.y;
    z /= v.z;
}

Vector3 Vector3::operator/(const double value) const { return Vector3(x / value, y / value, z / value); }

Vector3 Vector3::operator/(const Vector3 &v) const { return Vector3(x / v.x, y / v.y, z / v.z); }

Vector3 Vector3::operator*(const double value) const { return Vector3(x * value, y * value, z * value); }

void Vector3::operator*=(const double value)
{
    x *= value;
    y *= value;
    z *= value;
}

Vector3 Vector3::operator*(const Vector3 &v) const
{
    // Cross Product
    return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

double &Vector3::operator[](int index)
{
    if (index == 0)
        return x;
    else if (index == 1)
        return y;
    else if (index == 2)
        return z;

    Messenger::exception("Vec3 - array access failed - index {} is out of bounds.", index);
}

/*
 * Methods
 */

// Return vector of absolute elements
Vector3 Vector3::abs() const { return Vector3(fabs(x), fabs(y), fabs(z)); }

// Returns the largest absolute value of the vector
double Vector3::absMax() const
{
    double a = (fabs(x) < fabs(y)) ? fabs(y) : fabs(x);
    return (a < fabs(z)) ? fabs(z) : a;
}

// Returns the index of the maximum absolute-valued element in the vector
int Vector3::absMaxElement() const
{
    if ((fabs(x) >= fabs(y)) && (fabs(x) >= fabs(z)))
        return 0;
    if ((fabs(y) >= fabs(x)) && (fabs(y) >= fabs(z)))
        return 1;
    return 2;
}

// Returns the smallest absolute value of the vector
double Vector3::absMin() const
{
    double a = (fabs(x) > fabs(y)) ? fabs(y) : fabs(x);
    return (a > fabs(z)) ? fabs(z) : a;
}

// Returns the index of the minimum absolute-valued element in the vector
int Vector3::absMinElement() const
{
    if ((fabs(x) <= fabs(y)) && (fabs(x) <= fabs(z)))
        return 0;
    if ((fabs(y) <= fabs(x)) && (fabs(y) <= fabs(z)))
        return 1;
    return 2;
}

// Dot product between this and supplied vector
double Vector3::dp(const Vector3 &v) const { return (x * v.x + y * v.y + z * v.z); }

// Normalise and return original magnitude
double Vector3::magAndNormalise()
{
    double mag = sqrt(x * x + y * y + z * z);
    if (mag < 1.0E-8)
        zero();
    else
    {
        x /= mag;
        y /= mag;
        z /= mag;
    }
    return mag;
}

// Normalise and return original magnitude squared
double Vector3::magSqAndNormalise()
{
    double magSq = x * x + y * y + z * z;
    double mag = sqrt(magSq);
    if (mag < 1.0E-8)
        zero();
    else
    {
        x /= mag;
        y /= mag;
        z /= mag;
    }
    return magSq;
}

// Calculate vector magnitude
double Vector3::magnitude() const { return sqrt(x * x + y * y + z * z); }

// Calculate square of vector magnitude
double Vector3::magnitudeSq() const { return x * x + y * y + z * z; }

// Returns the largest value of the vector
double Vector3::max() const
{
    double a = (x < y) ? y : x;
    return (a < z) ? z : a;
}

// Returns the maximum valued element in the vector
int Vector3::maxElement() const
{
    if ((x >= y) && (x >= z))
        return 0;
    if ((y >= x) && (y >= z))
        return 1;
    return 2;
}

// Returns the smallest value of the vector
double Vector3::min() const
{
    double a = (x > y) ? y : x;
    return (a > z) ? z : a;
}

// Returns the minimum valued element in the vector
int Vector3::minElement() const
{
    if ((x <= y) && (x <= z))
        return 0;
    if ((y <= x) && (y <= z))
        return 1;
    return 2;
}

// Return vector with specified element adjusted
Vector3 Vector3::adjusted(int element, double delta) const
{
    auto newValue = *this;
    newValue[element] += delta;
    return newValue;
}

// Multiply elements of this vector with factors supplied
void Vector3::multiply(double facx, double facy, double facz)
{
    x *= facx;
    y *= facy;
    z *= facz;
}

// Multiply elements of this vector by those of supplied vector
void Vector3::multiply(Vector3 v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
}

// Normalise the vector to unity
void Vector3::normalise()
{
    auto mag = sqrt(x * x + y * y + z * z);
    if (mag < 1.0E-8)
        zero();
    else
    {
        x /= mag;
        y /= mag;
        z /= mag;
    }
}

// Return the normalised vector
Vector3 Vector3::normalised() const
{
    auto mag = sqrt(x * x + y * y + z * z);
    return {x / mag, y / mag, z / mag};
}
// Returns an orthogonal, normalised unit vector
Vector3 Vector3::orthogonal() const
{
    Vector3 result;
    const int maxComponent = absMaxElement();
    if (maxComponent == 0)
    {
        // X component is largest so return XP with (0,1,0)
        result.x = -z;
        result.y = 0.0;
        result.z = x;
    }
    else if (maxComponent == 1)
    {
        // Y component is largest, so return XP with (0,0,1)
        result.x = y;
        result.y = -x;
        result.z = 0.0;
    }
    else
    {
        // Z component is largest, so return XP with (1,0,0)
        result.x = 0.0;
        result.y = z;
        result.z = -y;
    }
    result.normalise();
    return result;
}

// Orthogonalise (Gram-Schmidt) w.r.t. supplied vector
void Vector3::orthogonalise(const Vector3 &reference)
{
    double sourcemag = reference.magnitude();
    double dpovermagsq = dp(reference) / (sourcemag * sourcemag);
    x = x - dpovermagsq * reference.x;
    y = y - dpovermagsq * reference.y;
    z = z - dpovermagsq * reference.z;
}

// Orthogonalise (two vectors)
void Vector3::orthogonalise(const Vector3 &reference1, const Vector3 &reference2)
{
    // This routine actually generates the orthogonal vector via the cross-product
    // We also calculate the scalar resolute (dp) to ensure the new vector points in the same direction
    Vector3 newvec = reference1 * reference2;
    newvec.normalise();
    double dp = newvec.dp(*this);
    if (dp < 0.0)
        newvec *= -1.0;
    *this = newvec;
}

// Prints the contents of the vector
void Vector3::print() const { std::cout << std::format("{} {} {}", x, y, z) << std::endl; }

// Generate random unit vector (on a unit sphere)
Vector3 Vector3::randomUnit()
{
    auto y = 2.0 * (DissolveMath::random() - 0.5);
    auto r = sqrt(1 - y * y);
    auto lambda = M_PI * (DissolveMath::random() - 0.5);
    return {r * sin(lambda), y, r * cos(lambda)};
}

// Convert spherical who,phi,theta coordinates into cartesian x,y,z
void Vector3::toCartesian()
{
    // x = rho, y = theta (inclination), z = phi (azimuthal angle), assuming that phi and theta are in degrees
    double newx, newy, newz;
    y = DissolveMath::toRadians(y);
    z = DissolveMath::toRadians(z);
    newx = x * sin(y) * cos(z);
    newy = x * sin(y) * sin(z);
    newz = x * cos(y);
    set(newx, newy, newz);
}

// Convert cartesian x,y,z coordinates into spherical (rho,phi/zenith,theta/azimuthal)
void Vector3::toSpherical()
{
    double rho, phi, theta;
    rho = magnitude();
    theta = acos(z / rho);
    phi = atan2(y, x);
    set(rho, DissolveMath::toDegrees(phi), DissolveMath::toDegrees(theta));
}

// Set all three values simultaneously, converting supplied spherical coordinates to cartesian
void Vector3::fromSpherical(double r, double phi, double theta)
{
    x = r;
    y = phi;
    z = theta;
    toCartesian();
}

// Return a unit vector along the specified direction
Vector3 Vector3::unit(int index)
{
    if (index == 0)
        return {1, 0, 0};
    else if (index == 1)
        return {0, 1, 0};
    else if (index == 2)
        return {0, 0, 1};

    Messenger::exception("Vec3 - unit() generation failed - index {} is out of bounds.", index);
}

// Swap the two specified elements
void Vector3::swap(int a, int b)
{
    double temp = get(a);
    set(a, get(b));
    set(b, temp);
}

// Convert from (assumed) degrees to radians
void Vector3::toRadians()
{
    x = DissolveMath::toRadians(x);
    y = DissolveMath::toRadians(y);
    z = DissolveMath::toRadians(z);
}

// Convert from (assumed) radians to degrees
void Vector3::toDegrees()
{
    x = DissolveMath::toDegrees(x);
    y = DissolveMath::toDegrees(y);
    z = DissolveMath::toDegrees(z);
}

// Return angle between this and supplied vector, in radians, ensuring normalisation
double Vector3::angleInRadians(const Vector3 &to) const { return acos(normalised().dp(to.normalised())); }

// Return angle between this and supplied vector, in degrees, ensuring normalisation
double Vector3::angleInDegrees(const Vector3 &to) const { return DissolveMath::toDegrees(angleInRadians(to)); }

/*
 * Serialisation
 */

// Express as a serialisable value
void Vector3::serialise(std::string tag, SerialisedValue &target) const
{
    SerialisedValue::array_type result;
    result.push_back(x);
    result.push_back(y);
    result.push_back(z);
    target[tag] = result;
}

// Read values from a serialisable value
void Vector3::deserialise(const SerialisedValue &node)
{
    x = toml::get<double>(node[0]);
    y = toml::get<double>(node[1]);
    z = toml::get<double>(node[2]);
}
