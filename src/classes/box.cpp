// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/box.h"
#include "base/processpool.h"
#include "classes/cell.h"
#include "math/data1d.h"
#include "math/interpolator.h"
#include <algorithm>

Box::Box()
{
    type_ = Box::BoxType::Cubic;
    periodic_.set(true, true, true);
    volume_ = 0.0;
}

/*
 * Basic Definition
 */

// Return enum options for BoxType
EnumOptions<Box::BoxType> Box::boxTypes()
{
    return EnumOptions<Box::BoxType>("BoxType", {{Box::BoxType::NonPeriodic, "NonPeriodic"},
                                                 {Box::BoxType::Cubic, "Cubic"},
                                                 {Box::BoxType::Orthorhombic, "Orthorhombic"},
                                                 {Box::BoxType::Monoclinic, "Monoclinic"},
                                                 {Box::BoxType::Triclinic, "Triclinic"}});
}

// Return Box type
Box::BoxType Box::type() const { return type_; }

// Finalise Box, storing volume and reciprocal and inverted axes
void Box::finalise()
{
    // Copy axes array
    axesArray_ = axes_.matrix();

    // Calculate box volume
    volume_ = axes_.determinant();

    // Calculate inverse axes
    inverseAxes_ = axes_;
    inverseAxes_.invert();
    inverseAxesArray_ = inverseAxes_.matrix();

    /*
     * Calculate reciprocal axes and volume.
     * Reciprocal cell vectors are perpendicular to normal cell axes - calculate from cross products of normal cell vectors.
     */
    reciprocalAxes_.setColumn(0, axes_.columnAsVec3(1) * axes_.columnAsVec3(2));
    reciprocalAxes_.setColumn(1, axes_.columnAsVec3(2) * axes_.columnAsVec3(0));
    reciprocalAxes_.setColumn(2, axes_.columnAsVec3(0) * axes_.columnAsVec3(1));
    reciprocalAxes_.columnMultiply(0, TWOPI / volume_);
    reciprocalAxes_.columnMultiply(1, TWOPI / volume_);
    reciprocalAxes_.columnMultiply(2, TWOPI / volume_);
    reciprocalVolume_ = (reciprocalAxes_.columnAsVec3(1) * reciprocalAxes_.columnAsVec3(2)).dp(reciprocalAxes_.columnAsVec3(0));
}

// Return volume
double Box::volume() const { return volume_; }

// Return axis lengths
Vec3<double> Box::axisLengths() const
{
    return Vec3<double>(axes_.columnMagnitude(0), axes_.columnMagnitude(1), axes_.columnMagnitude(2));
}

// Return axis length specified
double Box::axisLength(int n) const
{
    assert(n >= 0 && n < 3);

    return axes_.columnMagnitude(n);
}

// Return axis angles
Vec3<double> Box::axisAngles() const { return Vec3<double>(axisAngle(0), axisAngle(1), axisAngle(2)); }

// Return axis angle specified
double Box::axisAngle(int n) const
{
    assert(n >= 0 && n < 3);

    Vec3<double> u, v;
    u = axes_.columnAsVec3((n + 1) % 3);
    v = axes_.columnAsVec3((n + 2) % 3);
    u.normalise();
    v.normalise();
    double dp = u.dp(v);
    return acos(dp) * DEGRAD;
}

// Return axes matrix
const Matrix3 &Box::axes() const { return axes_; }

// Return inverse axes matrix
const Matrix3 &Box::inverseAxes() const { return inverseAxes_; }

// Return reciprocal box volume
double Box::reciprocalVolume() const { return reciprocalVolume_; }

// Return reciprocal axis lengths
Vec3<double> Box::reciprocalAxisLengths() const
{
    return Vec3<double>(reciprocalAxes_.columnMagnitude(0), reciprocalAxes_.columnMagnitude(1),
                        reciprocalAxes_.columnMagnitude(2));
}

// Return reciprocal axes matrix
const Matrix3 &Box::reciprocalAxes() const { return reciprocalAxes_; }

// Scale Box by specified factor
void Box::scale(double factor)
{
    // Scale lengths
    a_ *= factor;
    b_ *= factor;
    c_ *= factor;
    ra_ = 1.0 / a_;
    rb_ = 1.0 / b_;
    rc_ = 1.0 / c_;

    // Scale axes
    axes_.columnMultiply(0, factor);
    axes_.columnMultiply(1, factor);
    axes_.columnMultiply(2, factor);

    // Calculate box volume
    volume_ = axes_.determinant();

    // Calculate inverse axes
    inverseAxes_ = axes_;
    inverseAxes_.invert();

    // Calculate reciprocal axes and volume
    // Reciprocal cell vectors are perpendicular to normal cell axes.
    // Calculate from cross products of normal cell vectors
    reciprocalAxes_.setColumn(0, axes_.columnAsVec3(1) * axes_.columnAsVec3(2));
    reciprocalAxes_.setColumn(1, axes_.columnAsVec3(2) * axes_.columnAsVec3(0));
    reciprocalAxes_.setColumn(2, axes_.columnAsVec3(0) * axes_.columnAsVec3(1));
    reciprocalAxes_.columnMultiply(0, TWOPI / volume_);
    reciprocalAxes_.columnMultiply(1, TWOPI / volume_);
    reciprocalAxes_.columnMultiply(2, TWOPI / volume_);
    reciprocalVolume_ = (reciprocalAxes_.columnAsVec3(1) * reciprocalAxes_.columnAsVec3(2)).dp(reciprocalAxes_.columnAsVec3(0));
}

/*
 * Coordinate Conversion
 */

// Return specified fractional coordinates converted to real-space coordinates
Vec3<double> Box::getReal(Vec3<double> r) const
{
    toReal(r);
    return r;
}

/*
 * Geometry Calculation
 */

// Return angle (in degrees) between coordinates
double Box::angleInDegrees(const Vec3<double> &i, const Vec3<double> &j, const Vec3<double> &k) const
{
    static Vec3<double> vecji, vecjk;
    vecji = minimumVector(j, i);
    vecjk = minimumVector(j, k);

    // Normalise vectors
    vecji.normalise();
    vecjk.normalise();

    // Determine Angle
    return angleInDegrees(vecji, vecjk);
}

// Return angle (in degrees) between supplied normalised vectors
double Box::angleInDegrees(const Vec3<double> &normji, const Vec3<double> &normjk) { return acos(normji.dp(normjk)) * DEGRAD; }

// Return angle (in degrees) between supplied normalised vectors
double Box::angleInDegrees(const Vec3<double> &normji, const Vec3<double> &normjk, double &dotProduct)
{
    dotProduct = normji.dp(normjk);
    return acos(dotProduct) * DEGRAD;
}

// Return literal angle (in degrees) between coordinates, without applying minimum image convention
double Box::literalAngleInDegrees(const Vec3<double> &i, const Vec3<double> &j, const Vec3<double> &k)
{
    static Vec3<double> vecji, vecjk;
    vecji = i - j;
    vecji.normalise();
    vecjk = k - j;
    vecjk.normalise();
    return acos(vecji.dp(vecjk)) * DEGRAD;
}

// Return torsion (in degrees) between supplied unnormalised vectors
double Box::torsionInDegrees(const Vec3<double> &vecji, const Vec3<double> &vecjk, const Vec3<double> &veckl)
{
    // Calculate cross products and torsion angle formed (in radians)
    Vec3<double> xpj, xpk;
    double magxpj, magxpk;

    return torsionInRadians(vecji, vecjk, veckl, xpj, magxpj, xpk, magxpk) * DEGRAD;
}

// Return torsion (in degrees) between supplied unnormalised vectors
double Box::torsionInDegrees(const Vec3<double> &vecji, const Vec3<double> &vecjk, const Vec3<double> &veckl, Vec3<double> &xpj,
                             double &magxpj, Vec3<double> &xpk, double &magxpk)
{
    return torsionInRadians(vecji, vecjk, veckl, xpj, magxpj, xpk, magxpk) * DEGRAD;
}

// Return torsion (in radians) between supplied unnormalised vectors
double Box::torsionInRadians(const Vec3<double> &vecji, const Vec3<double> &vecjk, const Vec3<double> &veckl)
{
    // Calculate cross products and torsion angle formed (in radians)
    Vec3<double> xpj, xpk;
    double magxpj, magxpk;

    return torsionInRadians(vecji, vecjk, veckl, xpj, magxpj, xpk, magxpk);
}

// Return torsion (in radians) between supplied unnormalised vectors, storing cross products and magnitude in supplied variables
double Box::torsionInRadians(const Vec3<double> &vecji, const Vec3<double> &vecjk, const Vec3<double> &veckl, Vec3<double> &xpj,
                             double &magxpj, Vec3<double> &xpk, double &magxpk)
{
    xpj = vecjk * vecji;
    xpk = vecjk * veckl;
    magxpj = xpj.magAndNormalise();
    magxpk = xpk.magAndNormalise();
    return atan2(vecjk.dp(xpj * xpk) / vecjk.magnitude(), xpj.dp(xpk));
}

/*
 * Utility Routines
 */

// Generate a suitable Box given the supplied relative lengths, angles
std::unique_ptr<Box> Box::generate(Vec3<double> lengths, Vec3<double> angles)
{
    // Determine box type from supplied lengths / angles
    auto rightAlpha = (fabs(angles.x - 90.0) < 0.001);
    auto rightBeta = (fabs(angles.y - 90.0) < 0.001);
    auto rightGamma = (fabs(angles.z - 90.0) < 0.001);

    if (rightAlpha && rightBeta && rightGamma)
    {
        auto abSame = (fabs(lengths.x - lengths.y) < 0.0001);
        auto acSame = (fabs(lengths.x - lengths.z) < 0.0001);
        if (abSame && acSame)
            return std::make_unique<CubicBox>(lengths.x);
        else
            return std::make_unique<OrthorhombicBox>(lengths);
    }
    else if (rightAlpha && (!rightBeta) && rightGamma)
        return std::make_unique<MonoclinicBox>(lengths, angles.y);
    else
        return std::make_unique<TriclinicBox>(lengths, angles);
}

// Return radius of largest possible inscribed sphere for box
double Box::inscribedSphereRadius() const
{
    // Radius of largest inscribed sphere is the smallest of the three calculated values....
    double mag, diameter, result = 0.0;
    Vec3<double> cross;
    int n, i, j;
    for (n = 0; n < 3; ++n)
    {
        i = (n + 1) % 3;
        j = (n + 2) % 3;
        cross = axes_.columnAsVec3(i) * axes_.columnAsVec3(j);
        mag = cross.magnitude();
        diameter = fabs(axes_.columnAsVec3(n).dp(cross / mag));
        if (n == 0)
            result = diameter;
        else if (diameter < result)
            result = diameter;
    }
    return result * 0.5;
}

// Return random coordinate inside Box
Vec3<double> Box::randomCoordinate() const
{
    Vec3<double> r(DissolveMath::random(), DissolveMath::random(), DissolveMath::random());
    toReal(r);
    return r;
}

// Return folded coordinate (i.e. inside current Box)
Vec3<double> Box::fold(const Vec3<double> &r) const
{
    // Convert coordinate to fractional coords
    auto frac = r;
    toFractional(frac);

    // Fold into Box
    frac.x -= floor(frac.x);
    frac.y -= floor(frac.y);
    frac.z -= floor(frac.z);

    toReal(frac);

    return frac;
}

// Return folded fractional coordinate (i.e. inside current Box)
Vec3<double> Box::foldFrac(const Vec3<double> &r) const
{
    // Convert coordinate to fractional coords
    auto frac = r;
    toFractional(frac);

    // Fold into Box
    frac.x -= floor(frac.x);
    frac.y -= floor(frac.y);
    frac.z -= floor(frac.z);

    return frac;
}
