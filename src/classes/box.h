// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "math/matrix3.h"
#include "math/vector3.h"

#include <map>
#include <vector>

// Forward Declarations
class Cell;
class Data1D;

// Basic Box Definition
class Box : public Serialisable<>
{
    public:
    // Box Type Enum
    enum class BoxType
    {
        NonPeriodic,     /* Non-periodic system - cubic box, but no minimum image calculation */
        Cubic,           /* Cubic box with A == B == C, alphe == beta == gamma == 90 */
        Orthorhombic,    /* Orthorhombic box with A != B != C, alphe == beta == gamma = 90 */
        MonoclinicAlpha, /* Monoclinic box with A != B != C, alpha != 90, and beta == gamma == 90 */
        MonoclinicBeta,  /* Monoclinic box with A != B != C, beta != 90, and alpha == gamma == 90 */
        MonoclinicGamma, /* Monoclinic box with A != B != C, gamma != 90, and alpha == beta == 90 */
        Triclinic        /* Triclinic box with A != B != C, alpha != beta != gamma != 90 */
    };
    // Return enum options for BoxType
    static EnumOptions<BoxType> boxTypes();
    Box(Box::BoxType boxType, const Vector3 lengths, const Vector3 angles);
    virtual ~Box() = default;
    Box &operator=(const Box &source) = default;

    /*
     * Basic Definition
     */
    protected:
    // Box type
    BoxType type_;
    // Box lengths
    double a_, b_, c_;
    // Reciprocal Box lengths
    double ra_, rb_, rc_;
    // Box angles
    double alpha_, beta_, gamma_;
    // Flags stating periodicity along x, y, and z
    std::array<bool, 3> periodic_;
    // Axes
    Matrix3 axes_;
    // Axes as simple array
    std::array<double, 9> axesArray_;
    // Inverse axes
    Matrix3 inverseAxes_;
    // Inverse axes as simple array
    std::array<double, 9> inverseAxesArray_;
    // Reciprocal axes
    Matrix3 reciprocalAxes_;
    // Volume
    double volume_;
    // Reciprocal volume
    double reciprocalVolume_;

    public:
    // Return Box type
    BoxType type() const;
    // Determine Box type
    static std::optional<BoxType> type(Vector3 lengths, Vector3 angles);
    // Return volume
    double volume() const;
    // Return axis lengths
    Vector3 axisLengths() const;
    // Return axis length specified
    double axisLength(int n) const;
    // Return axis angles
    Vector3 axisAngles() const;
    // Return axis angle specified
    double axisAngle(int n) const;
    // Return axis matrix
    const Matrix3 &axes() const;
    // Return inverse axes matrix
    const Matrix3 &inverseAxes() const;
    // Return reciprocal box volume
    double reciprocalVolume() const;
    // Return reciprocal axis lengths
    Vector3 reciprocalAxisLengths() const;
    // Return reciprocal axes matrix
    const Matrix3 &reciprocalAxes() const;
    // Scale Box lengths by specified factors
    void scale(Vector3 scaleFactors);

    /*
     * Coordinate Conversion
     */
    public:
    // Convert specified fractional coordinates to real-space coordinates
    inline virtual void toReal(Vector3 &r) const = 0;
    // Return specified fractional coordinates converted to real-space coordinates
    Vector3 getReal(Vector3 r) const;
    // Convert specified real-space coordinates to fractional coordinates
    inline virtual void toFractional(Vector3 &r) const = 0;
    // Return specified real coordinates converted to fractional coordinates
    Vector3 getFractional(Vector3 r) const;

    /*
     * Minimum Image Calculation
     */
    protected:
    // Wrap fractional coordinate into Box assuming it can be no more than half a fractional Box length away in any one
    // direction
    inline void wrap(Vector3 &rFrac) const
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

    public:
    // Return minimum image coordinates of r1 with respect to r2
    virtual Vector3 minimumImage(const Vector3 &r1, const Vector3 &r2) const = 0;
    // Return minimum image vector from r1 to r2
    virtual Vector3 minimumVector(const Vector3 &r1, const Vector3 &r2) const = 0;
    // Return minimum image distance from r1 to r2
    virtual double minimumDistance(const Vector3 &r1, const Vector3 &r2) const = 0;
    // Return minimum image squared distance from r1 to r2
    virtual double minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const = 0;

    /*
     * Geometry
     */
    public:
    // Return angle (in degrees) between coordinates, accounting for minimum image
    double angleInDegrees(const Vector3 &i, const Vector3 &j, const Vector3 &k) const;
    // Return angle (in radians) between coordinates, accounting for minimum image
    double angleInRadians(const Vector3 &i, const Vector3 &j, const Vector3 &k) const;
    // Return literal angle (in degrees) between coordinates, without applying minimum image convention
    static double literalAngleInDegrees(const Vector3 &i, const Vector3 &j, const Vector3 &k);
    // Return torsion (in degrees) between supplied coordinates, accounting for minimum image
    double torsionInDegrees(const Vector3 &i, const Vector3 &j, const Vector3 &k, const Vector3 &l) const;
    // Return torsion (in radians) between supplied coordinates, accounting for minimum image
    double torsionInRadians(const Vector3 &i, const Vector3 &j, const Vector3 &k, const Vector3 &l) const;

    /*
     * Utility Routines
     */
    public:
    // Generate a suitable Box given the supplied relative lengths, angles, and volume
    static std::unique_ptr<Box> generate(Vector3 lengths, Vector3 angles);
    // Return radius of largest possible inscribed sphere for box
    double inscribedSphereRadius() const;
    // Return random coordinate inside Box
    Vector3 randomCoordinate() const;
    // Return folded coordinate (i.e. inside current Box)
    Vector3 fold(const Vector3 &r) const;
    // Return folded fractional coordinate (i.e. inside current Box)
    Vector3 foldFrac(const Vector3 &r) const;
    // Determine axis scale factors to give requested volume, with scaling ratios provided
    Vector3 scaleFactors(double requestedVolume, const std::array<bool, 3> &scalableAxes = {true, true, true}) const;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    SerialisedValue serialise() const override;
};

// Single Image Box Definition
class SingleImageBox : public Box
{
    public:
    SingleImageBox();
    ~SingleImageBox() override = default;

    /*
     * Coordinate Conversion
     */
    public:
    // Convert specified fractional coordinates to real-space coordinates
    void toReal(Vector3 &r) const override;
    // Convert specified real-space coordinates to fractional coordinates
    void toFractional(Vector3 &r) const override;

    /*
     * Minimum Image Calculations
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vector3 minimumImage(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image vector from r1 to r2
    Vector3 minimumVector(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const override;
};

// Non-Periodic Box Definition
class NonPeriodicBox : public Box
{
    public:
    NonPeriodicBox(double length = 1.0);
    ~NonPeriodicBox() override = default;

    /*
     * Coordinate Conversion
     */
    public:
    // Convert specified fractional coordinates to real-space coordinates
    void toReal(Vector3 &r) const override;
    // Convert specified real-space coordinates to fractional coordinates
    void toFractional(Vector3 &r) const override;

    /*
     * Minimum Image Calculations
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vector3 minimumImage(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image vector from r1 to r2
    Vector3 minimumVector(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const override;
};

// Cubic Box Definition
class CubicBox : public Box
{
    public:
    CubicBox(double length);
    ~CubicBox() override = default;

    /*
     * Coordinate Conversion
     */
    public:
    // Convert specified fractional coordinates to real-space coordinates
    void toReal(Vector3 &r) const override;
    // Convert specified real-space coordinates to fractional coordinates
    void toFractional(Vector3 &r) const override;

    /*
     * Minimum Image Calculations
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vector3 minimumImage(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image vector from r1 to r2
    Vector3 minimumVector(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const override;

    /*
     * Utility Functions
     */
    public:
    // Return folded coordinate (i.e. inside current Box)
    //    Vector3 fold(const Vector3 &r) const override;
};

// Orthorhombic Box Definition
class OrthorhombicBox : public Box
{
    public:
    OrthorhombicBox(const Vector3 lengths);
    ~OrthorhombicBox() override = default;

    /*
     * Coordinate Conversion
     */
    public:
    // Convert specified fractional coordinates to real-space coordinates
    void toReal(Vector3 &r) const override;
    // Convert specified real-space coordinates to fractional coordinates
    void toFractional(Vector3 &r) const override;

    /*
     * Minimum Image Calculations
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vector3 minimumImage(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image vector from r1 to r2
    Vector3 minimumVector(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const override;
};

// MonoclinicAlpha Box Definition
class MonoclinicAlphaBox : public Box
{
    public:
    MonoclinicAlphaBox(const Vector3 lengths, double alpha);
    ~MonoclinicAlphaBox() override = default;

    /*
     * Coordinate Conversion
     */
    public:
    // Convert specified fractional coordinates to real-space coordinates
    void toReal(Vector3 &r) const override;
    // Convert specified real-space coordinates to fractional coordinates
    void toFractional(Vector3 &r) const override;

    /*
     * Minimum Image Calculations
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vector3 minimumImage(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image vector from r1 to r2
    Vector3 minimumVector(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const override;
};

// MonoclinicBeta Box Definition
class MonoclinicBetaBox : public Box
{
    public:
    MonoclinicBetaBox(const Vector3 lengths, double beta);
    ~MonoclinicBetaBox() override = default;

    /*
     * Coordinate Conversion
     */
    public:
    // Convert specified fractional coordinates to real-space coordinates
    void toReal(Vector3 &r) const override;
    // Convert specified real-space coordinates to fractional coordinates
    void toFractional(Vector3 &r) const override;

    /*
     * Minimum Image Calculations
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vector3 minimumImage(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image vector from r1 to r2
    Vector3 minimumVector(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const override;
};

// MonoclinicGamma Box Definition
class MonoclinicGammaBox : public Box
{
    public:
    MonoclinicGammaBox(const Vector3 lengths, double gamma);
    ~MonoclinicGammaBox() override = default;

    /*
     * Coordinate Conversion
     */
    public:
    // Convert specified fractional coordinates to real-space coordinates
    void toReal(Vector3 &r) const override;
    // Convert specified real-space coordinates to fractional coordinates
    void toFractional(Vector3 &r) const override;

    /*
     * Minimum Image Calculations
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vector3 minimumImage(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image vector from r1 to r2
    Vector3 minimumVector(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const override;
};

// Triclinic Box Definition
class TriclinicBox : public Box
{
    public:
    TriclinicBox(const Vector3 lengths, const Vector3 angles);
    ~TriclinicBox() override = default;

    /*
     * Coordinate Conversion
     */
    public:
    // Convert specified fractional coordinates to real-space coordinates
    void toReal(Vector3 &r) const override;
    // Convert specified real-space coordinates to fractional coordinates
    void toFractional(Vector3 &r) const override;

    /*
     * Minimum Image Calculations
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vector3 minimumImage(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image vector from r1 to r2
    Vector3 minimumVector(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vector3 &r1, const Vector3 &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vector3 &r1, const Vector3 &r2) const override;
};
