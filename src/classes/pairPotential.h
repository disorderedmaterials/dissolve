// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include "math/data1D.h"
#include "math/function1D.h"
#include "math/interpolator.h"
#include <memory>

// Forward Declarations
class AtomType;

// PairPotential Definition
class PairPotential : Serialisable<>
{
    public:
    PairPotential(std::string_view nameI = {}, std::string_view nameJ = {});
    PairPotential(std::string_view nameI, std::string_view nameJ, const InteractionPotential<Functions1D> &potential);
    // Coulomb Truncation Scheme enum
    enum CoulombTruncationScheme
    {
        NoCoulombTruncation,     /* No truncation scheme */
        ShiftedCoulombTruncation /* Shifted and truncated */
    };
    // Return enum options for CoulombTruncationScheme
    static EnumOptions<PairPotential::CoulombTruncationScheme> coulombTruncationSchemes();
    // Short-Range Truncation Scheme enum
    enum ShortRangeTruncationScheme
    {
        NoShortRangeTruncation,     /* No truncation scheme */
        ShiftedShortRangeTruncation /* Shifted and truncated */
    };
    // Return enum options for ShortRangeTruncationScheme
    static EnumOptions<PairPotential::ShortRangeTruncationScheme> shortRangeTruncationSchemes();
    // Typedefs
    using Definition = std::tuple<std::shared_ptr<AtomType>, std::shared_ptr<AtomType>, std::unique_ptr<PairPotential>>;

    /*
     * Seed Interaction Type
     */
    private:
    // Truncation scheme to apply to short-range part of potential
    static ShortRangeTruncationScheme shortRangeTruncationScheme_;
    // Short-range energy at cutoff distance (used by truncation scheme)
    double shortRangeEnergyAtCutoff_{0.0};
    // Short-range force at cutoff distance (used by truncation scheme)
    double shortRangeForceAtCutoff_{0.0};
    // Whether atom type charges should be included in the generated potential
    bool includeAtomTypeCharges_{false};
    // Truncation scheme to apply to Coulomb part of potential
    static CoulombTruncationScheme coulombTruncationScheme_;

    public:
    // Set short-ranged truncation scheme
    static void setShortRangeTruncationScheme(ShortRangeTruncationScheme scheme);
    // Return short-ranged truncation scheme
    static ShortRangeTruncationScheme shortRangeTruncationScheme();
    // Return whether atom type charges should be included in the generated potential
    bool includeAtomTypeCharges() const;
    // Set Coulomb truncation scheme
    static void setCoulombTruncationScheme(CoulombTruncationScheme scheme);
    // Return Coulomb truncation scheme
    static CoulombTruncationScheme coulombTruncationScheme();

    /*
     * Source Parameters
     */
    private:
    // Names reflecting target atom types for potential
    std::string nameI_, nameJ_;
    // Interaction potential and function
    InteractionPotential<Functions1D> interactionPotential_;
    Function1DWrapper potentialFunction_;
    // Charge on I (taken from AtomType)
    double chargeI_{0.0};
    // Charge on J (taken from AtomType)
    double chargeJ_{0.0};

    private:
    // Set Data1D names from source AtomTypes
    void setData1DNames();

    public:
    // Set names reflecting target atom types for potential
    void setNames(std::string_view nameI, std::string_view nameJ);
    // Return name for first target atom type
    std::string_view nameI() const;
    // Return name for second target atom type
    std::string_view nameJ() const;
    // Set interaction potential
    bool setInteractionPotential(Functions1D::Form form, std::string_view parameters);
    bool setInteractionPotential(const InteractionPotential<Functions1D> &potential);
    // Set form of interaction potential
    void setInteractionPotentialForm(Functions1D::Form form);
    // Return interaction potential
    const InteractionPotential<Functions1D> &interactionPotential() const;
    // Set included charges
    void setIncludedCharges(double qi, double qj);
    // Set no included charges
    void setNoIncludedCharges();
    // Return charge I
    double chargeI() const;
    // Return charge J
    double chargeJ() const;

    /*
     * Tabulated Potential
     */
    private:
    // Number of points to tabulate
    int nPoints_{0};
    // Maximum distance of potential
    double range_{0.0};
    // Distance between points in tabulated potentials
    double delta_{-1.0}, rDelta_{0.0};
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
        double r, PairPotential::ShortRangeTruncationScheme truncation = PairPotential::shortRangeTruncationScheme()) const;
    // Return analytic short range force
    double analyticShortRangeForce(
        double r, PairPotential::ShortRangeTruncationScheme truncation = PairPotential::shortRangeTruncationScheme()) const;

    public:
    // Generate energy and force tables
    bool tabulate(double maxR, double delta);
    // Calculate full potential
    void calculateUFull();
    // Calculate derivative of potential
    void calculateDUFull();
    // Return number of tabulated points in potential
    int nPoints() const;
    // Return range of potential
    double range() const;
    // Return spacing between points
    double delta() const;
    // Calculate original potential (uOriginal) from current parameters
    void calculateUOriginal();
    // Return potential at specified r
    double energy(double r);
    // Return analytic potential at specified r, including Coulomb term from local atomtype charges
    double analyticEnergy(double r) const;
    // Return analytic potential at specified r, including Coulomb term from supplied charge product
    double analyticEnergy(double qiqj, double r, double elecScale = 1.0, double vdwScale = 1.0,
                          PairPotential::CoulombTruncationScheme truncation = PairPotential::coulombTruncationScheme()) const;
    // Return analytic coulomb potential energy of specified charge product
    double
    analyticCoulombEnergy(double qiqj, double r,
                          PairPotential::CoulombTruncationScheme truncation = PairPotential::coulombTruncationScheme()) const;
    // Return derivative of potential at specified r
    double force(double r);
    // Return analytic force at specified r, including Coulomb term from local atomtype charges
    double analyticForce(double r) const;
    // Return analytic force at specified r, including Coulomb term from supplied charge product
    double analyticForce(double qiqj, double r, double elecScale = 1.0, double vdwScale = 1.0,
                         PairPotential::CoulombTruncationScheme truncation = PairPotential::coulombTruncationScheme()) const;
    // Return analytic coulomb force of specified charge product
    double
    analyticCoulombForce(double qiqj, double r,
                         PairPotential::CoulombTruncationScheme truncation = PairPotential::coulombTruncationScheme()) const;
    // Return full tabulated potential (original plus additional)
    Data1D &uFull();
    const Data1D &uFull() const;
    // Return full tabulated derivative
    Data1D &dUFull();
    const Data1D &dUFull() const;
    // Return original potential
    Data1D &uOriginal();
    const Data1D &uOriginal() const;
    // Return additional potential
    Data1D &uAdditional();
    const Data1D &uAdditional() const;
    // Zero additional potential
    void resetUAdditional();
    // Set additional potential
    void setUAdditional(Data1D &newUAdditional);
    // Adjust additional potential, and recalculate UFull and dUFull
    void adjustUAdditional(const Data1D &deltaU, double factor = 1.0);

    /*
     * I/O
     */
    public:
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
};
