// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/vector3.h"
#include <cmath>

// 4D Real Vector
class Vector4
{
    public:
    Vector4() = default;
    Vector4(const double xx, const double yy, const double zz, const double ww);
    // Components of vector
    double x{0.0}, y{0.0}, z{0.0}, w{0.0};

    /*
     * Set / adjust / retrieve
     */
    public:
    // Set the vector to 0,0,0,0
    void zero();
    // Set all four components simultaneously
    void set(double newX, double newY, double newZ, double newW);
    // Set single component
    void set(int el, double value);
    // Adjust all four components simultaneously
    void add(double a, double b, double c, double d);
    // Set all four components simultaneously
    void set(Vector3 v, double newW);
    // Retrieve single element
    double get(int index) const;

    /*
     * Operators
     */
    public:
    // Operators + and +=
    Vector4 &operator+=(Vector4 &v);
    Vector4 &operator+=(Vector3 &v);
    Vector4 operator+(Vector4 &v);
    Vector4 operator+(Vector3 &v);
    Vector4 &operator-=(Vector4 &v);
    Vector4 &operator-=(Vector3 &v);
    Vector4 operator-(Vector4 &v);
    Vector4 operator-(Vector3 &v);
    Vector4 operator*(double value) const;
    void operator*=(double value);
    double operator[](int index);

    /*
     * Methods
     */
    public:
    // Normalise to unity
    void normalise();
    // Print
    void print() const;
};
