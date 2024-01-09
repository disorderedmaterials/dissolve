// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"

CubicBox::CubicBox(double length) : Box(Box::BoxType::Cubic, {length, length, length}, {90.0, 90.0, 90.0}) {}

/*
 * Coordinate Conversion
 */

// Convert specified fractional coordinates to real-space coordinates
void CubicBox::toReal(Vec3<double> &r) const
{
    r.x *= a_;
    r.y *= a_;
    r.z *= a_;
}

// Convert specified real-space coordinates to fractional coordinates
void CubicBox::toFractional(Vec3<double> &r) const
{
    r.x *= ra_;
    r.y *= ra_;
    r.z *= ra_;
}

/*
 * Minimum Image Calculation
 */

// Return minimum image coordinates of r1 with respect to r2
Vec3<double> CubicBox::minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v21 = r1 - r2;
    toFractional(v21);
    wrap(v21);
    toReal(v21);
    v21 += r2;
    return v21;
}

// Return minimum image vector from r1 to r2
Vec3<double> CubicBox::minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    return v12;
}

// Return normalised minimum image vector from r1 to r2
Vec3<double> CubicBox::minimumVectorN(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    v12.normalise();
    return v12;
}

// Return minimum image distance from r1 to r2
double CubicBox::minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    return v12.magnitude();
}

// Return minimum image squared distance from r1 to r2
double CubicBox::minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    return v12.magnitudeSq();
}
