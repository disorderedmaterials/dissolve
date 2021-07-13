// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"

MonoclinicBox::MonoclinicBox(const Vec3<double> lengths, double beta) : Box()
{
    type_ = Box::BoxType::Monoclinic;

    // Construct axes
    alpha_ = 90.0;
    beta_ = beta;
    gamma_ = 90.0;
    // Assume that A lays along x-axis and C lays along the z-axis (since gamma = 90)
    axes_.setColumn(0, 1.0, 0.0, 0.0);
    axes_.setColumn(1, 0.0, 1.0, 0.0);
    // The C vector will only have components in x and z
    double cosBeta = cos(beta_ / DEGRAD);
    axes_.setColumn(2, cosBeta, 0.0, sqrt(1.0 - cosBeta * cosBeta));

    // Multiply the unit vectors to have the correct lengths
    axes_.columnMultiply(0, lengths.x);
    axes_.columnMultiply(1, lengths.y);
    axes_.columnMultiply(2, lengths.z);

    // Store Box lengths
    a_ = lengths.x;
    b_ = lengths.y;
    c_ = lengths.z;

    // Finalise associated data
    finalise();
}

/*
 * Coordinate Conversion
 */

// Convert specified fractional coordinates to real-space coordinates
void MonoclinicBox::toReal(Vec3<double> &r) const
{
    auto x = r.x * axesArray_[0] + r.y * axesArray_[3] + r.z * axesArray_[6];
    auto y = r.x * axesArray_[1] + r.y * axesArray_[4] + r.z * axesArray_[7];
    auto z = r.x * axesArray_[2] + r.y * axesArray_[5] + r.z * axesArray_[8];
    r.x = x;
    r.y = y;
    r.z = z;
}

// Convert specified real-space coordinates to fractional coordinates
void MonoclinicBox::toFractional(Vec3<double> &r) const
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
Vec3<double> MonoclinicBox::minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const
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
double MonoclinicBox::minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const
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
Vec3<double> MonoclinicBox::minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const
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
double MonoclinicBox::minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const
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
