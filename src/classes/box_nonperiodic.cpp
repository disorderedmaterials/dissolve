// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "templates/array.h"

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

// Return minimum image coordinates of 'i' with respect to 'ref'
Vec3<double> NonPeriodicBox::minimumImage(const Vec3<double> &i, const Vec3<double> &ref) const { return i; }

// Return minimum image vector from 'i' to 'j'
Vec3<double> NonPeriodicBox::minimumVector(const Vec3<double> &i, const Vec3<double> &j) const { return j - i; }

// Return minimum image distance from 'i' to 'j'
double NonPeriodicBox::minimumDistance(const Vec3<double> &i, const Vec3<double> &j) const { return (j - i).magnitude(); }

// Return minimum image squared distance from 'i' to 'j'
double NonPeriodicBox::minimumDistanceSquared(const Vec3<double> &i, const Vec3<double> &j) const
{
    return (j - i).magnitudeSq();
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
Vec3<double> NonPeriodicBox::fracToReal(const Vec3<double> &r) const
{
    // Multiply by box length
    return r * a_;
}
