// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/vector4.h"
#include "base/messenger.h"
#include <cmath>
#include <iostream>

Vector4::Vector4(const double xx, const double yy, const double zz, const double ww)
{
    x = xx;
    y = yy;
    z = zz;
    w = ww;
}

/*
 * Set / adjust / retrieve
 */

// Set the vector to 0,0,0,0
void Vector4::zero()
{
    x = 0;
    y = 0;
    z = 0;
    w = 0;
}
// Set all four components simultaneously
void Vector4::set(double newX, double newY, double newZ, double newW)
{
    x = newX;
    y = newY;
    z = newZ;
    w = newW;
}
// Set single component
void Vector4::set(int el, double value)
{
    if (el == 0)
        x = value;
    else if (el == 1)
        y = value;
    else if (el == 2)
        z = value;
    else if (el == 3)
        w = value;
}
// Adjust all four components simultaneously
void Vector4::add(double a, double b, double c, double d)
{
    x += a;
    y += b;
    z += c;
    w += d;
}
// Set all four components simultaneously
void Vector4::set(Vector3 v, double newW)
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = newW;
}
// Retrieve single element
double Vector4::get(int index) const
{
    if (index == 0)
        return x;
    else if (index == 1)
        return y;
    else if (index == 2)
        return z;
    else if (index == 3)
        return w;

    Messenger::exception("Vec4 - array access failed - index {} is out of bounds.", index);
}

/*
 * Operators
 */

// Operators + and +=
Vector4 &Vector4::operator+=(Vector4 &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}
Vector4 &Vector4::operator+=(Vector3 &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}
Vector4 Vector4::operator+(Vector4 &v) { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
Vector4 Vector4::operator+(Vector3 &v) { return Vector4(x + v.x, y + v.y, z + v.z, w); }
// Operators - and -=
Vector4 &Vector4::operator-=(Vector4 &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}
Vector4 &Vector4::operator-=(Vector3 &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}
Vector4 Vector4::operator-(Vector4 &v) { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
Vector4 Vector4::operator-(Vector3 &v) { return Vector4(x - v.x, y - v.y, z - v.z, w); }
// Operators * and *=
Vector4 Vector4::operator*(double value) const { return Vector4(x * value, y * value, z * value, w * value); }
void Vector4::operator*=(double value)
{
    x *= value;
    y *= value;
    z *= value;
    w *= value;
}
double Vector4::operator[](int index)
{
    if (index == 0)
        return x;
    else if (index == 1)
        return y;
    else if (index == 2)
        return z;
    else if (index == 3)
        return w;

    Messenger::exception("Vec4 - array access failed - index {} is out of bounds.", index);
    return 0;
}

/*
 * Methods
 */

// Normalise to unity
void Vector4::normalise()
{
    double mag = sqrt(w * w + x * x + y * y + z * z);
    if (mag < 1.0E-8)
        zero();
    else
    {
        x /= mag;
        y /= mag;
        z /= mag;
        w /= mag;
    }
}

// Print
void Vector4::print() const { std::cout << std::format("vec(xyzw) = {} {} {} {}", x, y, z, w) << std::endl; }
