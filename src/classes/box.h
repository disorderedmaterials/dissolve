// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "math/matrix3.h"

// Forward Declarations
class Cell;
class Data1D;
class ProcessPool;

// Basic Box Definition
class Box
{
    public:
    Box();
    virtual ~Box() = default;
    Box &operator=(const Box &source) = default;

    /*
     * Basic Definition
     */
    public:
    // Box Type Enum
    enum class BoxType
    {
        NonPeriodic,  /* Non-periodic system - cubic box, but no minimum image calculation */
        Cubic,        /* Cubic box with A == B == C, alphe == beta == gamma == 90 */
        Orthorhombic, /* Orthorhombic box with A != B != C, alphe == beta == gamma = 90 */
        Monoclinic,   /* Monoclinic box with A != B != C, alpha != 90, and beta == gamma == 90 */
        Triclinic     /* Triclinic box with A != B != C, alpha != beta != gamma != 90 */
    };
    // Return enum options for BoxType
    static EnumOptions<BoxType> boxTypes();

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
    Vec3<bool> periodic_;
    // Axes
    Matrix3 axes_;
    // Inverse axes
    Matrix3 inverseAxes_;
    // Reciprocal axes
    Matrix3 reciprocalAxes_;
    // Volume
    double volume_;
    // Reciprocal volume
    double reciprocalVolume_;

    public:
    // Finalise Box, storing volume and reciprocal and inverted axes
    void finalise();
    // Return Box type
    BoxType type() const;
    // Return volume
    double volume() const;
    // Return axis lengths
    Vec3<double> axisLengths() const;
    // Return axis length specified
    double axisLength(int n) const;
    // Return axis angles
    Vec3<double> axisAngles() const;
    // Return axis angle specified
    double axisAngle(int n) const;
    // Return axis matrix
    const Matrix3 &axes() const;
    // Return inverse axes matrix
    const Matrix3 &inverseAxes() const;
    // Return reciprocal box volume
    double reciprocalVolume() const;
    // Return reciprocal axis lengths
    Vec3<double> reciprocalAxisLengths() const;
    // Return reciprocal axes matrix
    const Matrix3 &reciprocalAxes() const;
    // Scale Box by specified factor
    void scale(double factor);

    /*
     * Minimum Image Routines (Pure Virtual)
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    virtual Vec3<double> minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const = 0;
    // Return minimum image vector from r1 to r2
    virtual Vec3<double> minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const = 0;
    // Return minimum image distance from r1 to r2
    virtual double minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const = 0;
    // Return minimum image squared distance from r1 to r2
    virtual double minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const = 0;

    /*
     * Utility Routines
     */
    public:
    // Generate a suitable Box given the supplied relative lengths, angles, and volume
    static std::unique_ptr<Box> generate(Vec3<double> lengths, Vec3<double> angles);
    // Return radius of largest possible inscribed sphere for box
    double inscribedSphereRadius() const;
    // Calculate the RDF normalisation for the Box
    bool calculateRDFNormalisation(ProcessPool &procPool, Data1D &boxNorm, double rdfRange, double rdfBinWidth,
                                   int nPoints) const;

    /*
     * Utility Routines (Pure Virtual)
     */
    public:
    // Return random coordinate inside Box
    virtual Vec3<double> randomCoordinate() const = 0;
    // Return folded coordinate (i.e. inside current Box)
    virtual Vec3<double> fold(const Vec3<double> &r) const = 0;
    // Return folded fractional coordinate (i.e. inside current Box)
    virtual Vec3<double> foldFrac(const Vec3<double> &r) const = 0;
    // Convert supplied fractional coordinates to real space
    virtual Vec3<double> fracToReal(const Vec3<double> &r) const = 0;

    /*
     * Utility Routines
     */
    public:
    // Return angle (in degrees) between coordinates
    double angleInDegrees(const Vec3<double> &i, const Vec3<double> &j, const Vec3<double> &k) const;
    // Return angle (in degrees) between supplied normalised vectors
    static double angleInDegrees(const Vec3<double> &normji, const Vec3<double> &normjk);
    // Return angle (in degrees) between supplied normalised vectors (storing dot product)
    static double angleInDegrees(const Vec3<double> &normji, const Vec3<double> &normjk, double &dotProduct);
    // Return literal angle (in degrees) between coordinates, without applying minimum image convention
    static double literalAngleInDegrees(const Vec3<double> &i, const Vec3<double> &j, const Vec3<double> &k);
    // Return torsion (in degrees) between supplied unnormalised vectors
    static double torsionInDegrees(const Vec3<double> &vecji, const Vec3<double> &vecjk, const Vec3<double> &veckl);
    // Return torsion (in degrees) between supplied unnormalised vectors, storing cross products and magnitude in supplied
    // variables
    static double torsionInDegrees(const Vec3<double> &vecji, const Vec3<double> &vecjk, const Vec3<double> &veckl,
                                   Vec3<double> &xpj, double &magxpj, Vec3<double> &xpk, double &magxpk);
    // Return torsion (in radians) between supplied unnormalised vectors
    static double torsionInRadians(const Vec3<double> &vecji, const Vec3<double> &vecjk, const Vec3<double> &veckl);
    // Return torsion (in radians) between supplied unnormalised vectors, storing cross products and magnitude in supplied
    // variables
    static double torsionInRadians(const Vec3<double> &vecji, const Vec3<double> &vecjk, const Vec3<double> &veckl,
                                   Vec3<double> &xpj, double &magxpj, Vec3<double> &xpk, double &magxpk);
};

// Non-Periodic Box Definition
class NonPeriodicBox : public Box
{
    public:
    NonPeriodicBox(double length = 1.0);
    ~NonPeriodicBox() override = default;

    /*
     * Minimum Image Routines (Virtual Implementations)
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vec3<double> minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image vector from r1 to r2
    Vec3<double> minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const override;

    /*
     * Utility Routines (Virtual Implementations)
     */
    public:
    // Return random coordinate inside Box
    Vec3<double> randomCoordinate() const override;
    // Return folded coordinate (i.e. inside current Box)
    Vec3<double> fold(const Vec3<double> &r) const override;
    // Return folded fractional coordinate (i.e. inside current Box)
    Vec3<double> foldFrac(const Vec3<double> &r) const override;
    // Convert supplied fractional coordinates to real space
    Vec3<double> fracToReal(const Vec3<double> &r) const override;
};

// Cubic Box Definition
class CubicBox : public Box
{
    public:
    CubicBox(double length);
    ~CubicBox() override = default;

    /*
     * Minimum Image Routines (Virtual Implementations)
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vec3<double> minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image vector from r1 to r2
    Vec3<double> minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const override;

    /*
     * Utility Routines (Virtual Implementations)
     */
    public:
    // Return random coordinate inside Box
    Vec3<double> randomCoordinate() const override;
    // Return folded coordinate (i.e. inside current Box)
    Vec3<double> fold(const Vec3<double> &r) const override;
    // Return folded fractional coordinate (i.e. inside current Box)
    Vec3<double> foldFrac(const Vec3<double> &r) const override;
    // Convert supplied fractional coordinates to real space
    Vec3<double> fracToReal(const Vec3<double> &r) const override;
};

// Orthorhombic Box Definition
class OrthorhombicBox : public Box
{
    public:
    OrthorhombicBox(const Vec3<double> lengths);
    ~OrthorhombicBox() override = default;

    /*
     * Minimum Image Routines (Virtual Implementations)
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vec3<double> minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image vector from r1 to r2
    Vec3<double> minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const override;

    /*
     * Utility Routines (Virtual Implementations)
     */
    public:
    // Return random coordinate inside Box
    Vec3<double> randomCoordinate() const override;
    // Return folded coordinate (i.e. inside current Box)
    Vec3<double> fold(const Vec3<double> &r) const override;
    // Return folded fractional coordinate (i.e. inside current Box)
    Vec3<double> foldFrac(const Vec3<double> &r) const override;
    // Convert supplied fractional coordinates to real space
    Vec3<double> fracToReal(const Vec3<double> &r) const override;
};

// Monoclinic Box Definition
class MonoclinicBox : public Box
{
    public:
    MonoclinicBox(const Vec3<double> lengths, double beta);
    ~MonoclinicBox() override = default;

    /*
     * Minimum Image Routines (Virtual Implementations)
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vec3<double> minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image vector from r1 to r2
    Vec3<double> minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const override;

    /*
     * Utility Routines (Virtual Implementations)
     */
    public:
    // Return random coordinate inside Box
    Vec3<double> randomCoordinate() const override;
    // Return folded coordinate (i.e. inside current Box)
    Vec3<double> fold(const Vec3<double> &r) const override;
    // Return folded fractional coordinate (i.e. inside current Box)
    Vec3<double> foldFrac(const Vec3<double> &r) const override;
    // Convert supplied fractional coordinates to real space
    Vec3<double> fracToReal(const Vec3<double> &r) const override;
};

// Triclinic Box Definition
class TriclinicBox : public Box
{
    public:
    TriclinicBox(const Vec3<double> lengths, const Vec3<double> angles);
    ~TriclinicBox() override = default;

    /*
     * Minimum Image Routines (Virtual Implementations)
     */
    public:
    // Return minimum image coordinates of r1 with respect to r2
    Vec3<double> minimumImage(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image vector from r1 to r2
    Vec3<double> minimumVector(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image distance from r1 to r2
    double minimumDistance(const Vec3<double> &r1, const Vec3<double> &r2) const override;
    // Return minimum image squared distance from r1 to r2
    double minimumDistanceSquared(const Vec3<double> &r1, const Vec3<double> &r2) const override;

    /*
     * Utility Routines (Virtual Implementations)
     */
    public:
    // Return random coordinate inside Box
    Vec3<double> randomCoordinate() const override;
    // Return folded coordinate (i.e. inside current Box)
    Vec3<double> fold(const Vec3<double> &r) const override;
    // Return folded fractional coordinate (i.e. inside current Box)
    Vec3<double> foldFrac(const Vec3<double> &r) const override;
    // Convert supplied fractional coordinates to real space
    Vec3<double> fracToReal(const Vec3<double> &r) const override;
};
