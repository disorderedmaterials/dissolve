// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/pairPotential.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "math/constants.h"
#include "math/derivative.h"
#include <cmath>

// Static members
PairPotential::CoulombTruncationScheme PairPotential::coulombTruncationScheme_ = PairPotential::ShiftedCoulombTruncation;
PairPotential::ShortRangeTruncationScheme PairPotential::shortRangeTruncationScheme_ =
    PairPotential::ShiftedShortRangeTruncation;
bool PairPotential::includeCoulombPotential_ = false;

PairPotential::PairPotential(std::string_view nameI, std::string_view nameJ)
    : nameI_(nameI), nameJ_(nameJ), totalShortRangePotentialInterpolation_(referenceShortRangePotential_),
      coulombPotentialInterpolation_(coulombPotential_), totalPotentialInterpolation_(totalPotential_),
      totalShortRangeDerivativeInterpolation_(totalShortRangeDerivative_), coulombDerivativeInterpolation_(coulombDerivative_),
      totalDerivativeInterpolation_(totalDerivative_), interactionPotential_{Functions1D::Form::None, ""},
      potentialFunction_{Functions1D::Form::None, {}}
{
}

PairPotential::PairPotential(std::string_view nameI, std::string_view nameJ, const InteractionPotential<Functions1D> &potential)
    : nameI_(nameI), nameJ_(nameJ), interactionPotential_(potential),
      totalShortRangePotentialInterpolation_(referenceShortRangePotential_), coulombPotentialInterpolation_(coulombPotential_),
      totalPotentialInterpolation_(totalPotential_), totalShortRangeDerivativeInterpolation_(totalShortRangeDerivative_),
      coulombDerivativeInterpolation_(coulombDerivative_), totalDerivativeInterpolation_(totalDerivative_)
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

// Set whether Coulomb contributions should be included in the generated potential
void PairPotential::setIncludeCoulombPotential(bool b) { includeCoulombPotential_ = b; }

// Return whether Coulomb contributions should be included in the generated potential
bool PairPotential::includeCoulombPotential() { return includeCoulombPotential_; }

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

// Set Data1D names from source AtomTypes
void PairPotential::setData1DNames()
{
    totalPotential_.setTag(fmt::format("{}-{}", nameI_, nameJ_));

    additionalShortRangePotential_.setTag(fmt::format("{}-{} (Add SR)", nameI_, nameJ_));
    referenceShortRangePotential_.setTag(fmt::format("{}-{} (Ref SR)", nameI_, nameJ_));

    coulombPotential_.setTag(fmt::format("{}-{} (Elec)", nameI_, nameJ_));

    totalDerivative_.setTag(fmt::format("{}-{} (dU/dr)", nameI_, nameJ_));
}

// Set names reflecting target atom types for potential
void PairPotential::setNames(std::string_view nameI, std::string_view nameJ)
{
    nameI_ = nameI;
    nameJ_ = nameJ;
    setData1DNames();
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
bool PairPotential::setInteractionPotential(const InteractionPotential<Functions1D> &potential)
{
    interactionPotential_ = potential;
    return potentialFunction_.setFormAndParameters(interactionPotential_.form(), interactionPotential_.parameters());
}

// Set form of interaction potential
void PairPotential::setInteractionPotentialForm(Functions1D::Form form)
{
    interactionPotential_.setForm(form);
    potentialFunction_.setFormAndParameters(interactionPotential_.form(), interactionPotential_.parameters());
}

// Return interaction potential
const InteractionPotential<Functions1D> &PairPotential::interactionPotential() const { return interactionPotential_; }

// Return local charge product (if including Coulomb terms)
double PairPotential::localChargeProduct() const { return localChargeProduct_; }

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

// Update totals
void PairPotential::updateTotals()
{
    // Update total energy
    for (auto &&[total, totalSR, refSR, addSR, coul] :
         zip(totalPotential_.values(), totalShortRangePotential_.values(), referenceShortRangePotential_.values(),
             additionalShortRangePotential_.values(), coulombPotential_.values()))
    {
        totalSR = refSR + addSR;
        total = totalSR + coul;
    }

    // Recalculate interpolations
    totalShortRangePotentialInterpolation_.interpolate(Interpolator::ThreePointInterpolation);
    coulombPotentialInterpolation_.interpolate(Interpolator::ThreePointInterpolation);
    totalPotentialInterpolation_.interpolate(Interpolator::ThreePointInterpolation);

    // Calculate derivatives
    totalShortRangeDerivative_ = Derivative::derivative(totalShortRangePotential_);
    coulombDerivative_ = Derivative::derivative(coulombPotential_);
    totalDerivative_ = Derivative::derivative(totalPotential_);

    // Update interpolations for derivatives
    totalShortRangeDerivativeInterpolation_.interpolate(Interpolator::ThreePointInterpolation);
    coulombDerivativeInterpolation_.interpolate(Interpolator::ThreePointInterpolation);
    totalDerivativeInterpolation_.interpolate(Interpolator::ThreePointInterpolation);
}

// Generate energy and force tables
void PairPotential::tabulate(double maxR, double delta, double chargeProduct)
{
    // Determine
    delta_ = delta;
    rDelta_ = 1.0 / delta_;
    range_ = maxR;

    // Precalculate some quantities
    shortRangeEnergyAtCutoff_ = analyticShortRangeEnergy(range_, PairPotential::NoShortRangeTruncation);
    shortRangeForceAtCutoff_ = analyticShortRangeForce(range_, PairPotential::NoShortRangeTruncation);
    localChargeProduct_ = includeCoulombPotential_ ? chargeProduct : 0.0;

    // Set up containers
    const auto nPoints = int(range_ / delta);
    referenceShortRangePotential_.initialise(nPoints);
    for (auto n = 0; n < nPoints; ++n)
        referenceShortRangePotential_.xAxis()[n] = n * delta_;
    coulombPotential_ = referenceShortRangePotential_;
    additionalShortRangePotential_ = referenceShortRangePotential_;
    totalShortRangePotential_ = referenceShortRangePotential_;
    totalPotential_ = referenceShortRangePotential_;
    totalDerivative_ = referenceShortRangePotential_;

    // Tabulate reference short-range and coulomb energies
    for (auto &&[r, refSR, coul] :
         zip(referenceShortRangePotential_.xAxis(), referenceShortRangePotential_.values(), coulombPotential_.values()))
    {
        refSR = analyticShortRangeEnergy(r);
        coul = analyticCoulombEnergy(localChargeProduct_, r);
    }

    // Since the first point at r = 0.0 risks being a nan, set it to ten times the second point instead
    referenceShortRangePotential_.value(0) = 10.0 * referenceShortRangePotential_.value(1);
    coulombPotential_.value(0) = 10.0 * coulombPotential_.value(1);

    // Ensure additional potential is set to zero and update full potential
    std::fill(additionalShortRangePotential_.values().begin(), additionalShortRangePotential_.values().end(), 0);

    // Update totals
    updateTotals();
}

// Add supplied function to the reference short-range potential
void PairPotential::addToReferenceShortRangePotential(const Function1DWrapper &potential, bool overwriteExisting)
{
    if (overwriteExisting)
        std::fill(referenceShortRangePotential_.values().begin(), referenceShortRangePotential_.values().end(), 0.0);

    for (auto &&[r, sr] : zip(referenceShortRangePotential_.xAxis(), referenceShortRangePotential_.values()))
        sr += potential.y(r);

    // Update totals
    updateTotals();
}

// Return range of potential
double PairPotential::range() const { return range_; }

// Return spacing between points
double PairPotential::delta() const { return delta_; }

// Return potential at specified r
double PairPotential::energy(double r)
{
    assert(r >= 0);

    return totalPotentialInterpolation_.y(r, r * rDelta_);
}
double PairPotential::energy(double r, double elecScale, double srScale)
{
    assert(r >= 0);

    return totalShortRangePotentialInterpolation_.y(r, r * rDelta_) * srScale +
           coulombPotentialInterpolation_.y(r, r * rDelta_) * elecScale;
}

// Return analytic potential at specified r, including Coulomb term from local atomtype charges
double PairPotential::analyticEnergy(double r, double elecScale, double srScale) const
{
    if (r > range_)
        return 0.0;

    // Short-range potential and Coulomb contribution
    return srScale * analyticShortRangeEnergy(r) + elecScale * analyticCoulombEnergy(localChargeProduct_, r);
}

// Return analytic potential at specified r, including Coulomb term from supplied charge product
double PairPotential::analyticEnergy(double qiqj, double r, double elecScale, double srScale,
                                     PairPotential::CoulombTruncationScheme truncation) const
{
    if (r > range_)
        return 0.0;

    return analyticShortRangeEnergy(r) * srScale + analyticCoulombEnergy(qiqj, r, truncation) * elecScale;
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

    return -totalDerivativeInterpolation_.y(r, r * rDelta_);
}
double PairPotential::force(double r, double elecScale, double srScale)
{
    assert(r >= 0);

    return -(totalShortRangeDerivativeInterpolation_.y(r, r * rDelta_) * srScale +
             coulombDerivativeInterpolation_.y(r, r * rDelta_) * elecScale);
}

// Return analytic force at specified r
double PairPotential::analyticForce(double r, double elecScale, double srScale) const
{
    if (r > range_)
        return 0.0;

    // Short-range potential and Coulomb contribution
    return srScale * analyticShortRangeForce(r) + elecScale * analyticCoulombForce(localChargeProduct_, r);
}

// Return analytic force at specified r, including Coulomb term from supplied charge product
double PairPotential::analyticForce(double qiqj, double r, double elecScale, double srScale,
                                    PairPotential::CoulombTruncationScheme truncation) const
{
    if (r > range_)
        return 0.0;

    return analyticShortRangeForce(r) * srScale + analyticCoulombForce(qiqj, r) * elecScale;
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
const Data1D &PairPotential::totalPotential() const { return totalPotential_; }

// Return full tabulated derivative
const Data1D &PairPotential::derivative() const { return totalDerivative_; }

// Return short-range potential
const Data1D &PairPotential::shortRangePotential() const { return referenceShortRangePotential_; }

// Return Coulomb potential
const Data1D &PairPotential::coulombPotential() const { return coulombPotential_; }

// Return additional potential
const Data1D &PairPotential::additionalPotential() const { return additionalShortRangePotential_; }

// Zero additional potential
void PairPotential::resetAdditionalPotential()
{
    std::fill(additionalShortRangePotential_.values().begin(), additionalShortRangePotential_.values().end(), 0.0);

    updateTotals();
}

// Set additional potential
void PairPotential::setAdditionalPotential(Data1D &newUAdditional)
{
    additionalShortRangePotential_ = newUAdditional;

    updateTotals();
}

/*
 * I/O
 */

// Express as a serialisable value
SerialisedValue PairPotential::serialise() const
{
    SerialisedValue result;
    result["nameI"] = nameI_;
    result["nameJ"] = nameJ_;
    result["form"] = Functions1D::forms().keyword(interactionPotential_.form());

    auto &values = interactionPotential().parameters();
    if (!values.empty())
    {
        SerialisedValue potentialParameters;
        auto &parameters = Functions1D::parameters(interactionPotential_.form());
        for (auto &&[parameter, value] : zip(parameters, values))
            potentialParameters[parameter] = value;
        result["parameters"] = potentialParameters;
    }
    return result;
}

// Read values from a serialisable value
void PairPotential::deserialise(const SerialisedValue &node)
{
    nameI_ = toml::find<std::string>(node, "nameI");
    nameJ_ = toml::find<std::string>(node, "nameJ");

    Functions1D::Form form;
    Serialisable::optionalOn(node, "form",
                             [&](const auto node) { form = Functions1D::forms().enumeration(std::string(node.as_string())); });

    std::vector<double> parameters;
    Serialisable::optionalOn(node, "parameters",
                             [&](const auto node)
                             {
                                 auto &parameterNames = Functions1D::parameters(form);
                                 std::transform(parameterNames.begin(), parameterNames.end(), std::back_inserter(parameters),
                                                [&node](const auto parameterName)
                                                { return node.at(parameterName).as_floating(); });
                             });

    setInteractionPotential({form, parameters});
}
