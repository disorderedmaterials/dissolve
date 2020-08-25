/*
    *** Interatomic Pair Potential
    *** src/classes/pairpotential.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "base/parameters.h"
#include "data/ff.h"
#include "math/data1d.h"
#include "math/interpolator.h"
#include "templates/list.h"
#include <memory>

// Forward Declarations
class AtomType;

// PairPotential Definition
class PairPotential : public ListItem<PairPotential>
{
    public:
    PairPotential();
    // Coulomb Truncation Scheme enum
    enum CoulombTruncationScheme
    {
        NoCoulombTruncation,      /* No truncation scheme */
        ShiftedCoulombTruncation, /* Shifted and truncated */
        nCoulombTruncationSchemes /* Number of Coulomb truncation schemes */
    };
    // Return enum options for CoulombTruncationScheme
    static EnumOptions<PairPotential::CoulombTruncationScheme> &coulombTruncationSchemes();
    // Short-Range Truncation Scheme enum
    enum ShortRangeTruncationScheme
    {
        NoShortRangeTruncation,      /* No truncation scheme */
        ShiftedShortRangeTruncation, /* Shifted and truncated */
        CosineShortRangeTruncation,  /* Cosine-multiplied truncation */
        nShortRangeTruncationSchemes /* Number of Short-Range truncation schemes */
    };
    // Return enum options for ShortRangeTruncationScheme
    static EnumOptions<PairPotential::ShortRangeTruncationScheme> &shortRangeTruncationSchemes();

    /*
     * Seed Interaction Type
     */
    private:
    // Truncation scheme to apply to short-range part of potential
    static ShortRangeTruncationScheme shortRangeTruncationScheme_;
    // Width of short-range potential over which to truncate (if scheme = Cosine)
    static double shortRangeTruncationWidth_;
    // Short-range energy at cutoff distance (used by truncation scheme)
    double shortRangeEnergyAtCutoff_;
    // Short-range force at cutoff distance (used by truncation scheme)
    double shortRangeForceAtCutoff_;
    // Whether Coulomb term should be included in the generated potential
    bool includeCoulomb_;
    // Truncation scheme to apply to Coulomb part of potential
    static CoulombTruncationScheme coulombTruncationScheme_;
    // Coulomb energy at cutoff distance (used by truncation scheme)
    double coulombEnergyAtCutoff_;
    // Coulomb force at cutoff distance (used by truncation scheme)
    double coulombForceAtCutoff_;

    public:
    // Set short-ranged truncation scheme
    static void setShortRangeTruncationScheme(ShortRangeTruncationScheme scheme);
    // Return short-ranged truncation scheme
    static ShortRangeTruncationScheme shortRangeTruncationScheme();
    // Set width of short-range potential over which to truncate (if scheme = Cosine)
    static void setShortRangeTruncationWidth(double width);
    // Return width of short-range potential over which to truncate (if scheme = Cosine)
    static double shortRangeTruncationWidth();
    // Set whether Coulomb term should be included in the generated potential
    void setIncludeCoulomb(bool b);
    // Return whether Coulomb term should be included in the generated potential
    bool includeCoulomb();
    // Set Coulomb truncation scheme
    static void setCoulombTruncationScheme(CoulombTruncationScheme scheme);
    // Return Coulomb truncation scheme
    static CoulombTruncationScheme coulombTruncationScheme();

    /*
     * Source Parameters
     */
    private:
    // Original source AtomTypes
    std::shared_ptr<AtomType> atomTypeI_, atomTypeJ_;
    // Parameters for short-range potential
    double parameters_[MAXSRPARAMETERS];
    // Short range type (determined from AtomTypes)
    Forcefield::ShortRangeType shortRangeType_;
    // Charge on I (taken from AtomType)
    double chargeI_;
    // Charge on J (taken from AtomType)
    double chargeJ_;

    private:
    // Set Data1D names from source AtomTypes
    void setData1DNames();

    public:
    // Set up PairPotential parameters from specified AtomTypes
    bool setUp(std::shared_ptr<AtomType> typeI, std::shared_ptr<AtomType> typeJ);
    // Return short-ranged type
    Forcefield::ShortRangeType shortRangeType() const;
    // Return first AtomType name
    std::string_view atomTypeNameI() const;
    // Return second AtomType name
    std::string_view atomTypeNameJ() const;
    // Return first source AtomType
    std::shared_ptr<AtomType> atomTypeI() const;
    // Return second source AtomType
    std::shared_ptr<AtomType> atomTypeJ() const;
    // Set parameter specified
    void setParameter(int index, double value);
    // Return short-range parameter specified
    double parameter(int index) const;
    // Set charge I
    void setChargeI(double value);
    // Return charge I
    double chargeI() const;
    // Set charge J
    void setChargeJ(double value);
    // Return charge J
    double chargeJ() const;

    /*
     * Tabulated Potential
     */
    private:
    // Number of points to tabulate
    int nPoints_;
    // Maximum distance of potential
    double range_;
    // Distance between points in tabulated potentials
    double delta_, rDelta_;
    // Tabulated original potential, calculated from AtomType parameters
    Data1D uOriginal_;
    // Additional potential, generated by some means
    Data1D uAdditional_;
    // Full potential (original plus additional), used in simulations
    Data1D uFull_;
    // Interpolation of full potential
    Interpolator uFullInterpolation_;
    // Tabulated derivative of full potential
    Data1D dUFull_;
    // Interpolation of derivative of full potential
    Interpolator dUFullInterpolation_;

    private:
    // Return analytic short range potential energy
    double analyticShortRangeEnergy(
        double r, Forcefield::ShortRangeType type,
        PairPotential::ShortRangeTruncationScheme truncation = PairPotential::shortRangeTruncationScheme());
    // Return analytic short range force
    double
    analyticShortRangeForce(double r, Forcefield::ShortRangeType type,
                            PairPotential::ShortRangeTruncationScheme truncation = PairPotential::shortRangeTruncationScheme());
    // Calculate full potential
    void calculateUFull();
    // Calculate derivative of potential
    void calculateDUFull();

    public:
    // Generate energy and force tables
    bool tabulate(double maxR, double delta, bool includeCoulomb);
    // Return number of tabulated points in potential
    int nPoints() const;
    // Return range of potential
    double range() const;
    // Return spacing between points
    double delta() const;
    // (Re)generate original potential (uOriginal) from current parameters
    void calculateUOriginal(bool recalculateUFull = true);
    // Return potential at specified r
    double energy(double r);
    // Return analytic potential at specified r, including Coulomb term from local atomtype charges
    double analyticEnergy(double r);
    // Return analytic potential at specified r, including Coulomb term from supplied charge product
    double analyticEnergy(double qiqj, double r,
                          PairPotential::CoulombTruncationScheme truncation = PairPotential::coulombTruncationScheme());
    // Return analytic coulomb potential energy of specified charge product
    double analyticCoulombEnergy(double qiqj, double r,
                                 PairPotential::CoulombTruncationScheme truncation = PairPotential::coulombTruncationScheme());
    // Return derivative of potential at specified r
    double force(double r);
    // Return analytic force at specified r, including Coulomb term from local atomtype charges
    double analyticForce(double r);
    // Return analytic force at specified r, including Coulomb term from supplied charge product
    double analyticForce(double qiqj, double r,
                         PairPotential::CoulombTruncationScheme truncation = PairPotential::coulombTruncationScheme());
    // Return analytic coulomb force of specified charge product
    double analyticCoulombForce(double qiqj, double r,
                                PairPotential::CoulombTruncationScheme truncation = PairPotential::coulombTruncationScheme());
    // Return full tabulated potential (original plus additional)
    Data1D &uFull();
    // Return full tabulated derivative
    Data1D &dUFull();
    // Return original potential
    Data1D &uOriginal();
    // Return additional potential
    Data1D &uAdditional();
    // Zero additional potential
    void resetUAdditional();
    // Set additional potential
    void setUAdditional(Data1D &newUAdditional);
    // Adjust additional potential, and recalculate UFull and dUFull
    void adjustUAdditional(Data1D deltaU, double factor = 1.0);
};
