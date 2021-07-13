// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"

NonPeriodicBox::NonPeriodicBox(double length) : Box()
{
    type_ = Box::BoxType::NonPeriodic;
    periodic_.set(false, false, false);

    // Construct axes_
    axes_.setColumn(0, length, 0.0, 0.0);
    axes_.setColumn(1, 0.0, length, 0.0);
    axes_.setColumn(2, 0.0, 0.0, length);

    // Store Box length
    a_ = length;
    ra_ = 1.0 / a_;

    // Finalise associated data
    finalise();
}

/*
 * Minimum Image Routines (virtual implementations)
 */

// Return minimum image coordinates of r1 with respect to r2
Vec3<double> NonPeriodicBox::minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const { return r1; }

// Return minimum image vector from r1 to r2
Vec3<double> NonPeriodicBox::minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const { return r2 - r1; }

// Return minimum image distance from r1 to r2
double NonPeriodicBox::minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const { return (r2 - r1).magnitude(); }

// Return minimum image squared distance from r1 to r2
double NonPeriodicBox::minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const
{
    return (r2 - r1).magnitudeSq();
}

/*
 * Utility Routines (Virtual Implementations)
 */

// Return random coordinate inside Box
Vec3<double> NonPeriodicBox::randomCoordinate() const
{
    static Vec3<double> pos;
    pos.x = a_ * DissolveMath::random();
    pos.y = a_ * DissolveMath::random();
    pos.z = a_ * DissolveMath::random();
    return pos;
}

// Return folded coordinate (i.e. inside current Box)
Vec3<double> NonPeriodicBox::fold(const Vec3<double> &r) const
{
    // Convert coordinate to fractional coords
    static Vec3<double> frac;
    frac.set(r.x * ra_, r.y * ra_, r.z * ra_);

    // Fold into Box and divide by integer Cell sizes
    frac.x -= floor(frac.x);
    frac.y -= floor(frac.y);
    frac.z -= floor(frac.z);
    return frac * a_;
}

// Return folded coordinate (i.e. inside current Box)
Vec3<double> NonPeriodicBox::foldFrac(const Vec3<double> &r) const
{
    // Convert coordinate to fractional coords
    static Vec3<double> frac;
    frac.set(r.x * ra_, r.y * ra_, r.z * ra_);

    // Fold into Box and divide by integer Cell sizes
    frac.x -= floor(frac.x);
    frac.y -= floor(frac.y);
    frac.z -= floor(frac.z);
    return frac;
}

// Convert supplied fractional coordinates to real space
Vec3<double> NonPeriodicBox::fracToReal(const Vec3<double> &r) const { return r * a_; }
