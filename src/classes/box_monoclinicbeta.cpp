// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    auto x = r.x * axesArray_[0] + r.y * axesArray_[3] + r.z * axesArray_[6];
    auto y = r.x * axesArray_[1] + r.y * axesArray_[4] + r.z * axesArray_[7];
    auto z = r.x * axesArray_[2] + r.y * axesArray_[5] + r.z * axesArray_[8];
    r.x = x;
    r.y = y;
    r.z = z;
}

// Convert specified real-space coordinates to fractional coordinates
void MonoclinicBetaBox::toFractional(Vec3<double> &r) const
{
    auto x = r.x * inverseAxesArray_[0] + r.y * inverseAxesArray_[3] + r.z * inverseAxesArray_[6];
    auto y = r.x * inverseAxesArray_[1] + r.y * inverseAxesArray_[4] + r.z * inverseAxesArray_[7];
    auto z = r.x * inverseAxesArray_[2] + r.y * inverseAxesArray_[5] + r.z * inverseAxesArray_[8];
    r.x = x;
    r.y = y;
    r.z = z;
}

/*
 * Minimum Image Calculation
 */

// Return minimum image coordinates of r1 with respect to r2
Vec3<double> MonoclinicBetaBox::minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v21 = r1 - r2;
    Vec3<double> rFrac(v21.x * inverseAxesArray_[0] + v21.y * inverseAxesArray_[3] + v21.z * inverseAxesArray_[6],
                       v21.x * inverseAxesArray_[1] + v21.y * inverseAxesArray_[4] + v21.z * inverseAxesArray_[7],
                       v21.x * inverseAxesArray_[2] + v21.y * inverseAxesArray_[5] + v21.z * inverseAxesArray_[8]);

    wrap(rFrac);

    v21.x = rFrac.x * axesArray_[0] + rFrac.y * axesArray_[3] + rFrac.z * axesArray_[6] + r2.x;
    v21.y = rFrac.x * axesArray_[1] + rFrac.y * axesArray_[4] + rFrac.z * axesArray_[7] + r2.y;
    v21.z = rFrac.x * axesArray_[2] + rFrac.y * axesArray_[5] + rFrac.z * axesArray_[8] + r2.z;

    return v21;
}

// Return minimum image distance from r1 to r2
double MonoclinicBetaBox::minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v12 = r2 - r1;
    Vec3<double> rFrac(v12.x * inverseAxesArray_[0] + v12.y * inverseAxesArray_[3] + v12.z * inverseAxesArray_[6],
                       v12.x * inverseAxesArray_[1] + v12.y * inverseAxesArray_[4] + v12.z * inverseAxesArray_[7],
                       v12.x * inverseAxesArray_[2] + v12.y * inverseAxesArray_[5] + v12.z * inverseAxesArray_[8]);

    wrap(rFrac);

    v12.x = rFrac.x * axesArray_[0] + rFrac.y * axesArray_[3] + rFrac.z * axesArray_[6];
    v12.y = rFrac.x * axesArray_[1] + rFrac.y * axesArray_[4] + rFrac.z * axesArray_[7];
    v12.z = rFrac.x * axesArray_[2] + rFrac.y * axesArray_[5] + rFrac.z * axesArray_[8];

    return v12.magnitude();
}

// Return minimum image vector from r1 to r2
Vec3<double> MonoclinicBetaBox::minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v12 = r2 - r1;
    Vec3<double> rFrac(v12.x * inverseAxesArray_[0] + v12.y * inverseAxesArray_[3] + v12.z * inverseAxesArray_[6],
                       v12.x * inverseAxesArray_[1] + v12.y * inverseAxesArray_[4] + v12.z * inverseAxesArray_[7],
                       v12.x * inverseAxesArray_[2] + v12.y * inverseAxesArray_[5] + v12.z * inverseAxesArray_[8]);

    wrap(rFrac);

    v12.x = rFrac.x * axesArray_[0] + rFrac.y * axesArray_[3] + rFrac.z * axesArray_[6];
    v12.y = rFrac.x * axesArray_[1] + rFrac.y * axesArray_[4] + rFrac.z * axesArray_[7];
    v12.z = rFrac.x * axesArray_[2] + rFrac.y * axesArray_[5] + rFrac.z * axesArray_[8];

    return v12;
}

// Return minimum image squared distance from r1 to r2
double MonoclinicBetaBox::minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    Vec3<double> v12 = r2 - r1;
    Vec3<double> rFrac(v12.x * inverseAxesArray_[0] + v12.y * inverseAxesArray_[3] + v12.z * inverseAxesArray_[6],
                       v12.x * inverseAxesArray_[1] + v12.y * inverseAxesArray_[4] + v12.z * inverseAxesArray_[7],
                       v12.x * inverseAxesArray_[2] + v12.y * inverseAxesArray_[5] + v12.z * inverseAxesArray_[8]);

    wrap(rFrac);

    v12.x = rFrac.x * axesArray_[0] + rFrac.y * axesArray_[3] + rFrac.z * axesArray_[6];
    v12.y = rFrac.x * axesArray_[1] + rFrac.y * axesArray_[4] + rFrac.z * axesArray_[7];
    v12.z = rFrac.x * axesArray_[2] + rFrac.y * axesArray_[5] + rFrac.z * axesArray_[8];

    return v12.magnitudeSq();
}
