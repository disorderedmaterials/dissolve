// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/vector3i.h"
#include "base/messenger.h"
#include "math/mathFunc.h"
#include <cmath>
#include <format>
#include <iostream>
#include <stdexcept>

Vector3i::Vector3i(const int xx, const int yy, const int zz) : x(xx), y(yy), z(zz) {};

/*
 * Set / adjust / retrieve
 */

// Set the vector to 0,0,0
void Vector3i::zero()
{
    x = 0;
    y = 0;
    z = 0;
}

// Set the specific element to value
void Vector3i::set(int index, int value)
{
    if (index == 0)
        x = value;
    else if (index == 1)
        y = value;
    else if (index == 2)
        z = value;
}

// Set all three values simultaneously
void Vector3i::set(int newX, int newY, int newZ)
{
    x = newX;
    y = newY;
    z = newZ;
}

// Add value to single component
void Vector3i::add(int index, int delta)
{
    if (index == 0)
        x += delta;
    else if (index == 1)
        y += delta;
    else if (index == 2)
        z += delta;
}

// Add values to all three values simultaneously
void Vector3i::add(int dx, int dy, int dz)
{
    x += dx;
    y += dy;
    z += dz;
}

// Returns the value of the specified element
int Vector3i::get(int index) const
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

bool Vector3i::operator==(const Vector3i &value) const { return x == value.x && y == value.y && z == value.z; }

bool Vector3i::operator!=(const Vector3i &value) const { return !(*this == value); }

bool Vector3i::operator==(const int &value) const { return x == value && y == value && z == value; }

bool Vector3i::operator!=(const int &value) const { return !(*this == value); }

void Vector3i::operator=(const int value)
{
    x = value;
    y = value;
    z = value;
}

void Vector3i::operator+=(const int value)
{
    x += value;
    y += value;
    z += value;
}

void Vector3i::operator+=(const Vector3i &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
}

Vector3i Vector3i::operator+(const int value) const
{
    Vector3i result;
    result.x = x + value;
    result.y = y + value;
    result.z = z + value;
    return result;
}

Vector3i Vector3i::operator+(const Vector3i &v) const
{
    Vector3i result;
    result.x = x + v.x;
    result.y = y + v.y;
    result.z = z + v.z;
    return result;
}

Vector3i Vector3i::operator-() const { return Vector3i(-x, -y, -z); }

void Vector3i::operator-=(const int value)
{
    x -= value;
    y -= value;
    z -= value;
}

void Vector3i::operator-=(const Vector3i &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

Vector3i Vector3i::operator-(const int value) const { return Vector3i(x - value, y - value, z - value); }

Vector3i Vector3i::operator-(const Vector3i &v) const { return Vector3i(x - v.x, y - v.y, z - v.z); }

void Vector3i::operator/=(const int value)
{
    x /= value;
    y /= value;
    z /= value;
}

void Vector3i::operator/=(const Vector3i &v)
{
    x /= v.x;
    y /= v.y;
    z /= v.z;
}

Vector3i Vector3i::operator/(const int value) const { return Vector3i(x / value, y / value, z / value); }

Vector3i Vector3i::operator/(const Vector3i &v) const { return Vector3i(x / v.x, y / v.y, z / v.z); }

Vector3i Vector3i::operator*(const int value) const { return Vector3i(x * value, y * value, z * value); }

void Vector3i::operator*=(const int value)
{
    x *= value;
    y *= value;
    z *= value;
}

Vector3i Vector3i::operator*(const Vector3i &v) const
{
    // Cross Product
    return Vector3i(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

int &Vector3i::operator[](int index)
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
Vector3i Vector3i::abs() const { return Vector3i(fabs(x), fabs(y), fabs(z)); }

// Returns the largest absolute value of the vector
int Vector3i::absMax() const
{
    int a = (fabs(x) < fabs(y)) ? fabs(y) : fabs(x);
    return (a < fabs(z)) ? fabs(z) : a;
}

// Returns the index of the maximum absolute-valued element in the vector
int Vector3i::absMaxElement() const
{
    if ((fabs(x) >= fabs(y)) && (fabs(x) >= fabs(z)))
        return 0;
    if ((fabs(y) >= fabs(x)) && (fabs(y) >= fabs(z)))
        return 1;
    return 2;
}

// Returns the smallest absolute value of the vector
int Vector3i::absMin() const
{
    int a = (fabs(x) > fabs(y)) ? fabs(y) : fabs(x);
    return (a > fabs(z)) ? fabs(z) : a;
}

// Returns the index of the minimum absolute-valued element in the vector
int Vector3i::absMinElement() const
{
    if ((fabs(x) <= fabs(y)) && (fabs(x) <= fabs(z)))
        return 0;
    if ((fabs(y) <= fabs(x)) && (fabs(y) <= fabs(z)))
        return 1;
    return 2;
}

// Dot product between this and supplied vector
int Vector3i::dp(const Vector3i &v) const { return (x * v.x + y * v.y + z * v.z); }

// Normalise and return original magnitude
int Vector3i::magAndNormalise()
{
    int mag = sqrt(x * x + y * y + z * z);
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
int Vector3i::magSqAndNormalise()
{
    int magSq = x * x + y * y + z * z;
    int mag = sqrt(magSq);
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
int Vector3i::magnitude() const { return sqrt(x * x + y * y + z * z); }

// Calculate square of vector magnitude
int Vector3i::magnitudeSq() const { return x * x + y * y + z * z; }

// Returns the largest value of the vector
int Vector3i::max() const
{
    int a = (x < y) ? y : x;
    return (a < z) ? z : a;
}

// Returns the maximum valued element in the vector
int Vector3i::maxElement() const
{
    if ((x >= y) && (x >= z))
        return 0;
    if ((y >= x) && (y >= z))
        return 1;
    return 2;
}

// Returns the smallest value of the vector
int Vector3i::min() const
{
    int a = (x > y) ? y : x;
    return (a > z) ? z : a;
}

// Returns the minimum valued element in the vector
int Vector3i::minElement() const
{
    if ((x <= y) && (x <= z))
        return 0;
    if ((y <= x) && (y <= z))
        return 1;
    return 2;
}

// Return vector with specified element adjusted
Vector3i Vector3i::adjusted(int element, int delta) const
{
    auto newValue = *this;
    newValue[element] += delta;
    return newValue;
}

// Multiply elements of this vector with factors supplied
void Vector3i::multiply(int facx, int facy, int facz)
{
    x *= facx;
    y *= facy;
    z *= facz;
}

// Multiply elements of this vector by those of supplied vector
void Vector3i::multiply(Vector3i v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
}

// Prints the contents of the vector
void Vector3i::print() const { std::cout << std::format("{} {} {}", x, y, z) << std::endl; }

// Swap the two specified elements
void Vector3i::swap(int a, int b)
{
    int temp = get(a);
    set(a, get(b));
    set(b, temp);
}

/*
 * Serialisation
 */

// Express as a serialisable value
void Vector3i::serialise(std::string tag, SerialisedValue &target) const
{
    SerialisedValue::array_type result;
    result.push_back(x);
    result.push_back(y);
    result.push_back(z);
    target[tag] = result;
}

// Read values from a serialisable value
void Vector3i::deserialise(const SerialisedValue &node)
{
    x = toml::get<int>(node[0]);
    y = toml::get<int>(node[1]);
    z = toml::get<int>(node[2]);
}
