// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"
#include "classes/cell.h"

MonoclinicBox::MonoclinicBox(const Vec3<double> lengths, double beta) : Box()
{
    type_ = Box::MonoclinicBoxType;

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

MonoclinicBox::~MonoclinicBox() {}

/*
 * Minimum Image Routines (virtual implementations)
 */

// Return minimum image coordinates of 'i' with respect to 'j'
Vec3<double> MonoclinicBox::minimumImage(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> ref) const
{
    // TODO Can speed this up since we know which matrix elements are zero
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

// Return minimum image coordinates of 'i' with respect to 'j'
Vec3<double> MonoclinicBox::minimumImage(const std::shared_ptr<Atom> i, const Vec3<double> &ref) const
{
    // TODO Can speed this up since we know which matrix elements are zero
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

// Return minimum image coordinates of 'i' with respect to 'j'
Vec3<double> MonoclinicBox::minimumImage(const Vec3<double> &i, const Vec3<double> &ref) const
{
    // TODO Can speed this up since we know which matrix elements are zero
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
Vec3<double> MonoclinicBox::minimumVector(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const
{
    // TODO Can speed this up since we know which matrix elements are zero
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
Vec3<double> MonoclinicBox::minimumVector(const Atom &i, const Atom &j) const
{
    // TODO Can speed this up since we know which matrix elements are zero
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
Vec3<double> MonoclinicBox::minimumVector(const std::shared_ptr<Atom> i, const Vec3<double> &j) const
{
    // TODO Can speed this up since we know which matrix elements are zero
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
Vec3<double> MonoclinicBox::minimumVector(const Vec3<double> &i, const Vec3<double> &j) const
{
    // TODO Can speed this up since we know which matrix elements are zero
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
double MonoclinicBox::minimumDistance(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const
{
    return minimumVector(i, j).magnitude();
}

// Return minimum image distance from 'i' to 'j'
double MonoclinicBox::minimumDistance(const Atom &i, const Atom &j) const { return minimumVector(i, j).magnitude(); }

// Return minimum image distance from 'i' to 'j'
double MonoclinicBox::minimumDistance(const std::shared_ptr<Atom> i, const Vec3<double> &j) const
{
    return minimumVector(i, j).magnitude();
}

// Return minimum image distance from 'i' to 'j'
double MonoclinicBox::minimumDistance(const Vec3<double> &i, const Vec3<double> &j) const
{
    return minimumVector(i, j).magnitude();
}

// Return minimum image squared distance from 'i' to 'j' (pointers)
double MonoclinicBox::minimumDistanceSquared(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const
{
    return minimumVector(i, j).magnitudeSq();
}

// Return minimum image squared distance from 'i' to 'j' (references)
double MonoclinicBox::minimumDistanceSquared(const Atom &i, const Atom &j) const
{
    return minimumVector(i.r(), j.r()).magnitudeSq();
}

// Return minimum image squared distance from 'i' to 'j'
double MonoclinicBox::minimumDistanceSquared(const std::shared_ptr<Atom> i, const Vec3<double> &j) const
{
    return minimumVector(i, j).magnitudeSq();
}

// Return minimum image squared distance from 'i' to 'j'
double MonoclinicBox::minimumDistanceSquared(const Vec3<double> &i, const Vec3<double> &j) const
{
    return minimumVector(i, j).magnitudeSq();
}

/*
 * Utility Routines (Virtual Implementations)
 */

// Return random coordinate inside Box
Vec3<double> MonoclinicBox::randomCoordinate() const
{
    Vec3<double> pos(DissolveMath::random(), DissolveMath::random(), DissolveMath::random());
    return axes_ * pos;
}

// Return folded coordinate (i.e. inside current Box)
Vec3<double> MonoclinicBox::fold(const Vec3<double> &r) const
{
    // TODO Can speed this up part since we know which matrix elements are zero

    // Convert coordinate to fractional coords
    auto frac = inverseAxes_ * r;

    // Fold into Box and remultiply by inverse matrix
    frac.x -= floor(frac.x);
    frac.y -= floor(frac.y);
    frac.z -= floor(frac.z);

    return axes_ * frac;
}

// Return folded fractional coordinate (i.e. inside current Box)
Vec3<double> MonoclinicBox::foldFrac(const Vec3<double> &r) const
{
    // TODO Can speed this up part since we know which matrix elements are zero

    // Convert coordinate to fractional coords
    auto frac = inverseAxes_ * r;

    // Fold into Box and remultiply by inverse matrix
    frac.x -= floor(frac.x);
    frac.y -= floor(frac.y);
    frac.z -= floor(frac.z);

    return frac;
}

// Convert supplied fractional coordinates to real space
Vec3<double> MonoclinicBox::fracToReal(const Vec3<double> &r) const
{
    // Multiply by axes matrix
    // TODO Can speed this up part since we know which matrix elements are zero
    return axes_ * r;
}
