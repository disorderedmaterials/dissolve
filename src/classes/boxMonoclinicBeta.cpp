// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"

MonoclinicBetaBox::MonoclinicBetaBox(const Vec3<double> lengths, double beta)
    : Box(Box::BoxType::MonoclinicBeta, lengths, {90.0, beta, 90.0})
{
}

/*
 * Coordinate Conversion
 */

// Convert specified fractional coordinates to real-space coordinates
void MonoclinicBetaBox::toReal(Vec3<double> &r) const
{
    r.x *= axesArray_[0];
    r.x += r.z * axesArray_[6];
    r.y *= axesArray_[4];
    r.z *= axesArray_[8];
}

// Convert specified real-space coordinates to fractional coordinates
void MonoclinicBetaBox::toFractional(Vec3<double> &r) const
{
    r.x *= inverseAxesArray_[0];
    r.x += r.z * inverseAxesArray_[6];
    r.y *= inverseAxesArray_[4];
    r.z *= inverseAxesArray_[8];
}

/*
 * Minimum Image Calculation
 */

// Return minimum image coordinates of r1 with respect to r2
Vec3<double> MonoclinicBetaBox::minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v21 = r1 - r2;

    toFractional(v21);
    wrap(v21);
    toReal(v21);
    v21 += r2;

    return v21;
}

// Return minimum image vector from r1 to r2
Vec3<double> MonoclinicBetaBox::minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v12 = r2 - r1;

    toFractional(v12);
    wrap(v12);
    toReal(v12);

    return v12;
}

// Return normalised minimum image vector from r1 to r2
Vec3<double> MonoclinicBetaBox::minimumVectorN(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    v12.normalise();
    return v12;
}

// Return minimum image distance from r1 to r2
double MonoclinicBetaBox::minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v12 = r2 - r1;

    toFractional(v12);
    wrap(v12);
    toReal(v12);

    return v12.magnitude();
}

// Return minimum image squared distance from r1 to r2
double MonoclinicBetaBox::minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v12 = r2 - r1;

    toFractional(v12);
    wrap(v12);
    toReal(v12);

    return v12.magnitudeSq();
}
