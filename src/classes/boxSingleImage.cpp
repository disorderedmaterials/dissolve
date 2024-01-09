// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"

SingleImageBox::SingleImageBox() : Box(Box::BoxType::NonPeriodic, {0.0, 0.0, 0.0}, {90.0, 90.0, 90.0}) {}

/*
 * Coordinate Conversion
 */

// Convert specified fractional coordinates to real-space coordinates
void SingleImageBox::toReal(Vec3<double> &r) const
{
    throw(std::runtime_error("Can't convert to real coordinates in a SingleImage box.\n"));
}

// Convert specified real-space coordinates to fractional coordinates
void SingleImageBox::toFractional(Vec3<double> &r) const
{
    throw(std::runtime_error("Can't convert to fractional coordinates in a SingleImage box.\n"));
}

/*
 * Minimum Image Calculation
 */

// Return minimum image coordinates of r1 with respect to r2
Vec3<double> SingleImageBox::minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const { return r1; }

// Return minimum image vector from r1 to r2
Vec3<double> SingleImageBox::minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const { return r2 - r1; }

// Return normalised minimum image vector from r1 to r2
Vec3<double> SingleImageBox::minimumVectorN(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    auto v12 = r2 - r1;
    v12.normalise();
    return v12;
}

// Return minimum image distance from r1 to r2
double SingleImageBox::minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const { return (r2 - r1).magnitude(); }

// Return minimum image squared distance from r1 to r2
double SingleImageBox::minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    return (r2 - r1).magnitudeSq();
}
