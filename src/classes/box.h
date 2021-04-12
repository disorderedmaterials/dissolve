// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "math/matrix3.h"

// Forward Declarations
class Atom;
class Cell;
class Data1D;
class ProcessPool;

// Basic Box Definition
class Box
{
    public:
    Box();
    // Virtual Destructor
    virtual ~Box();
    void operator=(const Box &source);

    /*
     * Basic Definition
     */
    public:
    // Box Type Enum
    enum BoxType
    {
        NonPeriodicBoxType,  /* Non-periodic system - cubic box, but no minimum image calculation */
        CubicBoxType,        /* Cubic box with equivalent cell lengths, and right-angles */
        OrthorhombicBoxType, /* Orthorhombic box with inequivalent cell lengths, and right-angles */
        MonoclinicBoxType,   /* Monoclinic box with cell angles a != 90, and b == c == 90 */
        TriclinicBoxType     /* Triclinic box with cell angles a != b != c != 90 */
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
    // Return minimum image coordinates of 'i' with respect to 'ref'
    virtual Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> ref) const = 0;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    virtual Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const Vec3<double> &ref) const = 0;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    virtual Vec3<double> minimumImage(const Vec3<double> &i, const Vec3<double> &ref) const = 0;
    // Return minimum image vector from 'i' to 'j'
    virtual Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const = 0;
    // Return minimum image vector from 'i' to 'j'
    virtual Vec3<double> minimumVector(const Atom &i, const Atom &j) const = 0;
    // Return minimum image vector from 'i' to 'j'
    virtual Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const Vec3<double> &j) const = 0;
    // Return minimum image vector from 'i' to 'j'
    virtual Vec3<double> minimumVector(const Vec3<double> &i, const Vec3<double> &j) const = 0;
    // Return minimum image distance from 'i' to 'j'
    virtual double minimumDistance(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const = 0;
    // Return minimum image distance from 'i' to 'j' (references)
    virtual double minimumDistance(const Atom &i, const Atom &j) const = 0;
    // Return minimum image distance from 'i' to 'j'
    virtual double minimumDistance(const std::shared_ptr<Atom> i, const Vec3<double> &j) const = 0;
    // Return minimum image distance from 'i' to 'j'
    virtual double minimumDistance(const Vec3<double> &i, const Vec3<double> &j) const = 0;
    // Return minimum image squared distance from 'i' to 'j' (pointers)
    virtual double minimumDistanceSquared(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const = 0;
    // Return minimum image squared distance from 'i' to 'j' (references)
    virtual double minimumDistanceSquared(const Atom &i, const Atom &j) const = 0;
    // Return minimum image squared distance from 'i' to 'j'
    virtual double minimumDistanceSquared(const std::shared_ptr<Atom> i, const Vec3<double> &j) const = 0;
    // Return minimum image squared distance from 'i' to 'j'
    virtual double minimumDistanceSquared(const Vec3<double> &i, const Vec3<double> &j) const = 0;

    /*
     * Utility Routines
     */
    public:
    // Generate a suitable Box given the supplied relative lengths, angles, and volume
    static Box *generate(Vec3<double> lengths, Vec3<double> angles);
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
    virtual Vec3<double> fold(const Vec3<double> &i) const = 0;
    // Return folded fractional coordinate (i.e. inside current Box)
    virtual Vec3<double> foldFrac(const Vec3<double> &i) const = 0;
    // Convert supplied fractional coordinates to real space
    virtual Vec3<double> fracToReal(const Vec3<double> &r) const = 0;

    /*
     * Utility Routines
     */
    public:
    // Return angle (in degrees, no MIM) between Atoms
    double angleInDegrees(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, const std::shared_ptr<Atom> k) const;
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
    NonPeriodicBox(double length);
    ~NonPeriodicBox();

    /*
     * Minimum Image Routines (Virtual Implementations)
     */
    public:
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> ref) const;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const Vec3<double> &ref) const;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const Vec3<double> &i, const Vec3<double> &ref) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const Atom &i, const Atom &j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const Vec3<double> &i, const Vec3<double> &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image distance from 'i' to 'j' (references)
    double minimumDistance(const Atom &i, const Atom &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const Vec3<double> &i, const Vec3<double> &j) const;
    // Return minimum image squared distance from 'i' to 'j' (pointers)
    double minimumDistanceSquared(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image squared distance from 'i' to 'j' (references)
    double minimumDistanceSquared(const Atom &i, const Atom &j) const;
    // Return minimum image squared distance from 'i' to 'j'
    double minimumDistanceSquared(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image squared distance from 'i' to 'j'
    double minimumDistanceSquared(const Vec3<double> &i, const Vec3<double> &j) const;

    /*
     * Utility Routines (Virtual Implementations)
     */
    public:
    // Return random coordinate inside Box
    Vec3<double> randomCoordinate() const;
    // Return folded coordinate (i.e. inside current Box)
    Vec3<double> fold(const Vec3<double> &i) const;
    // Return folded fractional coordinate (i.e. inside current Box)
    Vec3<double> foldFrac(const Vec3<double> &i) const;
    // Convert supplied fractional coordinates to real space
    Vec3<double> fracToReal(const Vec3<double> &r) const;
};

// Cubic Box Definition
class CubicBox : public Box
{
    public:
    CubicBox(double length);
    ~CubicBox();

    /*
     * Minimum Image Routines (Virtual Implementations)
     */
    public:
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> ref) const;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const Vec3<double> &ref) const;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const Vec3<double> &i, const Vec3<double> &ref) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const Atom &i, const Atom &j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const Vec3<double> &i, const Vec3<double> &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image distance from 'i' to 'j' (references)
    double minimumDistance(const Atom &i, const Atom &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const Vec3<double> &i, const Vec3<double> &j) const;
    // Return minimum image squared distance from 'i' to 'j' (pointers)
    double minimumDistanceSquared(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image squared distance from 'i' to 'j' (references)
    double minimumDistanceSquared(const Atom &i, const Atom &j) const;
    // Return minimum image squared distance from 'i' to 'j'
    double minimumDistanceSquared(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image squared distance from 'i' to 'j'
    double minimumDistanceSquared(const Vec3<double> &i, const Vec3<double> &j) const;

    /*
     * Utility Routines (Virtual Implementations)
     */
    public:
    // Return random coordinate inside Box
    Vec3<double> randomCoordinate() const;
    // Return folded coordinate (i.e. inside current Box)
    Vec3<double> fold(const Vec3<double> &i) const;
    // Return folded fractional coordinate (i.e. inside current Box)
    Vec3<double> foldFrac(const Vec3<double> &i) const;
    // Convert supplied fractional coordinates to real space
    Vec3<double> fracToReal(const Vec3<double> &r) const;
};

// Orthorhombic Box Definition
class OrthorhombicBox : public Box
{
    public:
    OrthorhombicBox(const Vec3<double> lengths);
    ~OrthorhombicBox();

    /*
     * Minimum Image Routines (Virtual Implementations)
     */
    public:
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> ref) const;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const Vec3<double> &ref) const;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const Vec3<double> &i, const Vec3<double> &ref) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const Atom &i, const Atom &j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const Vec3<double> &i, const Vec3<double> &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image distance from 'i' to 'j' (references)
    double minimumDistance(const Atom &i, const Atom &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const Vec3<double> &i, const Vec3<double> &j) const;
    // Return minimum image squared distance from 'i' to 'j' (pointers)
    double minimumDistanceSquared(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image squared distance from 'i' to 'j' (references)
    double minimumDistanceSquared(const Atom &i, const Atom &j) const;
    // Return minimum image squared distance from 'i' to 'j'
    double minimumDistanceSquared(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image squared distance from 'i' to 'j'
    double minimumDistanceSquared(const Vec3<double> &i, const Vec3<double> &j) const;

    /*
     * Utility Routines (Virtual Implementations)
     */
    public:
    // Return random coordinate inside Box
    Vec3<double> randomCoordinate() const;
    // Return folded coordinate (i.e. inside current Box)
    Vec3<double> fold(const Vec3<double> &i) const;
    // Return folded fractional coordinate (i.e. inside current Box)
    Vec3<double> foldFrac(const Vec3<double> &i) const;
    // Convert supplied fractional coordinates to real space
    Vec3<double> fracToReal(const Vec3<double> &r) const;
};

// Monoclinic Box Definition
class MonoclinicBox : public Box
{
    public:
    MonoclinicBox(const Vec3<double> lengths, double beta);
    ~MonoclinicBox();

    /*
     * Minimum Image Routines (Virtual Implementations)
     */
    public:
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> ref) const;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const Vec3<double> &ref) const;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const Vec3<double> &i, const Vec3<double> &ref) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const Atom &i, const Atom &j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const Vec3<double> &i, const Vec3<double> &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image distance from 'i' to 'j' (references)
    double minimumDistance(const Atom &i, const Atom &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const Vec3<double> &i, const Vec3<double> &j) const;
    // Return minimum image squared distance from 'i' to 'j' (pointers)
    double minimumDistanceSquared(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image squared distance from 'i' to 'j' (references)
    double minimumDistanceSquared(const Atom &i, const Atom &j) const;
    // Return minimum image squared distance from 'i' to 'j'
    double minimumDistanceSquared(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image squared distance from 'i' to 'j'
    double minimumDistanceSquared(const Vec3<double> &i, const Vec3<double> &j) const;

    /*
     * Utility Routines (Virtual Implementations)
     */
    public:
    // Return random coordinate inside Box
    Vec3<double> randomCoordinate() const;
    // Return folded coordinate (i.e. inside current Box)
    Vec3<double> fold(const Vec3<double> &i) const;
    // Return folded fractional coordinate (i.e. inside current Box)
    Vec3<double> foldFrac(const Vec3<double> &i) const;
    // Convert supplied fractional coordinates to real space
    Vec3<double> fracToReal(const Vec3<double> &r) const;
};

// Triclinic Box Definition
class TriclinicBox : public Box
{
    public:
    TriclinicBox(const Vec3<double> lengths, const Vec3<double> angles);
    ~TriclinicBox();

    /*
     * Minimum Image Routines (Virtual Implementations)
     */
    public:
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> ref) const;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const std::shared_ptr<Atom> i, const Vec3<double> &ref) const;
    // Return minimum image coordinates of 'i' with respect to 'ref'
    Vec3<double> minimumImage(const Vec3<double> &i, const Vec3<double> &ref) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const Atom &i, const Atom &j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image vector from 'i' to 'j'
    Vec3<double> minimumVector(const Vec3<double> &i, const Vec3<double> &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image distance from 'i' to 'j' (references)
    double minimumDistance(const Atom &i, const Atom &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image distance from 'i' to 'j'
    double minimumDistance(const Vec3<double> &i, const Vec3<double> &j) const;
    // Return minimum image squared distance from 'i' to 'j' (pointers)
    double minimumDistanceSquared(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j) const;
    // Return minimum image squared distance from 'i' to 'j' (references)
    double minimumDistanceSquared(const Atom &i, const Atom &j) const;
    // Return minimum image squared distance from 'i' to 'j'
    double minimumDistanceSquared(const std::shared_ptr<Atom> i, const Vec3<double> &j) const;
    // Return minimum image squared distance from 'i' to 'j'
    double minimumDistanceSquared(const Vec3<double> &i, const Vec3<double> &j) const;

    /*
     * Utility Routines (Virtual Implementations)
     */
    public:
    // Return random coordinate inside Box
    Vec3<double> randomCoordinate() const;
    // Return folded coordinate (i.e. inside current Box)
    Vec3<double> fold(const Vec3<double> &i) const;
    // Return folded fractional coordinate (i.e. inside current Box)
    Vec3<double> foldFrac(const Vec3<double> &i) const;
    // Convert supplied fractional coordinates to real space
    Vec3<double> fracToReal(const Vec3<double> &r) const;
};
