// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"
#include "classes/cell.h"

TriclinicBox::TriclinicBox(const Vec3<double> lengths, const Vec3<double> angles) : Box()
{
    type_ = Box::TriclinicBoxType;

    // Construct axes_
    alpha_ = angles.x;
    beta_ = angles.y;
    gamma_ = angles.z;

    // Assume that A lays along x-axis
    axes_.setColumn(0, 1.0, 0.0, 0.0);

    // Assume that B lays in the xy plane. Since A={1,0,0}, cos(gamma) equals 'x' of the B vector.
    double temp = cos(gamma_ / DEGRAD);
    axes_.setColumn(1, temp, sqrt(1.0 - temp * temp), 0.0);

    // The C vector can now be determined in parts.
    // It's x-component is equal to cos(beta) since {1,0,0}{x,y,z} = {1}{x} = cos(beta)
    axes_.setColumn(2, cos(beta_ / DEGRAD), 0.0, 0.0);

    // The y-component can be determined by completing the dot product between the B and C vectors
    axes_[7] = (cos(alpha_ / DEGRAD) - axes_[3] * axes_[6]) / axes_[4];

    // The z-component is simply the remainder of the unit vector...
    axes_[8] = sqrt(1.0 - axes_[6] * axes_[6] - axes_[7] * axes_[7]);

    // Multiply the unit vectors to have the correct lengths
    axes_.columnMultiply(0, lengths.x);
    axes_.columnMultiply(1, lengths.y);
    axes_.columnMultiply(2, lengths.z);

    // Finalise associated data
    finalise();
}

TriclinicBox::~TriclinicBox() {}

/*
 * Minimum Image Routines (virtual implementations)
 */

// Return minimum image coordinates of 'i' with respect to 'ref'
Vec3<double> TriclinicBox::minimumImage(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> ref) const
{
    auto mim = inverseAxes_ * (ref->r() - i->r());
    if (mim.x < -0.5)
        mim.x += 1.0;
    else if (mim.x > 0.5)
        mim.x -= 1.0;
    if (mim.y < -0.5)
        mim.y += 1.0;
    else if (mim.y > 0.5)
        mim.y -= 1.0;
    if (mim.z < -0.5)
        mim.z += 1.0;
    else if (mim.z > 0.5)
        mim.z -= 1.0;
    return axes_ * mim + ref->r();
}

// Return minimum image coordinates of 'i' with respect to 'ref'
Vec3<double> TriclinicBox::minimumImage(const std::shared_ptr<Atom> i, const Vec3<double> &ref) const
{
    auto mim = inverseAxes_ * (ref - i->r());
    if (mim.x < -0.5)
        mim.x += 1.0;
    else if (mim.x > 0.5)
        mim.x -= 1.0;
    if (mim.y < -0.5)
        mim.y += 1.0;
    else if (mim.y > 0.5)
        mim.y -= 1.0;
    if (mim.z < -0.5)
        mim.z += 1.0;
    else if (mim.z > 0.5)
        mim.z -= 1.0;
    return axes_ * mim + ref;
}

// Return minimum image coordinates of 'i' with respect to 'ref'
Vec3<double> TriclinicBox::minimumImage(const Vec3<double> &i, const Vec3<double> &ref) const
{
    auto mim = inverseAxes_ * (ref - i);
    if (mim.x < -0.5)
        mim.x += 1.0;
    else if (mim.x > 0.5)
        mim.x -= 1.0;
    if (mim.y < -0.5)
        mim.y += 1.0;
    else if (mim.y > 0.5)
        mim.y -= 1.0;
    if (mim.z < -0.5)
        mim.z += 1.0;
    else if (mim.z > 0.5)
        mim.z -= 1.0;
    return axes_ * mim + ref;
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> TriclinicBox::minimumVector(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const
{
    auto mim = inverseAxes_ * (j->r() - i->r());
    if (mim.x < -0.5)
        mim.x += 1.0;
    else if (mim.x > 0.5)
        mim.x -= 1.0;
    if (mim.y < -0.5)
        mim.y += 1.0;
    else if (mim.y > 0.5)
        mim.y -= 1.0;
    if (mim.z < -0.5)
        mim.z += 1.0;
    else if (mim.z > 0.5)
        mim.z -= 1.0;
    return axes_ * mim;
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> TriclinicBox::minimumVector(const Atom &i, const Atom &j) const
{
    auto mim = inverseAxes_ * (j.r() - i.r());
    if (mim.x < -0.5)
        mim.x += 1.0;
    else if (mim.x > 0.5)
        mim.x -= 1.0;
    if (mim.y < -0.5)
        mim.y += 1.0;
    else if (mim.y > 0.5)
        mim.y -= 1.0;
    if (mim.z < -0.5)
        mim.z += 1.0;
    else if (mim.z > 0.5)
        mim.z -= 1.0;
    return axes_ * mim;
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> TriclinicBox::minimumVector(const std::shared_ptr<Atom> i, const Vec3<double> &j) const
{
    auto mim = inverseAxes_ * (j - i->r());

    if (mim.x < -0.5)
        mim.x += 1.0;
    else if (mim.x > 0.5)
        mim.x -= 1.0;
    if (mim.y < -0.5)
        mim.y += 1.0;
    else if (mim.y > 0.5)
        mim.y -= 1.0;
    if (mim.z < -0.5)
        mim.z += 1.0;
    else if (mim.z > 0.5)
        mim.z -= 1.0;
    return axes_ * mim;
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> TriclinicBox::minimumVector(const Vec3<double> &i, const Vec3<double> &j) const
{
    auto mim = inverseAxes_ * (j - i);
    if (mim.x < -0.5)
        mim.x += 1.0;
    else if (mim.x > 0.5)
        mim.x -= 1.0;
    if (mim.y < -0.5)
        mim.y += 1.0;
    else if (mim.y > 0.5)
        mim.y -= 1.0;
    if (mim.z < -0.5)
        mim.z += 1.0;
    else if (mim.z > 0.5)
        mim.z -= 1.0;
    return axes_ * mim;
}

// Return minimum image distance from 'i' to 'j'
double TriclinicBox::minimumDistance(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const
{
    return minimumVector(i, j).magnitude();
}

// Return minimum image distance from 'i' to 'j'
double TriclinicBox::minimumDistance(const Atom &i, const Atom &j) const { return minimumVector(i, j).magnitude(); }

// Return minimum image distance from 'i' to 'j'
double TriclinicBox::minimumDistance(const std::shared_ptr<Atom> i, const Vec3<double> &j) const
{
    return minimumVector(i, j).magnitude();
}

// Return minimum image distance from 'i' to 'j'
double TriclinicBox::minimumDistance(const Vec3<double> &i, const Vec3<double> &j) const
{
    return minimumVector(i, j).magnitude();
}

// Return minimum image squared distance from 'i' to 'j' (pointers)
double TriclinicBox::minimumDistanceSquared(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const
{
    return minimumVector(i, j).magnitudeSq();
}

// Return minimum image squared distance from 'i' to 'j' (references)
double TriclinicBox::minimumDistanceSquared(const Atom &i, const Atom &j) const
{
    return minimumVector(i.r(), j.r()).magnitudeSq();
}

// Return minimum image squared distance from 'i' to 'j'
double TriclinicBox::minimumDistanceSquared(const std::shared_ptr<Atom> i, const Vec3<double> &j) const
{
    return minimumVector(i, j).magnitudeSq();
}

// Return minimum image squared distance from 'i' to 'j'
double TriclinicBox::minimumDistanceSquared(const Vec3<double> &i, const Vec3<double> &j) const
{
    return minimumVector(i, j).magnitudeSq();
}

/*
 * Utility Routines (Virtual Implementations)
 */

// Return random coordinate inside Box
Vec3<double> TriclinicBox::randomCoordinate() const
{
    Vec3<double> pos(DissolveMath::random(), DissolveMath::random(), DissolveMath::random());
    return axes_ * pos;
}

// Return folded coordinate (i.e. inside current Box)
Vec3<double> TriclinicBox::fold(const Vec3<double> &r) const
{
    // Convert coordinate to fractional coords
    auto frac = inverseAxes_ * r;

    // Fold into Box and remultiply by inverse matrix
    frac.x -= floor(frac.x);
    frac.y -= floor(frac.y);
    frac.z -= floor(frac.z);

    return axes_ * frac;
}

// Return folded fractional coordinate (i.e. inside current Box)
Vec3<double> TriclinicBox::foldFrac(const Vec3<double> &r) const
{
    // Convert coordinate to fractional coords
    auto frac = inverseAxes_ * r;

    // Fold into Box and remultiply by inverse matrix
    frac.x -= floor(frac.x);
    frac.y -= floor(frac.y);
    frac.z -= floor(frac.z);

    return frac;
}

// Convert supplied fractional coordinates to real space
Vec3<double> TriclinicBox::fracToReal(const Vec3<double> &r) const
{
    // Multiply by axes matrix
    // TODO Can speedup this part since we know which matrix elements are zero
    return axes_ * r;
}
