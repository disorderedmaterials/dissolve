// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configurationAtom.h"

NonPeriodicBox::NonPeriodicBox(double length) : Box(Box::BoxType::NonPeriodic, {length, length, length}, {90.0, 90.0, 90.0}) {}

/*
 * Coordinate Conversion
 */

// Convert specified fractional coordinates to real-space coordinates
void NonPeriodicBox::toReal(Vector3 &r) const
{
    r.x *= a_;
    r.y *= a_;
    r.z *= a_;
}

// Convert specified real-space coordinates to fractional coordinates
void NonPeriodicBox::toFractional(Vector3 &r) const
{
    r.x *= ra_;
    r.y *= ra_;
    r.z *= ra_;
}

/*
 * Minimum Image Calculation
 */

// Return minimum image coordinates of r1 with respect to r2
Vector3 NonPeriodicBox::minimumImage(const Vector3 &r1, const Vector3 &r2) const
{
    Vector3 v21 = r1 - r2;
    toFractional(v21);
    wrap(v21);
    toReal(v21);
    v21 += r2;
    return v21;
}

// Return minimum image vector from r1 to r2
Vector3 NonPeriodicBox::minimumVector(const Vector3 &r1, const Vector3 &r2) const
{
    Vector3 v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    return v12;
}

// Return minimum image distance from r1 to r2
double NonPeriodicBox::minimumDistance(const Vector3 &r1, const Vector3 &r2) const
{
    Vector3 v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    return v12.magnitude();
}

// Return minimum image squared distance from r1 to r2
double NonPeriodicBox::minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const
{
    Vector3 v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    return v12.magnitudeSq();
}
