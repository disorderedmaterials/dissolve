// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/pairPotential.h"
#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "math/constants.h"
#include <cmath>

// Static members
PairPotential::CoulombTruncationScheme PairPotential::coulombTruncationScheme_ = PairPotential::ShiftedCoulombTruncation;
PairPotential::ShortRangeTruncationScheme PairPotential::shortRangeTruncationScheme_ =
    PairPotential::ShiftedShortRangeTruncation;

PairPotential::PairPotential()
    : interactionPotential_(Functions1D::Form::None), uFullInterpolation_(uFull_), dUFullInterpolation_(dUFull_)
{
}

PairPotential::PairPotential(const std::shared_ptr<AtomType> &typeI, const std::shared_ptr<AtomType> &typeJ,
                             bool includeCharges)
    : interactionPotential_(Functions1D::Form::None), uFullInterpolation_(uFull_), dUFullInterpolation_(dUFull_)
{
    setUp(typeI, typeJ, includeCharges);
}

PairPotential::PairPotential(std::string_view nameI, std::string_view nameJ, const InteractionPotential<Functions1D> &potential)
    : includeAtomTypeCharges_(false), nameI_(nameI), nameJ_(nameJ), interactionPotential_(potential),
      uFullInterpolation_(uFull_), dUFullInterpolation_(dUFull_)
{
    potentialFunction_.setFormAndParameters(interactionPotential_.form(), interactionPotential_.parameters());
}

// Return enum option info for CoulombTruncationScheme
EnumOptions<PairPotential::CoulombTruncationScheme> PairPotential::coulombTruncationSchemes()
{
    return EnumOptions<PairPotential::CoulombTruncationScheme>(
        "CoulombTruncationScheme",
        {{PairPotential::NoCoulombTruncation, "None"}, {PairPotential::ShiftedCoulombTruncation, "Shifted"}});
}

// Return enum option info for ShortRangeTruncationScheme
EnumOptions<PairPotential::ShortRangeTruncationScheme> PairPotential::shortRangeTruncationSchemes()
{
    return EnumOptions<PairPotential::ShortRangeTruncationScheme>(
        "ShortRangeTruncationScheme",
        {{PairPotential::NoShortRangeTruncation, "None"}, {PairPotential::ShiftedShortRangeTruncation, "Shifted"}});
}

/*
 * Seed Interaction Type
 */

// Set short-ranged truncation scheme
void PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme scheme)
{
    shortRangeTruncationScheme_ = scheme;
}

// Return short-ranged truncation scheme
PairPotential::ShortRangeTruncationScheme PairPotential::shortRangeTruncationScheme() { return shortRangeTruncationScheme_; }

// Set whether atom type charges should be included in the generated potential
void PairPotential::setIncludeAtomTypeCharges(bool b) { includeAtomTypeCharges_ = b; }

// Return whether atom type charges should be included in the generated potential
bool PairPotential::includeAtomTypeCharges() const { return includeAtomTypeCharges_; }

// Set Coulomb truncation scheme
void PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme scheme)
{
    coulombTruncationScheme_ = scheme;
}

// Return Coulomb truncation scheme
PairPotential::CoulombTruncationScheme PairPotential::coulombTruncationScheme() { return coulombTruncationScheme_; }

/*
 * Source Parameters
 */

// Set up PairPotential parameters from specified AtomTypes
bool PairPotential::setUp(const std::shared_ptr<AtomType> &typeI, const std::shared_ptr<AtomType> &typeJ, bool includeCharges)
{
    // Check for NULL pointers
    if (typeI == nullptr)
        throw(std::runtime_error("Invalid AtomType pointer (typeI) given to PairPotential::setUp().\n"));
    if (typeJ == nullptr)
        throw(std::runtime_error("Invalid AtomType pointer (typeJ) given to PairPotential::setUp().\n"));

    includeAtomTypeCharges_ = includeCharges;
    interactionPotential_.setFormAndParameters(Functions1D::Form::None, "");

    nameI_ = typeI->name();
    nameJ_ = typeJ->name();
    setData1DNames();

    // Sanity check - do both atom types have valid short range functions set?
    if (!typeI->interactionPotential().hasValidForm())
        return Messenger::error("Can't set parameters for PairPotential since atom type {} has no valid short range form.\n",
                                typeI->name());
    if (!typeJ->interactionPotential().hasValidForm())
        return Messenger::error("Can't set parameters for PairPotential since atom type {} has no valid short range form.\n",
                                typeJ->name());

    // Combine the atom type parameters into potential function parameters
    auto optPotential = ShortRangeFunctions::combine(typeI->interactionPotential(), typeJ->interactionPotential());
    if (!optPotential)
        return false;
    interactionPotential_ = *optPotential;
    potentialFunction_.setFormAndParameters(interactionPotential_.form(), interactionPotential_.parameters());

    // Set charges
    chargeI_ = includeAtomTypeCharges_ ? typeI->charge() : 0.0;
    chargeJ_ = includeAtomTypeCharges_ ? typeJ->charge() : 0.0;

    return true;
}

// Set Data1D names from source AtomTypes
void PairPotential::setData1DNames()
{
    uFull_.setTag(fmt::format("{}-{}", nameI_, nameJ_));

    uAdditional_.setTag(fmt::format("{}-{} (Add)", nameI_, nameJ_));

    uOriginal_.setTag(fmt::format("{}-{} (Orig)", nameI_, nameJ_));

    dUFull_.setTag(fmt::format("{}-{} (dU/dr)", nameI_, nameJ_));
}

// Return name for first source parameters
std::string_view PairPotential::nameI() const { return nameI_; }

// Return name for second source parameters
std::string_view PairPotential::nameJ() const { return nameJ_; };

// Set interaction potential
bool PairPotential::setInteractionPotential(Functions1D::Form form, std::string_view parameters)
{
    return interactionPotential_.setFormAndParameters(form, parameters) &&
           potentialFunction_.setFormAndParameters(form, interactionPotential_.parameters());
}

// Return interaction potential
const InteractionPotential<Functions1D> &PairPotential::interactionPotential() const { return interactionPotential_; }

// Set charge I
void PairPotential::setChargeI(double value) { chargeI_ = value; }

// Return charge I
double PairPotential::chargeI() const { return chargeI_; }

// Set charge J
void PairPotential::setChargeJ(double value) { chargeJ_ = value; }

// Return charge J
double PairPotential::chargeJ() const { return chargeJ_; }

/*
 * Tabulated PairPotential
 */

// Return analytic short range potential energy
double PairPotential::analyticShortRangeEnergy(double r, PairPotential::ShortRangeTruncationScheme truncation) const
{
    // Assess stored potential function at specified r
    auto energy = potentialFunction_.y(r);

    // Apply the selected truncation scheme
    if (truncation == PairPotential::ShiftedShortRangeTruncation)
    {
        energy += (r - range_) * shortRangeForceAtCutoff_ - shortRangeEnergyAtCutoff_;
    }

    return energy;
}

// Return analytic short range force
double PairPotential::analyticShortRangeForce(double r, PairPotential::ShortRangeTruncationScheme truncation) const
{
    // Assess stored potential function derivative at specified r and negate to get force
    auto force = -potentialFunction_.dYdX(r);

    // Apply the selected truncation scheme
    if (truncation == PairPotential::ShiftedShortRangeTruncation)
    {
        force -= shortRangeForceAtCutoff_;
    }

    return force;
}

// Calculate full potential
void PairPotential::calculateUFull()
{
    // Copy uOriginal_ into uFull_...
    uFull_ = uOriginal_;

    // ...add on uAdditional...
    uFull_ += uAdditional_;

    // ...and update its interpolation
    uFullInterpolation_.interpolate(Interpolator::ThreePointInterpolation);
}

// Calculate derivative of potential
void PairPotential::calculateDUFull()
{
    dUFull_.initialise(uFull_);

    if (nPoints_ < 2)
        return;

    double fprime;
    for (auto n = 1; n < nPoints_ - 1; ++n)
    {
        /* Calculate numerical derivative with five-point stencil if possible. Otherwise use three-point stencil.
         * Assumes data are regularly-spaced (they should be, with gap of delta_)
         *            -f(x+2) + 8 f(x+1) - 8 f(x-1) + f(x-2)
         *    f'(x) = --------------------------------------
         *                           12 h
         */
        if ((n == 1) || (n == (nPoints_ - 2)))
        {
            // Three-point
            dUFull_.value(n) = -(uFull_.value(n - 1) - uFull_.value(n + 1)) / (2 * delta_);
        }
        else
        {
            // Five-point stencil
            fprime = -uFull_.value(n + 2) + 8 * uFull_.value(n + 1) - 8 * uFull_.value(n - 1) + uFull_.value(n - 2);
            fprime /= 12 * delta_;
            dUFull_.value(n) = fprime;
        }
    }

    // Set first and last points
    dUFull_.value(0) = 10.0 * dUFull_.value(1);
    dUFull_.value(nPoints_ - 1) = dUFull_.value(nPoints_ - 2) + (dUFull_.value(nPoints_ - 2) - dUFull_.value(nPoints_ - 3));

    // Update interpolation
    dUFullInterpolation_.interpolate(Interpolator::ThreePointInterpolation);
}

// Generate energy and force tables
bool PairPotential::tabulate(double maxR, double delta)
{
    // Determine nPoints_
    delta_ = delta;
    rDelta_ = 1.0 / delta_;
    range_ = maxR;
    nPoints_ = range_ / delta_;

    // Calculate energies and forces at the cutoff distance, for later use in truncation schemes
    shortRangeEnergyAtCutoff_ = analyticShortRangeEnergy(range_, PairPotential::NoShortRangeTruncation);
    shortRangeForceAtCutoff_ = analyticShortRangeForce(range_, PairPotential::NoShortRangeTruncation);
    coulombEnergyAtCutoff_ = analyticCoulombEnergy(chargeI_ * chargeJ_, range_, PairPotential::NoCoulombTruncation);
    coulombForceAtCutoff_ = analyticCoulombForce(chargeI_ * chargeJ_, range_, PairPotential::NoCoulombTruncation);

    // Initialise original and additional potential arrays, and calculate original potential
    uOriginal_.initialise(nPoints_);
    calculateUOriginal();

    // Set additional potential to zero and update full potential
    uAdditional_ = uOriginal_;
    std::fill(uAdditional_.values().begin(), uAdditional_.values().end(), 0);
    calculateUFull();

    // Generate derivative data
    dUFull_.initialise(nPoints_);
    calculateDUFull();

    return true;
}

// Return number of tabulated points in potential
int PairPotential::nPoints() const { return nPoints_; }

// Return range of potential
double PairPotential::range() const { return range_; }

// Return spacing between points
double PairPotential::delta() const { return delta_; }

// Calculate original potential from current parameters
void PairPotential::calculateUOriginal()
{
    // Loop over points
    for (auto n = 1; n < nPoints_; ++n)
    {
        auto r = n * delta_;
        uOriginal_.xAxis(n) = r;

        // Set short-range potential contribution
        uOriginal_.value(n) = analyticShortRangeEnergy(r);

        // -- Add Coulomb contribution
        if (includeAtomTypeCharges_)
            uOriginal_.value(n) += analyticCoulombEnergy(chargeI_ * chargeJ_, r);
    }

    // Since the first point (at zero) risks being a nan, set it to ten times the second point instead
    uOriginal_.value(0) = 10.0 * uOriginal_.value(1);
}

// Return potential at specified r
double PairPotential::energy(double r)
{
    assert(r >= 0);

    return uFullInterpolation_.y(r, r * rDelta_);
}

// Return analytic potential at specified r, including Coulomb term from local atomtype charges
double PairPotential::analyticEnergy(double r) const
{
    if (r > range_)
        return 0.0;

    // Short-range potential and Coulomb contribution
    return analyticShortRangeEnergy(r) + analyticCoulombEnergy(chargeI_ * chargeJ_, r);
}

// Return analytic potential at specified r, including Coulomb term from supplied charge product
double PairPotential::analyticEnergy(double qiqj, double r, double elecScale, double vdwScale,
                                     PairPotential::CoulombTruncationScheme truncation) const
{
    if (r > range_)
        return 0.0;

    return analyticShortRangeEnergy(r) * vdwScale + analyticCoulombEnergy(qiqj, r, truncation) * elecScale;
}

// Return analytic coulomb potential energy of specified charges
double PairPotential::analyticCoulombEnergy(double qiqj, double r, PairPotential::CoulombTruncationScheme truncation) const
{
    // Calculate based on truncation scheme
    if (truncation == PairPotential::NoCoulombTruncation)
        return COULCONVERT * qiqj / r;
    else if (truncation == PairPotential::ShiftedCoulombTruncation)
        return COULCONVERT * qiqj * (1.0 / r + r / (range_ * range_) - 2.0 / range_);

    return 0.0;
}

// Return derivative at specified r
double PairPotential::force(double r)
{
    assert(r >= 0);

    return -dUFullInterpolation_.y(r, r * rDelta_);
}

// Return analytic force at specified r
double PairPotential::analyticForce(double r) const
{
    if (r > range_)
        return 0.0;

    // Short-range potential and Coulomb contribution
    auto force = analyticShortRangeForce(r) + analyticCoulombForce(chargeI_ * chargeJ_, r);

    return force;
}

// Return analytic force at specified r, including Coulomb term from supplied charge product
double PairPotential::analyticForce(double qiqj, double r, double elecScale, double vdwScale,
                                    PairPotential::CoulombTruncationScheme truncation) const
{
    if (r > range_)
        return 0.0;

    return analyticShortRangeForce(r) * vdwScale + analyticCoulombForce(qiqj, r) * elecScale;
}

// Return analytic coulomb force of specified charges
double PairPotential::analyticCoulombForce(double qiqj, double r, PairPotential::CoulombTruncationScheme truncation) const
{
    /*
     * Derivative of Coulomb's Law is:
     *
     *           dU  q(i)*q(j)       q(i)*q(j)
     *  dU/dr =  --  ---------  =  - ---------
     *           dr      r              r*r
     *
     *                         q(i)*q(j)
     * The force is -(dU/dr) = ---------
     *                            r*r
     */

    // Calculate based on truncation scheme
    if (truncation == PairPotential::NoCoulombTruncation)
        return COULCONVERT * qiqj / (r * r);
    else if (truncation == PairPotential::ShiftedCoulombTruncation)
        return COULCONVERT * qiqj * (1.0 / (r * r) - 1.0 / (range_ * range_));

    return 0.0;
}

// Return full tabulated potential (original plus additional)
Data1D &PairPotential::uFull() { return uFull_; }
const Data1D &PairPotential::uFull() const { return uFull_; }

// Return full tabulated derivative
Data1D &PairPotential::dUFull() { return dUFull_; }
const Data1D &PairPotential::dUFull() const { return dUFull_; }

// Return original potential
Data1D &PairPotential::uOriginal() { return uOriginal_; }
const Data1D &PairPotential::uOriginal() const { return uOriginal_; }

// Return additional potential
Data1D &PairPotential::uAdditional() { return uAdditional_; }
const Data1D &PairPotential::uAdditional() const { return uAdditional_; }

// Zero additional potential
void PairPotential::resetUAdditional()
{
    std::fill(uAdditional_.values().begin(), uAdditional_.values().end(), 0.0);

    calculateUFull();
    calculateDUFull();
}

// Set additional potential
void PairPotential::setUAdditional(Data1D &newUAdditional)
{
    uAdditional_ = newUAdditional;

    calculateUFull();
    calculateDUFull();
}

// Adjust additional potential, and recalculate UFull and dUFull
void PairPotential::adjustUAdditional(const Data1D &u, double factor)
{
    // Interpolate the supplied data 'u' and add it to the additional potential
    Interpolator::addInterpolated(u, uAdditional_, factor);

    calculateUFull();
    calculateDUFull();
}
