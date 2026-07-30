// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/cell.h"
#include "math/mathFunc.h"

Box::Box(Box::BoxType boxType, const Vector3 lengths, const Vector3 angles)
    : type_(boxType), a_(lengths.x), b_(lengths.y), c_(lengths.z), ra_(1.0 / lengths.x), rb_(1.0 / lengths.y),
      rc_(1.0 / lengths.z), alpha_(angles.x), beta_(angles.y), gamma_(angles.z)
{
    // Set periodicity flags
    periodic_ = {type_ != BoxType::None, type_ != BoxType::None, type_ != BoxType::None};

    // Construct axes matrix
    axes_.setIdentity();

    // Determine cosines of box angles, zeroing if below some tolerance to clean up matrices
    auto cosAlpha = cos(DissolveMath::toRadians(alpha_));
    if (fabs(cosAlpha) < 1.0e-10)
        cosAlpha = 0;
    auto cosBeta = cos(DissolveMath::toRadians(beta_));
    if (fabs(cosBeta) < 1.0e-10)
        cosBeta = 0;
    auto cosGamma = cos(DissolveMath::toRadians(gamma_));
    if (fabs(cosGamma) < 1.0e-10)
        cosGamma = 0;

    // We set A={1,0,0}, so cos(gamma) equals 'x' of the B vector
    axes_.setColumn(1, cosGamma, sqrt(1.0 - cosGamma * cosGamma), 0.0);

    // The C vector can now be determined in parts.
    // - C.x = cos(beta)    (since {1,0,0}{x,y,z} = {1}{x} = cos(beta))
    // - C.y can be determined by completing the dot product between the B and C vectors
    // - C.z is the remainder of the unit vector
    axes_[6] = cosBeta;
    axes_[7] = (cosAlpha - axes_[3] * axes_[6]) / axes_[4];
    axes_[8] = sqrt(1.0 - axes_[6] * axes_[6] - axes_[7] * axes_[7]);

    // Multiply the unit vectors to have the correct lengths
    axes_.columnMultiply(0, a_);
    axes_.columnMultiply(1, b_);
    axes_.columnMultiply(2, c_);

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
    reciprocalAxes_.columnMultiply(0, 2.0 * M_PI / volume_);
    reciprocalAxes_.columnMultiply(1, 2.0 * M_PI / volume_);
    reciprocalAxes_.columnMultiply(2, 2.0 * M_PI / volume_);
    reciprocalVolume_ = (reciprocalAxes_.columnAsVec3(1) * reciprocalAxes_.columnAsVec3(2)).dp(reciprocalAxes_.columnAsVec3(0));
}

Box::Box(const Box &other)
{
    type_ = other.type_;
    a_ = other.a_;
    b_ = other.b_;
    c_ = other.c_;
    ra_ = other.ra_;
    rb_ = other.rb_;
    rc_ = other.rc_;
    alpha_ = other.alpha_;
    beta_ = other.beta_;
    gamma_ = other.gamma_;
    axes_ = other.axes_;
    axesArray_ = other.axesArray_;
    inverseAxes_ = other.inverseAxes_;
    inverseAxesArray_ = other.inverseAxesArray_;
    reciprocalAxes_ = other.reciprocalAxes_;
    periodic_ = other.periodic_;
    volume_ = other.volume_;
    reciprocalVolume_ = other.reciprocalVolume_;
}

/*
 * Basic Definition
 */

// Return enum options for BoxType
EnumOptions<Box::BoxType> Box::boxTypes()
{
    return EnumOptions<Box::BoxType>("BoxType", {{Box::BoxType::None, "None"},
                                                 {Box::BoxType::Cubic, "Cubic"},
                                                 {Box::BoxType::Orthorhombic, "Orthorhombic"},
                                                 {Box::BoxType::MonoclinicAlpha, "MonoclinicAlpha"},
                                                 {Box::BoxType::MonoclinicBeta, "MonoclinicBeta"},
                                                 {Box::BoxType::MonoclinicGamma, "MonoclinicGamma"},
                                                 {Box::BoxType::Triclinic, "Triclinic"}});
}

// Return Box type
Box::BoxType Box::type() const { return type_; }

// Determine Box type
std::optional<Box::BoxType> Box::type(Vector3 lengths, Vector3 angles)
{
    // Check lengths
    if (lengths.min() < 1.0e-5 || angles.min() < 1.0)
        return {};

    // Determine any right angles
    auto rightAlpha = (fabs(angles.x - 90.0) < 1.0e-5);
    auto rightBeta = (fabs(angles.y - 90.0) < 1.0e-5);
    auto rightGamma = (fabs(angles.z - 90.0) < 1.0e-5);
    auto nRight = rightAlpha + rightBeta + rightGamma;

    if (nRight == 3)
    {
        auto abSame = (fabs(lengths.x - lengths.y) < 1.0e-5);
        auto acSame = (fabs(lengths.x - lengths.z) < 1.0e-5);
        if (abSame && acSame)
            return BoxType::Cubic;
        else
            return BoxType::Orthorhombic;
    }
    else if (nRight == 2)
    {
        if (!rightAlpha)
            return BoxType::MonoclinicAlpha;
        else if (!rightBeta)
            return BoxType::MonoclinicBeta;
        else
            return BoxType::MonoclinicGamma;
    }
    else
        return BoxType::Triclinic;
}

// Return volume
double Box::volume() const { return volume_; }

// Return axis lengths
Vector3 Box::axisLengths() const
{
    return Vector3(axes_.columnMagnitude(0), axes_.columnMagnitude(1), axes_.columnMagnitude(2));
}

// Return axis length specified
double Box::axisLength(int n) const
{
    assert(n >= 0 && n < 3);

    return axes_.columnMagnitude(n);
}

// Return axis angles
Vector3 Box::axisAngles() const { return Vector3(axisAngle(0), axisAngle(1), axisAngle(2)); }

// Return axis angle specified
double Box::axisAngle(int n) const
{
    assert(n >= 0 && n < 3);

    Vector3 u, v;
    u = axes_.columnAsVec3((n + 1) % 3);
    v = axes_.columnAsVec3((n + 2) % 3);
    u.normalise();
    v.normalise();
    double dp = u.dp(v);
    return DissolveMath::toDegrees(acos(dp));
}

// Return axes matrix
const Matrix3 &Box::axes() const { return axes_; }

// Return inverse axes matrix
const Matrix3 &Box::inverseAxes() const { return inverseAxes_; }

// Return reciprocal box volume
double Box::reciprocalVolume() const { return reciprocalVolume_; }

// Return reciprocal axis lengths
Vector3 Box::reciprocalAxisLengths() const
{
    return Vector3(reciprocalAxes_.columnMagnitude(0), reciprocalAxes_.columnMagnitude(1), reciprocalAxes_.columnMagnitude(2));
}

// Return reciprocal axes matrix
const Matrix3 &Box::reciprocalAxes() const { return reciprocalAxes_; }

// Scale Box lengths by specified factors
void Box::scale(Vector3 scaleFactors)
{
    // Make a check here for cubic boxes, in which case all elements in scaleFactors must be equal
    if (type_ == BoxType::Cubic)
    {
        auto abSame = (fabs(scaleFactors.x - scaleFactors.y) < 1.0e-5);
        auto acSame = (fabs(scaleFactors.x - scaleFactors.z) < 1.0e-5);
        if (!abSame || !acSame)
            Messenger::exception("Irregular scaling of cubic box requested (scale factors = {}, {}, {}\n", scaleFactors.x,
                                 scaleFactors.y, scaleFactors.z);
    }

    // Scale lengths
    a_ *= scaleFactors.x;
    b_ *= scaleFactors.y;
    c_ *= scaleFactors.z;
    ra_ = 1.0 / a_;
    rb_ = 1.0 / b_;
    rc_ = 1.0 / c_;

    // Scale axes
    axes_.columnMultiply(scaleFactors);
    axesArray_ = axes_.matrix();

    // Calculate box volume
    volume_ = axes_.determinant();

    // Calculate inverse axes
    inverseAxes_ = axes_;
    inverseAxes_.invert();
    inverseAxesArray_ = inverseAxes_.matrix();

    // Calculate reciprocal axes and volume
    // Reciprocal cell vectors are perpendicular to normal cell axes.
    // Calculate from cross products of normal cell vectors
    reciprocalAxes_.setColumn(0, axes_.columnAsVec3(1) * axes_.columnAsVec3(2));
    reciprocalAxes_.setColumn(1, axes_.columnAsVec3(2) * axes_.columnAsVec3(0));
    reciprocalAxes_.setColumn(2, axes_.columnAsVec3(0) * axes_.columnAsVec3(1));
    reciprocalAxes_.columnMultiply(0, 2.0 * M_PI / volume_);
    reciprocalAxes_.columnMultiply(1, 2.0 * M_PI / volume_);
    reciprocalAxes_.columnMultiply(2, 2.0 * M_PI / volume_);
    reciprocalVolume_ = (reciprocalAxes_.columnAsVec3(1) * reciprocalAxes_.columnAsVec3(2)).dp(reciprocalAxes_.columnAsVec3(0));
}

/*
 * Coordinate Conversion
 */

// Convert specified fractional coordinates to real-space coordinates
inline void Box::toReal(Vector3 &r) const
{
    switch (type_)
    {
        case BoxType::Cubic:
            r.x *= a_;
            r.y *= a_;
            r.z *= a_;
            break;
        case BoxType::Orthorhombic:
            r.x *= a_;
            r.y *= b_;
            r.z *= c_;
            break;
        case BoxType::MonoclinicAlpha:
            r.x *= axesArray_[0];
            r.y *= axesArray_[4];
            r.y += r.z * axesArray_[7];
            r.z *= axesArray_[8];
            break;
        case BoxType::MonoclinicBeta:
            r.x *= axesArray_[0];
            r.x += r.z * axesArray_[6];
            r.y *= axesArray_[4];
            r.z *= axesArray_[8];
            break;
        case BoxType::MonoclinicGamma:
            r.x *= axesArray_[0];
            r.x += r.y * axesArray_[3];
            r.y *= axesArray_[4];
            r.z *= axesArray_[8];
            break;
        case BoxType::Triclinic:
            r.x *= axesArray_[0];
            r.x += r.y * axesArray_[3] + r.z * axesArray_[6];
            r.y *= axesArray_[4];
            r.y += r.z * axesArray_[7];
            r.z *= axesArray_[8];
            break;
        case BoxType::None:
            break;
    }
}

// Return specified fractional coordinates converted to real-space coordinates
Vector3 Box::getReal(Vector3 r) const
{
    toReal(r);
    return r;
}

// Convert specified real-space coordinates to fractional coordinates
inline void Box::toFractional(Vector3 &r) const
{
    switch (type_)
    {
        case BoxType::Cubic:
            r.x *= ra_;
            r.y *= ra_;
            r.z *= ra_;
            break;
        case BoxType::Orthorhombic:
            r.x *= ra_;
            r.y *= rb_;
            r.z *= rc_;
            break;
        case BoxType::MonoclinicAlpha:
            r.x *= inverseAxesArray_[0];
            r.y *= inverseAxesArray_[4];
            r.y += r.z * inverseAxesArray_[7];
            r.z *= inverseAxesArray_[8];
            break;
        case BoxType::MonoclinicBeta:
            r.x *= inverseAxesArray_[0];
            r.x += r.z * inverseAxesArray_[6];
            r.y *= inverseAxesArray_[4];
            r.z *= inverseAxesArray_[8];
            break;
        case BoxType::MonoclinicGamma:
            r.x *= inverseAxesArray_[0];
            r.x += r.y * inverseAxesArray_[3];
            r.y *= inverseAxesArray_[4];
            r.z *= inverseAxesArray_[8];
            break;
        case BoxType::Triclinic:
            r.x *= inverseAxesArray_[0];
            r.x += r.y * inverseAxesArray_[3] + r.z * inverseAxesArray_[6];
            r.y *= inverseAxesArray_[4];
            r.y += r.z * inverseAxesArray_[7];
            r.z *= inverseAxesArray_[8];
            break;
        case BoxType::None:
            break;
    }
}

// Return specified real coordinates converted to fractional coordinates
Vector3 Box::getFractional(Vector3 r) const
{
    toFractional(r);
    return r;
}

/*
 * Geometry
 */

// Return angle (in degrees) between coordinates, accounting for minimum image
double Box::angleInDegrees(const Vector3 &i, const Vector3 &j, const Vector3 &k) const
{
    return minimumVector(j, i).angleInDegrees(minimumVector(j, k));
}

// Return angle (in radians) between coordinates, accounting for minimum image
double Box::angleInRadians(const Vector3 &i, const Vector3 &j, const Vector3 &k) const
{
    return minimumVector(j, i).angleInRadians(minimumVector(j, k));
}

// Return literal angle (in degrees) between coordinates, without applying minimum image convention
double Box::literalAngleInDegrees(const Vector3 &i, const Vector3 &j, const Vector3 &k)
{
    return (i - j).angleInDegrees(k - j);
}

// Return torsion (in degrees) between supplied coordinates, accounting for minimum image
double Box::torsionInDegrees(const Vector3 &i, const Vector3 &j, const Vector3 &k, const Vector3 &l) const
{
    return DissolveMath::toDegrees(torsionInRadians(i, j, k, l));
}

// Return torsion (in radians) between supplied coordinates, accounting for minimum image
double Box::torsionInRadians(const Vector3 &i, const Vector3 &j, const Vector3 &k, const Vector3 &l) const
{
    auto jk = minimumVector(j, k);
    auto xpj = jk * minimumVector(j, i);
    auto xpk = jk * minimumVector(k, l);
    xpj.magAndNormalise();
    xpk.magAndNormalise();
    return atan2(jk.dp(xpj * xpk) / jk.magnitude(), xpj.dp(xpk));
}

/*
 * Utility Routines
 */

// Generate a suitable Box given the supplied relative lengths, angles
Box Box::generate(Vector3 lengths, std::optional<Vector3> angles, bool nonPeriodic)
{
    if (!angles)
        angles = {90.0, 90.0, 90.0};
    return nonPeriodic ? Box(Box::BoxType::None, lengths, *angles) : Box::generate(lengths, *angles);
}

// Generate Boxes of a given type
Box Box::none() { return Box(Box::BoxType::None, Vector3{0, 0, 0}, Vector3{0.0, 0.0, 0.0}); }

Box Box::cubic(double length) { return Box(Box::BoxType::Cubic, Vector3{length, length, length}, Vector3{90.0, 90.0, 90.0}); }

Box Box::orthorhombic(const Vector3 &lengths) { return Box(Box::BoxType::Orthorhombic, lengths, Vector3{90.0, 90.0, 90.0}); }

Box Box::monoclinicAlpha(const Vector3 &lengths, double alpha)
{
    return Box(Box::BoxType::MonoclinicAlpha, lengths, Vector3{alpha, 90.0, 90.0});
}

Box Box::monoclinicBeta(const Vector3 &lengths, double beta)
{
    return Box(Box::BoxType::MonoclinicBeta, lengths, Vector3{90.0, beta, 90.0});
}

Box Box::monoclinicGamma(const Vector3 &lengths, double gamma)
{
    return Box(Box::BoxType::MonoclinicGamma, lengths, Vector3{90.0, 90.0, gamma});
}

Box Box::triclinic(const Vector3 &lengths, const Vector3 &angles) { return Box(Box::BoxType::Triclinic, lengths, angles); }

// Return radius of largest possible inscribed sphere for box
double Box::inscribedSphereRadius() const
{
    // Radius of largest inscribed sphere is the smallest of the three calculated values....
    double mag, diameter, result = 0.0;
    Vector3 cross;
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
Vector3 Box::randomCoordinate() const
{
    Vector3 r(DissolveMath::random(), DissolveMath::random(), DissolveMath::random());
    toReal(r);
    return r;
}

// Return folded coordinate (i.e. inside current Box)
Vector3 Box::fold(const Vector3 &r) const
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
Vector3 Box::foldFrac(const Vector3 &r) const
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

// Determine axis scale factors to give requested volume, with scaling ratios provided
Vector3 Box::scaleFactors(double requestedVolume, const std::array<bool, 3> &scalableAxes) const
{
    auto &[scaleX, scaleY, scaleZ] = scalableAxes;

    // Sanity check
    if (!scaleX && !scaleY && !scaleZ)
        throw(std::runtime_error("No axes specified as scalable, so no scaling factor can be calculated.\n"));

    // Determine root power
    auto rootPower = 1.0 / (scaleX + scaleY + scaleZ);

    auto ratio = pow(requestedVolume, rootPower) / pow(volume_, rootPower);

    return {scaleX ? ratio : 1.0, scaleY ? ratio : 1.0, scaleZ ? ratio : 1.0};
}

/*
 * Minimum Image Calculation
 */

// Wrap fractional coordinate into Box assuming it can be no more than half a fractional Box length away in any direction
void Box::wrap(Vector3 &rFrac) const
{
    if (type_ != BoxType::None)
    {
        if (rFrac.x < -0.5)
            rFrac.x += 1.0;
        else if (rFrac.x > 0.5)
            rFrac.x -= 1.0;
        if (rFrac.y < -0.5)
            rFrac.y += 1.0;
        else if (rFrac.y > 0.5)
            rFrac.y -= 1.0;
        if (rFrac.z < -0.5)
            rFrac.z += 1.0;
        else if (rFrac.z > 0.5)
            rFrac.z -= 1.0;
    }
}

// Return minimum image coordinates of r1 with respect to r2
Vector3 Box::minimumImage(const Vector3 &r1, const Vector3 &r2) const
{
    Vector3 v21 = r1 - r2;

    toFractional(v21);
    wrap(v21);
    toReal(v21);
    v21 += r2;

    return v21;
}

// Return minimum image vector from r1 to r2
Vector3 Box::minimumVector(const Vector3 &r1, const Vector3 &r2) const
{
    Vector3 v12 = r2 - r1;
    toFractional(v12);
    wrap(v12);
    toReal(v12);
    return v12;
}

// Return minimum image distance from r1 to r2
double Box::minimumDistance(const Vector3 &r1, const Vector3 &r2) const { return minimumVector(r1, r2).magnitude(); }

// Return minimum image squared distance from r1 to r2
double Box::minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const { return minimumVector(r1, r2).magnitudeSq(); }

/*
 * Serialisation
 */

// Express as a serialisable value
void Box::serialise(std::string tag, SerialisedValue &target) const
{
    auto &box = target[tag];
    box["lengths"] = {a_, b_, c_};
    box["angles"] = {alpha_, beta_, gamma_};
    box["nonPeriodic"] = {!std::get<0>(periodic_), !std::get<1>(periodic_), !std::get<2>(periodic_)};
}