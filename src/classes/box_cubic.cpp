// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"

CubicBox::CubicBox(double length) : Box()
{
    type_ = Box::BoxType::Cubic;

    // Construct axes_
    axes_.setColumn(0, length, 0.0, 0.0);
    axes_.setColumn(1, 0.0, length, 0.0);
    axes_.setColumn(2, 0.0, 0.0, length);

    // Store box length and reciprocal
    a_ = length;
    ra_ = 1.0 / a_;

    // Finalise associated data
    finalise();
}

/*
 * Minimum Image Routines (virtual implementations)
 */

// Return minimum image coordinates of r1 with respect to r2
Vec3<double> CubicBox::minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    auto mimVec = r1;
    mimVec -= r2;

    mimVec.x -= int(mimVec.x * ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5)) * a_;
    mimVec.y -= int(mimVec.y * ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5)) * a_;
    mimVec.z -= int(mimVec.z * ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5)) * a_;

    mimVec += r2;
    return mimVec;
}

// Return minimum image vector from r1 to r2
Vec3<double> CubicBox::minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    auto mimVec = r2;
    mimVec -= r1;

    mimVec.x -= int(mimVec.x * ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5)) * a_;
    mimVec.y -= int(mimVec.y * ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5)) * a_;
    mimVec.z -= int(mimVec.z * ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5)) * a_;

    return mimVec;
}

// Return minimum image distance from r1 to r2
double CubicBox::minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    auto mimVec = r2;
    mimVec -= r1;

    mimVec.x -= int(mimVec.x * ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5)) * a_;
    mimVec.y -= int(mimVec.y * ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5)) * a_;
    mimVec.z -= int(mimVec.z * ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5)) * a_;

    return mimVec.magnitude();
}

// Return minimum image distance from r1 to r2
double CubicBox::minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    double x = r2.x - r1.x, y = r2.y - r1.y, z = r2.z - r1.z;

    x -= int(x * ra_ + (x < 0.0 ? -0.5 : 0.5)) * a_;
    y -= int(y * ra_ + (y < 0.0 ? -0.5 : 0.5)) * a_;
    z -= int(z * ra_ + (z < 0.0 ? -0.5 : 0.5)) * a_;

    return (x * x + y * y + z * z);
}

/*
 * Utility Routines (Virtual Implementations)
 */

// Return random coordinate inside Box
Vec3<double> CubicBox::randomCoordinate() const
{
    Vec3<double> pos;
    pos.x = a_ * DissolveMath::random();
    pos.y = a_ * DissolveMath::random();
    pos.z = a_ * DissolveMath::random();
    return pos;
}

// Return folded coordinate (i.e. inside current Box)
Vec3<double> CubicBox::fold(const Vec3<double> &r) const
{
    // Convert coordinate to fractional coords
    static Vec3<double> frac;
    frac.set(r.x * ra_, r.y * ra_, r.z * ra_);

    // Fold into Box
    frac.x -= floor(frac.x);
    frac.y -= floor(frac.y);
    frac.z -= floor(frac.z);

    return frac * a_;
}

// Return folded fractional coordinate (i.e. inside current Box)
Vec3<double> CubicBox::foldFrac(const Vec3<double> &r) const
{
    // Convert coordinate to fractional coords
    static Vec3<double> frac;
    frac.set(r.x * ra_, r.y * ra_, r.z * ra_);

    // Fold into Box
    frac.x -= floor(frac.x);
    frac.y -= floor(frac.y);
    frac.z -= floor(frac.z);

    return frac;
}

// Convert supplied fractional coordinates to real space
Vec3<double> CubicBox::fracToReal(const Vec3<double> &r) const { return r * a_; }
