// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <cmath>
#include <format>
#include <stdexcept>

// 3D Real Vector
class Vector3
{
    public:
    Vector3() = default;
    Vector3(const double xx, const double yy, const double zz);
    // Components of vector
    double x{0.0}, y{0.0}, z{0.0};

    /*
     * Set / adjust / retrieve
     */
    public:
    // Set the vector to 0,0,0
    void zero();
    // Set the specific element to value
    void set(int index, double value);
    // Set all three values simultaneously
    void set(double newX, double newY, double newZ);
    // Add value to single component
    void add(int index, double delta);
    // Add values to all three values simultaneously
    void add(double dx, double dy, double dz);
    // Returns the value of the specified element
    double get(int index) const;

    /*
     * Operators
     */
    public:
    bool operator==(const Vector3 &value) const;
    bool operator!=(const Vector3 &value) const;
    bool operator==(const double &value) const;
    bool operator!=(const double &value) const;
    void operator=(const double value);
    void operator+=(const double value);
    void operator+=(const Vector3 &v);
    Vector3 operator+(const double value) const;
    Vector3 operator+(const Vector3 &v) const;
    Vector3 operator-() const;
    void operator-=(const double value);
    void operator-=(const Vector3 &v);
    Vector3 operator-(const double value) const;
    Vector3 operator-(const Vector3 &v) const;
    void operator/=(const double value);
    void operator/=(const Vector3 &v);
    Vector3 operator/(const double value) const;
    Vector3 operator/(const Vector3 &v) const;
    // Operators * and *=;
    Vector3 operator*(const double value) const;
    void operator*=(const double value);
    Vector3 operator*(const Vector3 &v) const;
    double &operator[](int index);

    /*
     * Methods
     */
    public:
    // Return vector of absolute elements
    Vector3 abs() const;
    // Returns the largest absolute value of the vector
    double absMax() const;
    // Returns the index of the maximum absolute-valued element in the vector
    int absMaxElement() const;
    // Returns the smallest absolute value of the vector
    double absMin() const;
    // Returns the index of the minimum absolute-valued element in the vector
    int absMinElement() const;
    // Dot product between this and supplied vector
    double dp(const Vector3 &v) const;
    // Normalise and return original magnitude
    double magAndNormalise();
    // Normalise and return original magnitude squared
    double magSqAndNormalise();
    // Calculate vector magnitude
    double magnitude() const;
    // Calculate square of vector magnitude
    double magnitudeSq() const;
    // Returns the largest value of the vector
    double max() const;
    // Returns the maximum valued element in the vector
    int maxElement() const;
    // Returns the smallest value of the vector
    double min() const;
    // Returns the minimum valued element in the vector
    int minElement() const;
    // Return vector with specified element adjusted
    Vector3 adjusted(int element, double delta) const;
    // Multiply elements of this vector with factors supplied
    void multiply(double facx, double facy, double facz);
    // Multiply elements of this vector by those of supplied vector
    void multiply(Vector3 v);
    // Normalise the vector to unity
    void normalise();
    // Return the normalised vector
    Vector3 normalised() const;
    // Returns an orthogonal, normalised unit vector
    Vector3 orthogonal() const;
    // Orthogonalise (Gram-Schmidt) w.r.t. supplied vector
    void orthogonalise(const Vector3 &reference);
    // Orthogonalise (two vectors)
    void orthogonalise(const Vector3 &reference1, const Vector3 &reference2);
    // Prints the contents of the vector
    void print() const;
    // Generate random unit vector
    static Vector3 randomUnit();
    // Convert spherical who,phi,theta coordinates into cartesian x,y,z
    void toCartesian();
    // Convert cartesian x,y,z coordinates into spherical (rho,phi/zenith,theta/azimuthal)
    void toSpherical();
    // Set all three values simultaneously, converting supplied spherical coordinates to cartesian
    void fromSpherical(double r, double phi, double theta);
    // Return a unit vector along the specified direction
    static Vector3 unit(int index);
    // Swap the two specified elements
    void swap(int a, int b);
    // Convert from (assumed) degrees to radians
    void toRadians();
    // Convert from (assumed) radians to degrees
    void toDegrees();
    // Return angle between this and supplied vector, in radians, ensuring normalisation
    double angleInRadians(const Vector3 &to) const;
    // Return angle between this and supplied vector, in degrees, ensuring normalisation
    double angleInDegrees(const Vector3 &to) const;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
};
