// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/pairPotential.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "math/derivative.h"

// Static members
VersionCounter PairPotential::coreDefinitionsVersion_ = ++VersionCounter();
double PairPotential::range_ = 15.0;
double PairPotential::delta_ = 0.005;
int PairPotential::nPoints_ = int(15.0 / 0.005);
double PairPotential::rDelta_ = 1.0 / 0.005;
PairPotential::CoulombTruncationScheme PairPotential::coulombTruncationScheme_ = PairPotential::ShiftedCoulombTruncation;
PairPotential::ShortRangeTruncationScheme PairPotential::shortRangeTruncationScheme_ =
    PairPotential::ShiftedShortRangeTruncation;
PairPotential::ChargeSource PairPotential::chargeSource_ = PairPotential::ChargeSource::Automatic;
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

// Return enum options for ChargeSource
EnumOptions<PairPotential::ChargeSource> PairPotential::chargeSources()
{
    return EnumOptions<PairPotential::ChargeSource>("ChargeSource", {{ChargeSource::Automatic, "Automatic"},
                                                                     {ChargeSource::SpeciesAtoms, "SpeciesAtoms"},
                                                                     {ChargeSource::AtomTypes, "AtomTypes"}});
}

/*
 * Global Parameters
 */

// Set pair potential range and delta
void PairPotential::setRange(double range, std::optional<double> delta)
{
    range_ = range;
    if (delta)
        delta_ = *delta;

    // Determine actual delta to cover requested pair potential range
    nPoints_ = int(range_ / delta_);
    delta_ += (range_ - (nPoints_ * delta_)) / nPoints_;
    rDelta_ = 1.0 / delta_;

    ++coreDefinitionsVersion_;
}

// Return pair potential range
double PairPotential::range() { return range_; }

// Return spacing between points
double PairPotential::delta() { return delta_; }

// Set short-ranged truncation scheme
void PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme scheme)
{
    if (scheme != shortRangeTruncationScheme_)
        ++coreDefinitionsVersion_;
    shortRangeTruncationScheme_ = scheme;
}

// Return short-ranged truncation scheme
PairPotential::ShortRangeTruncationScheme PairPotential::shortRangeTruncationScheme() { return shortRangeTruncationScheme_; }

// Set charge source
void PairPotential::setChargeSource(PairPotential::ChargeSource source)
{
    if (chargeSource_ != source)
        ++coreDefinitionsVersion_;
    chargeSource_ = source;

    // Set whether to include charges in pair potentials here - it will be overridden as necessary for ChargeSource::Auto
    includeCoulombPotential_ = source != ChargeSource::SpeciesAtoms;
}

// Return current charge source
PairPotential::ChargeSource PairPotential::chargeSource() { return chargeSource_; }

// Set whether Coulomb contributions should be included in the generated potential
void PairPotential::setIncludeCoulombPotential(bool b)
{
    if (b != includeCoulombPotential_)
        ++coreDefinitionsVersion_;
    includeCoulombPotential_ = b;
}

// Return whether Coulomb contributions should be included in the generated potential
bool PairPotential::includeCoulombPotential() { return includeCoulombPotential_; }

// Set Coulomb truncation scheme
void PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme scheme)
{
    if (scheme != coulombTruncationScheme_)
        ++coreDefinitionsVersion_;
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
    totalPotential_.setTag(std::format("{}-{}", nameI_, nameJ_));

    additionalShortRangePotential_.setTag(std::format("{}-{} (Add SR)", nameI_, nameJ_));
    referenceShortRangePotential_.setTag(std::format("{}-{} (Ref SR)", nameI_, nameJ_));

    coulombPotential_.setTag(std::format("{}-{} (Elec)", nameI_, nameJ_));

    totalDerivative_.setTag(std::format("{}-{} (dU/dr)", nameI_, nameJ_));
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
    version_.zero();
    return interactionPotential_.setFormAndParameters(form, parameters) &&
           potentialFunction_.setFormAndParameters(form, interactionPotential_.parameters());
}
bool PairPotential::setInteractionPotential(const InteractionPotential<Functions1D> &potential)
{
    version_.zero();
    interactionPotential_ = potential;
    return potentialFunction_.setFormAndParameters(interactionPotential_.form(), interactionPotential_.parameters());
}

// Set form of interaction potential
void PairPotential::setInteractionPotentialForm(Functions1D::Form form)
{
    version_.zero();
    interactionPotential_.setForm(form);
    potentialFunction_.setFormAndParameters(interactionPotential_.form(), interactionPotential_.parameters());
}

// Return interaction potential
const InteractionPotential<Functions1D> &PairPotential::interactionPotential() const { return interactionPotential_; }

// Set local charge product (if including Coulomb terms)
void PairPotential::setLocalChargeProduct(double qiqj)
{
    version_.zero();
    localChargeProduct_ = qiqj;
}

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
    totalShortRangePotentialInterpolation_ = Interpolator(totalShortRangePotential_, Interpolator::ThreePointInterpolation);
    coulombPotentialInterpolation_ = Interpolator(coulombPotential_, Interpolator::ThreePointInterpolation);
    totalPotentialInterpolation_ = Interpolator(totalPotential_, Interpolator::ThreePointInterpolation);

    // Calculate derivatives
    totalShortRangeDerivative_ = Derivative::derivative(totalShortRangePotential_);
    coulombDerivative_ = Derivative::derivative(coulombPotential_);
    totalDerivative_ = Derivative::derivative(totalPotential_);

    // Update interpolations for derivatives
    totalShortRangeDerivativeInterpolation_ = Interpolator(totalShortRangeDerivative_, Interpolator::ThreePointInterpolation);
    coulombDerivativeInterpolation_ = Interpolator(coulombDerivative_, Interpolator::ThreePointInterpolation);
    totalDerivativeInterpolation_ = Interpolator(totalDerivative_, Interpolator::ThreePointInterpolation);
}

// Generate energy and force tables
void PairPotential::tabulate()
{
    // Are we already up do date?
    if (version_ == coreDefinitionsVersion_)
        return;

    // Precalculate some quantities
    shortRangeEnergyAtCutoff_ = analyticShortRangeEnergy(range_, PairPotential::NoShortRangeTruncation);
    shortRangeForceAtCutoff_ = analyticShortRangeForce(range_, PairPotential::NoShortRangeTruncation);

    // Set up containers
    referenceShortRangePotential_.initialise(nPoints_);
    for (auto n = 0; n < nPoints_; ++n)
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
        coul = analyticCoulombEnergy(includeCoulombPotential_ ? localChargeProduct_ : 0.0, r);
    }

    // Since the first point at r = 0.0 risks being a nan, set it to ten times the second point instead
    referenceShortRangePotential_.value(0) = 10.0 * referenceShortRangePotential_.value(1);
    coulombPotential_.value(0) = 10.0 * coulombPotential_.value(1);

    // Ensure additional potential is set to zero and update full potential
    std::fill(additionalShortRangePotential_.values().begin(), additionalShortRangePotential_.values().end(), 0);

    // Update totals
    updateTotals();

    version_ = coreDefinitionsVersion_;
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

// Return potential at specified r
double PairPotential::energy(double r) const
{
    assert(r >= 0);

    return totalPotentialInterpolation_.y(r, r * rDelta_);
}
double PairPotential::energy(double r, double elecScale, double srScale) const
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
    return srScale * analyticShortRangeEnergy(r) +
           elecScale * analyticCoulombEnergy(includeCoulombPotential_ ? localChargeProduct_ : 0.0, r);
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
        return CoulConvert * qiqj / r;
    else if (truncation == PairPotential::ShiftedCoulombTruncation)
        return CoulConvert * qiqj * (1.0 / r + r / (range_ * range_) - 2.0 / range_);

    return 0.0;
}

// Return derivative at specified r
double PairPotential::force(double r) const
{
    assert(r >= 0);

    return -totalDerivativeInterpolation_.y(r, r * rDelta_);
}
double PairPotential::force(double r, double elecScale, double srScale) const
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
    return srScale * analyticShortRangeForce(r) +
           elecScale * analyticCoulombForce(includeCoulombPotential_ ? localChargeProduct_ : 0.0, r);
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
        return CoulConvert * qiqj / (r * r);
    else if (truncation == PairPotential::ShiftedCoulombTruncation)
        return CoulConvert * qiqj * (1.0 / (r * r) - 1.0 / (range_ * range_));

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
 * Serialisation
 */

// Express as a serialisable value
void PairPotential::serialise(std::string tag, SerialisedValue &target) const
{
    auto &result = target[tag];
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
}

// Read values from a serialisable value
void PairPotential::deserialise(const SerialisedValue &node)
{
    using namespace Deserialisable;
    nameI_ = de<std::string>(node.at("nameI"));
    nameJ_ = de<std::string>(node.at("nameJ"));

    Functions1D::Form form;
    optionalOn(node, "form", [&](const auto node) { form = Functions1D::forms().enumeration(std::string(node.as_string())); });

    std::vector<double> parameters;
    optionalOn(node, "parameters",
               [&](const auto node)
               {
                   auto &parameterNames = Functions1D::parameters(form);
                   std::transform(parameterNames.begin(), parameterNames.end(), std::back_inserter(parameters),
                                  [&node](const auto parameterName) { return node.at(parameterName).as_floating(); });
               });

    setInteractionPotential({form, parameters});
}
