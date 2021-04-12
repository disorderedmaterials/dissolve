// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/pairpotential.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/coredata.h"
#include "math/constants.h"
#include <math.h>

// Static members
PairPotential::CoulombTruncationScheme PairPotential::coulombTruncationScheme_ = PairPotential::ShiftedCoulombTruncation;
PairPotential::ShortRangeTruncationScheme PairPotential::shortRangeTruncationScheme_ =
    PairPotential::ShiftedShortRangeTruncation;
double PairPotential::shortRangeTruncationWidth_ = 2.0;

PairPotential::PairPotential()
    : ListItem<PairPotential>(), shortRangeEnergyAtCutoff_(0.0), shortRangeForceAtCutoff_(0.0), includeCoulomb_(true),
      coulombEnergyAtCutoff_(0.0), coulombForceAtCutoff_(0.0), shortRangeType_(Forcefield::UndefinedType), chargeI_(0.0),
      chargeJ_(0.0), nPoints_(0), range_(0.0), delta_(-1.0), rDelta_(0.0), uFullInterpolation_(uFull_),
      dUFullInterpolation_(dUFull_)
{
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
    return EnumOptions<PairPotential::ShortRangeTruncationScheme>("ShortRangeTruncationScheme",
                                                                  {{PairPotential::NoShortRangeTruncation, "None"},
                                                                   {PairPotential::ShiftedShortRangeTruncation, "Shifted"},
                                                                   {PairPotential::CosineShortRangeTruncation, "Cosine"}});
}

/*
 * Seed Interaction Type
 */

// Return short-ranged type
Forcefield::ShortRangeType PairPotential::shortRangeType() const { return shortRangeType_; }

// Set short-ranged truncation scheme
void PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme scheme)
{
    shortRangeTruncationScheme_ = scheme;
}

// Return short-ranged truncation scheme
PairPotential::ShortRangeTruncationScheme PairPotential::shortRangeTruncationScheme() { return shortRangeTruncationScheme_; }

// Set width of short-range potential over which to truncate (if scheme = Cosine)
void PairPotential::setShortRangeTruncationWidth(double width) { shortRangeTruncationWidth_ = width; }

// Return width of short-range potential over which to truncate (if scheme = Cosine)
double PairPotential::shortRangeTruncationWidth() { return shortRangeTruncationWidth_; }

// Set whether Coulomb term should be included in the generated potential
void PairPotential::setIncludeCoulomb(bool b) { includeCoulomb_ = b; }

// Return whether Coulomb term should be included in the generated potential
bool PairPotential::includeCoulomb() { return includeCoulomb_; }

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
    // Check for NULL pointers
    assert(atomTypeI_ && atomTypeJ_);

    uFull_.setTag(fmt::format("{}-{}", atomTypeI_->name(), atomTypeJ_->name()));

    uAdditional_.setTag(fmt::format("{}-{} (Add)", atomTypeI_->name(), atomTypeJ_->name()));

    uOriginal_.setTag(fmt::format("{}-{} (Orig)", atomTypeI_->name(), atomTypeJ_->name()));

    dUFull_.setTag(fmt::format("{}-{} (dU/dr)", atomTypeI_->name(), atomTypeJ_->name()));
}

// Set up PairPotential parameters from specified AtomTypes
bool PairPotential::setUp(std::shared_ptr<AtomType> typeI, std::shared_ptr<AtomType> typeJ)
{
    // Check for NULL pointers
    if (typeI == nullptr)
        throw(std::runtime_error("Invalid AtomType pointer (typeI) given to PairPotential::setUp().\n"));
    if (typeJ == nullptr)
        throw(std::runtime_error("Invalid AtomType pointer (typeJ) given to PairPotential::setUp().\n"));

    atomTypeI_ = typeI;
    atomTypeJ_ = typeJ;
    parameters_.clear();
    setData1DNames();
    auto &paramsI = atomTypeI_->shortRangeParameters();
    auto &paramsJ = atomTypeJ_->shortRangeParameters();

    // Sanity check - are either of the parameter sets empty (i.e. have never been set with useful data)?
    if (paramsI.empty() || paramsJ.empty())
        return Messenger::error(
            "Can't set parameters for PairPotential since there are {} ({}) and {} ({}) parameters set in the atom types.\n",
            paramsI.size(), atomTypeI_->name(), paramsJ.size(), atomTypeJ_->name());

    auto srI = atomTypeI_->shortRangeType(), srJ = atomTypeJ_->shortRangeType();

    // Combine / set parameters as necessary, depending on the short-range interaction types of the supplied AtomTypes
    if (srI == srJ)
    {
        shortRangeType_ = srI;
        switch (shortRangeType_)
        {
            case (Forcefield::UndefinedType):
                return Messenger::error("PairPotential between atom types '{}' and '{}' is undefined.\n", atomTypeI_->name(),
                                        atomTypeJ_->name());
            case (Forcefield::NoInteractionType):
                break;
            case (Forcefield::LennardJonesType):
                /*
                 * Combine parameters (Lorentz-Berthelot):
                 * Parameter 0 = Epsilon
                 * Parameter 1 = Sigma
                 */
                parameters_.push_back(sqrt(paramsI[0] * paramsJ[0]));
                parameters_.push_back((paramsI[1] + paramsJ[1]) * 0.5);
                break;
            case (Forcefield::LennardJonesGeometricType):
                /*
                 * Combine parameters (Geometric):
                 * Parameter 0 = Epsilon
                 * Parameter 1 = Sigma
                 */
                parameters_.push_back(sqrt(paramsI[0] * paramsJ[0]));
                parameters_.push_back(sqrt(paramsI[1] * paramsJ[1]));
                break;
            default:
                return Messenger::error("Short-range type {} is not accounted for in PairPotential::setUp().\n",
                                        shortRangeType_);
        }
    }
    else
    {
        // In the case of combining LJ and LJGeometric, default to standard Lorentz-Berthelot rules
        auto ljI = srI == Forcefield::LennardJonesType || srI == Forcefield::LennardJonesGeometricType;
        auto ljJ = srJ == Forcefield::LennardJonesType || srJ == Forcefield::LennardJonesGeometricType;
        if (ljI && ljJ)
        {
            Messenger::warn("Defaulting to Lorentz-Berthelot rules to combine parameters between atom types '{}' and '{}.\n",
                            atomTypeI_->name(), atomTypeJ_->name());

            shortRangeType_ = Forcefield::LennardJonesType;
            /*
             * Combine parameters (Lorentz-Berthelot):
             * Parameter 0 = Epsilon
             * Parameter 1 = Sigma
             */
            parameters_.push_back(sqrt(paramsI[0] * paramsJ[0]));
            parameters_.push_back((paramsI[1] + paramsJ[1]) * 0.5);
        }
        else
        {
            // Can't mix parameters of different functional forms in general, so complain...
            shortRangeType_ = Forcefield::UndefinedType;
            return Messenger::error("Can't generate potential parameters between atom types '{}' and '{}', which have "
                                    "short-range types {} and {}.\n",
                                    atomTypeI_->name(), atomTypeJ_->name(),
                                    Forcefield::shortRangeTypes().keyword(atomTypeI_->shortRangeType()),
                                    Forcefield::shortRangeTypes().keyword(atomTypeJ_->shortRangeType()));
        }
    }

    // Set charges
    chargeI_ = atomTypeI_->charge();
    chargeJ_ = atomTypeJ_->charge();

    return true;
}

// Return first AtomType name
std::string_view PairPotential::atomTypeNameI() const
{
    assert(atomTypeI_);
    return atomTypeI_->name();
}

// Return second AtomType name
std::string_view PairPotential::atomTypeNameJ() const
{
    assert(atomTypeJ_);
    return atomTypeJ_->name();
}

// Return first source AtomType
std::shared_ptr<AtomType> PairPotential::atomTypeI() const { return atomTypeI_; }

// Return second source AtomType
std::shared_ptr<AtomType> PairPotential::atomTypeJ() const { return atomTypeJ_; }

// Set parameter with index specified
void PairPotential::setParameter(int index, double value) { parameters_[index] = value; }

// Return parameters vector
const std::vector<double> &PairPotential::parameters() const { return parameters_; }

// Return short-range parameter with index specified
double PairPotential::parameter(int index) const { return parameters_[index]; }

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
double PairPotential::analyticShortRangeEnergy(double r, Forcefield::ShortRangeType type,
                                               PairPotential::ShortRangeTruncationScheme truncation)
{
    if (type == Forcefield::NoInteractionType)
        return 0.0;
    else if ((type == Forcefield::LennardJonesType) || (type == Forcefield::LennardJonesGeometricType))
    {
        /*
         * Standard Lennard-Jones potential
         * Parameter 0 = Epsilon
         * Parameter 1 = Sigma
         */
        auto sigmar = parameters_[1] / r;
        auto sigmar6 = pow(sigmar, 6.0);
        auto sigmar12 = sigmar6 * sigmar6;
        auto energy = 4.0 * parameters_[0] * (sigmar12 - sigmar6);

        // Apply the selected truncation scheme
        if (truncation == PairPotential::ShiftedShortRangeTruncation)
        {
            energy += -(r - range_) * shortRangeForceAtCutoff_ - shortRangeEnergyAtCutoff_;
        }
        else if (truncation == PairPotential::CosineShortRangeTruncation)
        {
            // Are we into the truncation strip?
            auto truncr = r - (range_ - shortRangeTruncationWidth_);
            if (truncr >= 0)
            {
                // Simple truncation scheme - (cos(x)+1)*0.5, mapping the truncation region to {0,Pi}
                energy *= (cos(PI * (truncr / shortRangeTruncationWidth_)) + 1) * 0.5;
            }
        }

        return energy;
    }

    Messenger::error("Short-range interaction type {} is not accounted for in PairPotential::analyticShortRangeEnergy(). "
                     "Returning 0.0...\n",
                     type);

    return 0.0;
}

// Return analytic short range force
double PairPotential::analyticShortRangeForce(double r, Forcefield::ShortRangeType type,
                                              PairPotential::ShortRangeTruncationScheme truncation)
{
    if (type == Forcefield::NoInteractionType)
        return 0.0;
    else if ((type == Forcefield::LennardJonesType) || (type == Forcefield::LennardJonesGeometricType))
    {
        /*
         * Standard Lennard-Jones potential
         * Parameter 0 = Epsilon
         * Parameter 1 = Sigma
         */

        // f = -48*epsilon*((sigma**12/x**13)-0.5*(sigma**6/x**7))

        auto sigmar = parameters_[1] / r;
        auto sigmar6 = pow(sigmar, 6.0);
        auto sigmar12 = sigmar6 * sigmar6;

        // Apply the selected truncation scheme
        if (truncation == PairPotential::NoShortRangeTruncation)
            return 48.0 * parameters_[0] * sigmar6 * (-sigmar6 + 0.5) / r;
        else if (truncation == PairPotential::ShiftedShortRangeTruncation)
        {
            return (48.0 * parameters_[0] * sigmar6 * (-sigmar6 + 0.5) / r) - shortRangeForceAtCutoff_;
        }
        else if (truncation == PairPotential::CosineShortRangeTruncation)
        {
            // Are we into the truncation strip?
            double truncr = r - (range_ - shortRangeTruncationWidth_);
            if (truncr >= 0)
            {
                // Simple truncation scheme - (cos(x)+1)*0.5, mapping the truncation region to {0,Pi}
                // d/dx = -PI*sin((PI*truncr)/shortRangeTruncationWidth_) / shortRangeTruncationWidth_
                double de_t = (-48.0 * parameters_[0] * ((sigmar12 / pow(r, 13.0)) - 0.5 * (sigmar6 / pow(r, 7.0)))) *
                              (cos(PI * (truncr / shortRangeTruncationWidth_)) + 1) * 0.5;
                double e_dt = (4.0 * parameters_[0] * (sigmar12 - sigmar6)) * -PI *
                              sin((PI * truncr) / shortRangeTruncationWidth_) / shortRangeTruncationWidth_;
                return de_t * e_dt;
            }
            else
                return 48.0 * parameters_[0] * sigmar6 * (-sigmar6 + 0.5) / r;
        }
    }

    Messenger::error("Short-range interaction type {} is not accounted for in PairPotential::analyticShortRangeForce(). "
                     "Returning 0.0...\n",
                     type);

    return 0.0;
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
    dUFull_.value(nPoints_ - 1) = dUFull_.value(nPoints_ - 2);

    // Update interpolation
    dUFullInterpolation_.interpolate(Interpolator::ThreePointInterpolation);
}

// Generate energy and force tables
bool PairPotential::tabulate(double maxR, double delta, bool includeCoulomb)
{
    // Check that AtomType pointers were set at some pointer
    if ((atomTypeI_ == nullptr) || (atomTypeJ_ == nullptr))
    {
        Messenger::error("NULL_POINTER - One or both AtomTypes in PairPotential are NULL.\n");
        return false;
    }

    // Determine nPoints_
    delta_ = delta;
    rDelta_ = 1.0 / delta_;
    range_ = maxR;
    includeCoulomb_ = includeCoulomb;
    nPoints_ = range_ / delta_;

    // Calculate energies and forces at the cutoff distance, for later use in truncation schemes
    shortRangeEnergyAtCutoff_ = analyticShortRangeEnergy(range_, shortRangeType_, PairPotential::NoShortRangeTruncation);
    shortRangeForceAtCutoff_ = analyticShortRangeForce(range_, shortRangeType_, PairPotential::NoShortRangeTruncation);
    coulombEnergyAtCutoff_ = analyticCoulombEnergy(chargeI_ * chargeJ_, range_, PairPotential::NoCoulombTruncation);
    coulombForceAtCutoff_ = analyticCoulombForce(chargeI_ * chargeJ_, range_, PairPotential::NoCoulombTruncation);

    // Initialise original and additional potential arrays, and calculate original potential
    uOriginal_.initialise(nPoints_);
    calculateUOriginal(false);

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

// (Re)generate potential from current parameters
void PairPotential::calculateUOriginal(bool recalculateUFull)
{
    double r;

    for (auto n = 1; n < nPoints_; ++n)
    {
        r = n * delta_;
        uOriginal_.xAxis(n) = r;

        // Construct potential
        uOriginal_.value(n) = 0.0;

        // Short-range potential contribution
        uOriginal_.value(n) += analyticShortRangeEnergy(r, shortRangeType_);

        // -- Add Coulomb contribution
        if (includeCoulomb_)
            uOriginal_.value(n) += analyticCoulombEnergy(chargeI_ * chargeJ_, r);
    }

    // Since the first point (at zero) risks being a nan, set it to ten times the second point instead
    uOriginal_.value(0) = 10.0 * uOriginal_.value(1);

    // Update full potential (if not the first generation of the potential)
    if (recalculateUFull)
        calculateUFull();
}

// Return potential at specified r
double PairPotential::energy(double r)
{
    assert(r >= 0);

    return uFullInterpolation_.y(r, r * rDelta_);
}

// Return analytic potential at specified r, including Coulomb term from local atomtype charges
double PairPotential::analyticEnergy(double r)
{
    if (r > range_)
        return 0.0;

    // Short-range potential
    auto energy = analyticShortRangeEnergy(r, shortRangeType_);

    // Coulomb contribution
    energy += analyticCoulombEnergy(chargeI_ * chargeJ_, r);

    return energy;
}

// Return analytic potential at specified r, including Coulomb term from supplied charge product
double PairPotential::analyticEnergy(double qiqj, double r, PairPotential::CoulombTruncationScheme truncation)
{
    if (r > range_)
        return 0.0;

    return analyticShortRangeEnergy(r, shortRangeType_) + analyticCoulombEnergy(qiqj, r, truncation);
}

// Return analytic coulomb potential energy of specified charges
double PairPotential::analyticCoulombEnergy(double qiqj, double r, PairPotential::CoulombTruncationScheme truncation)
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

    return dUFullInterpolation_.y(r, r * rDelta_);
}

// Return analytic force at specified r
double PairPotential::analyticForce(double r)
{
    if (r > range_)
        return 0.0;

    // Short-range potential
    double force = analyticShortRangeForce(r, shortRangeType_);

    // Coulomb contribution
    force += analyticCoulombForce(chargeI_ * chargeJ_, r);

    return force;
}

// Return analytic force at specified r, including Coulomb term from supplied charge product
double PairPotential::analyticForce(double qiqj, double r, PairPotential::CoulombTruncationScheme truncation)
{
    if (r > range_)
        return 0.0;

    return analyticShortRangeForce(r, shortRangeType_) + analyticCoulombForce(qiqj, r);
}

// Return analytic coulomb force of specified charges
double PairPotential::analyticCoulombForce(double qiqj, double r, PairPotential::CoulombTruncationScheme truncation)
{
    // Calculate based on truncation scheme
    if (truncation == PairPotential::NoCoulombTruncation)
        return -COULCONVERT * qiqj / (r * r);
    else if (truncation == PairPotential::ShiftedCoulombTruncation)
        return -COULCONVERT * qiqj * (1.0 / (r * r) - 1.0 / (range_ * range_));

    return 0.0;
}

// Return full tabulated potential (original plus additional)
Data1D &PairPotential::uFull() { return uFull_; }

// Return full tabulated derivative
Data1D &PairPotential::dUFull() { return dUFull_; }

// Return original potential
Data1D &PairPotential::uOriginal() { return uOriginal_; }

// Return additional potential
Data1D &PairPotential::uAdditional() { return uAdditional_; }

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
void PairPotential::adjustUAdditional(Data1D u, double factor)
{
    // Interpolate the supplied data 'u' and add it to the additional potential
    Interpolator::addInterpolated(uAdditional_, u, factor);

    calculateUFull();
    calculateDUFull();
}
