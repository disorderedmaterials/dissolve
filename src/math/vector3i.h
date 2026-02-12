// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <cmath>
#include <format>
#include <stdexcept>

class NodeValue;
class ExpressionVariable;

// 3D Real Vector
class Vector3i : public Serialisable<>
{
    public:
    Vector3i() = default;
    Vector3i(const int xx, const int yy, const int zz);
    // Components of vector
    int x{0}, y{0}, z{0};

    /*
     * Set / adjust / retrieve
     */
    public:
    // Set the vector to 0,0,0
    void zero();
    // Set the specific element to value
    void set(int index, int value);
    // Set all three values simultaneously
    void set(int newX, int newY, int newZ);
    // Add value to single component
    void add(int index, int delta);
    // Add values to all three values simultaneously
    void add(int dx, int dy, int dz);
    // Returns the value of the specified element
    int get(int index) const;

    /*
     * Operators
     */
    public:
    bool operator==(const Vector3i &value) const;
    bool operator!=(const Vector3i &value) const;
    bool operator==(const int &value) const;
    bool operator!=(const int &value) const;
    void operator=(const int value);
    void operator+=(const int value);
    void operator+=(const Vector3i &v);
    Vector3i operator+(const int value) const;
    Vector3i operator+(const Vector3i &v) const;
    inline Vector3i operator-() const;
    void operator-=(const int value);
    void operator-=(const Vector3i &v);
    Vector3i operator-(const int value) const;
    Vector3i operator-(const Vector3i &v) const;
    void operator/=(const int value);
    void operator/=(const Vector3i &v);
    Vector3i operator/(const int value) const;
    Vector3i operator/(const Vector3i &v) const;
    // Operators * and *=;
    Vector3i operator*(const int value) const;
    void operator*=(const int value);
    Vector3i operator*(const Vector3i &v) const;
    int &operator[](int index);

    /*
     * Methods
     */
    public:
    // Return vector of absolute elements
    Vector3i abs() const;
    // Returns the largest absolute value of the vector
    int absMax() const;
    // Returns the index of the maximum absolute-valued element in the vector
    int absMaxElement() const;
    // Returns the smallest absolute value of the vector
    int absMin() const;
    // Returns the index of the minimum absolute-valued element in the vector
    int absMinElement() const;
    // Dot product between this and supplied vector
    int dp(const Vector3i &v) const;
    // Normalise and return original magnitude
    int magAndNormalise();
    // Normalise and return original magnitude squared
    int magSqAndNormalise();
    // Calculate vector magnitude
    int magnitude() const;
    // Calculate square of vector magnitude
    int magnitudeSq() const;
    // Returns the largest value of the vector
    int max() const;
    // Returns the maximum valued element in the vector
    int maxElement() const;
    // Returns the smallest value of the vector
    int min() const;
    // Returns the minimum valued element in the vector
    int minElement() const;
    // Return vector with specified element adjusted
    Vector3i adjusted(int element, int delta) const;
    // Multiply elements of this vector with factors supplied
    void multiply(int facx, int facy, int facz);
    // Multiply elements of this vector by those of supplied vector
    void multiply(Vector3i v);
    // Prints the contents of the vector
    void print() const;
    // Swap the two specified elements
    void swap(int a, int b);

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
};
