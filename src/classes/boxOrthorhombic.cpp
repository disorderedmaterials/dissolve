// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"

OrthorhombicBox::OrthorhombicBox(const Vector3 lengths) : Box(Box::BoxType::Orthorhombic, lengths, {90.0, 90.0, 90.0}) {}

/*
 * Coordinate Conversion
 */

// Convert specified fractional coordinates to real-space coordinates
void OrthorhombicBox::toReal(Vector3 &r) const
{
    r.x *= a_;
    r.y *= b_;
    r.z *= c_;
}

// Convert specified real-space coordinates to fractional coordinates
void OrthorhombicBox::toFractional(Vector3 &r) const
{
    r.x *= ra_;
    r.y *= rb_;
    r.z *= rc_;
}

/*
 * Minimum Image Calculation
 */

// Return minimum image coordinates of r1 with respect to r2
Vector3 OrthorhombicBox::minimumImage(const Vector3 &r1, const Vector3 &r2) const
{
    Vector3 v21 = r1 - r2;
    toFractional(v21);
    wrap(v21);
    toReal(v21);
    v21 += r2;
    return v21;
}

// Return minimum image vector from r1 to r2
Vector3 OrthorhombicBox::minimumVector(const Vector3 &r1, const Vector3 &r2) const
{
    Vector3 v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    return v12;
}

// Return normalised minimum image vector from r1 to r2
Vector3 OrthorhombicBox::minimumVectorN(const Vector3 &r1, const Vector3 &r2) const
{
    Vector3 v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    v12.normalise();
    return v12;
}

// Return minimum image distance from r1 to r2
double OrthorhombicBox::minimumDistance(const Vector3 &r1, const Vector3 &r2) const
{
    Vector3 v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    return v12.magnitude();
}

// Return minimum image squared distance from r1 to r2
double OrthorhombicBox::minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const
{
    Vector3 v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    return v12.magnitudeSq();
}
