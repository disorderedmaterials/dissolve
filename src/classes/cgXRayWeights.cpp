// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/cgXRayWeights.h"
#include "base/lineParser.h"
#include "classes/atomType.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <functional>
#include <numeric>

CGXRayWeights::CGXRayWeights() {}

CGXRayWeights::CGXRayWeights(const CGXRayWeights &source) { (*this) = source; }

void CGXRayWeights::operator=(const CGXRayWeights &source)
{
    formFactors_ = source.formFactors_;
    atomTypeMix_ = source.atomTypeMix_;
    concentrations_ = source.concentrations_;
    concentrationProducts_ = source.concentrationProducts_;
    formFactorData_ = source.formFactorData_;
    preFactors_ = source.preFactors_;
    valid_ = source.valid_;
    beadMap_ = source.beadMap_;
}

/*
 * Source AtomTypes
 */

// Initialise form factor data for the current atom types
bool CGXRayWeights::initialiseFormFactors()
{
    formFactorData_.clear();

    for (auto &atd : atomTypeMix_)
    {
        auto at = atd.atomType();

        // Try to retrieve form factor data for this atom type (element, formal charge [TODO])
        auto data = XRayFormFactors::formFactorData(formFactors_, at->Z());
        if (!data)
            return Messenger::error("No form factor data present for element {} (formal charge {}) in x-ray data set '{}'.\n",
                                    Elements::symbol(at->Z()), 0, XRayFormFactors::xRayFormFactorData().keyword(formFactors_));

        formFactorData_.push_back(*data);
    }

    return true;
}

// Clear contents
void CGXRayWeights::clear()
{
    atomTypeMix_.clear();
    concentrations_.clear();
    concentrationProducts_.clear();
    preFactors_.clear();
    valid_ = false;
    beadMap_.clear();
}

// Set-up from supplied SpeciesInfo list
bool CGXRayWeights::setUp(std::vector<std::pair<const Species *, int>> &speciesPopulations,
                        XRayFormFactors::XRayFormFactorData formFactors)
{
    valid_ = false;

    // Fill atomTypeMix_ list with AtomType populations, based on Isotopologues relative populations and associated Species
    // populations
    atomTypeMix_.clear();
    for (auto &spPop : speciesPopulations)
        for (const auto &i : spPop.first->atoms())
            atomTypeMix_.add(i.atomType(), spPop.second);

    // Perform final setup based on now-completed atomtypes list
    return finalise(formFactors);
}

// Add Species to weights in the specified population
void CGXRayWeights::addSpecies(const Species *sp, int population)
{
    for (const auto &i : sp->atoms())
        if (i.isPresence(SpeciesAtom::Presence::Physical))
            atomTypeMix_.add(i.atomType(), population);

    valid_ = false;
}

// Finalise weights after addition of all individual Species
bool CGXRayWeights::finalise(XRayFormFactors::XRayFormFactorData formFactors)
{
    valid_ = false;

    atomTypeMix_.finalise();

    // Retrieve form factor data for the current atom types
    formFactors_ = formFactors;
    if (!initialiseFormFactors())
        return false;

    setUpMatrices();

    valid_ = true;

    return true;
}

// Return X-Ray form factors being used
XRayFormFactors::XRayFormFactorData CGXRayWeights::formFactors() const { return formFactors_; }

// Return AtomTypeMix
const AtomTypeMix &CGXRayWeights::atomTypeMix() const { return atomTypeMix_; }

// Return number of used AtomTypes
int CGXRayWeights::nUsedTypes() const { return atomTypeMix_.nItems(); }

// Print atomtype information
void CGXRayWeights::print() const
{
    // Print atomtypes table
    Messenger::print("\n");
    atomTypeMix_.print();
}

/*
 * Data
 */

// Set up matrices based on current AtomType information
void CGXRayWeights::setUpMatrices()
{
    concentrations_.clear();
    concentrations_.resize(atomTypeMix_.nItems());
    concentrationProducts_.initialise(atomTypeMix_.nItems(), atomTypeMix_.nItems(), true);
    preFactors_.initialise(atomTypeMix_.nItems(), atomTypeMix_.nItems(), true);

    // Determine atomic concentration products and full pre-factor
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypeMix_.begin(), atomTypeMix_.end(),
                            [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2)
                            {
                                double ci = atd1.fraction();
                                concentrations_.at(typeI) = ci;

                                double cj = atd2.fraction();

                                concentrationProducts_[{typeI, typeJ}] = ci * cj;
                                preFactors_[{typeI, typeJ}] = ci * cj * (typeI == typeJ ? 1 : 2);
                            });
}

// Return concentration product for type i
double CGXRayWeights::concentration(int typeIndexI) const { return concentrations_[typeIndexI]; }

// Return concentration product for types i and j
double CGXRayWeights::concentrationProduct(int typeIndexI, int typeIndexJ) const
{
    return concentrationProducts_[{typeIndexI, typeIndexJ}];
}

// Return pre-factor for types i and j
double CGXRayWeights::preFactor(int typeIndexI, int typeIndexJ) const { return preFactors_[{typeIndexI, typeIndexJ}]; }

// Return form factor for type i over supplied Q values
std::vector<double> CGXRayWeights::formFactor(int typeIndexI, const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> fiq(Q.size());

    std::transform(Q.begin(), Q.end(), fiq.begin(),
                   [&](const double q) { return beadMap_[typeIndexI].xRayFormFactorMagnitude(q); });

    return fiq;
}

// Return form factor product for types i and j at specified Q value
double CGXRayWeights::formFactorProduct(int typeIndexI, int typeIndexJ, double Q) const
{
    //return formFactorData_[typeIndexI].get().magnitude(Q) * formFactorData_[typeIndexJ].get().magnitude(Q);
    return beadMap_[typeIndexI].xRayFormFactorMagnitude(Q) * beadMap_[typeIndexJ].xRayFormFactorMagnitude(Q);
}

// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) at specified Q value
double CGXRayWeights::weight(int typeIndexI, int typeIndexJ, double Q) const
{
    return preFactors_[{typeIndexI, typeIndexJ}] * formFactorProduct(typeIndexI, typeIndexJ, Q);
}

// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) over supplied Q values
std::vector<double> CGXRayWeights::weight(int typeIndexI, int typeIndexJ, const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> fijq(Q.size());

    //auto &fi = formFactorData_[typeIndexI].get();
    //auto &fj = formFactorData_[typeIndexJ].get();
    const CGBead &bi = beadMap_[typeIndexI];
    const CGBead &bj = beadMap_[typeIndexJ];
    auto preFactor = preFactors_[{typeIndexI, typeIndexJ}];

    std::transform(Q.begin(), Q.end(), fijq.begin(), [preFactor, &bi, &bj](auto q)
                   { return bi.xRayFormFactorMagnitude(q) * bj.xRayFormFactorMagnitude(q) * preFactor; });

    return fijq;
}

// Calculate and return Q-dependent average squared scattering (<b>**2) for supplied Q value
double CGXRayWeights::boundCoherentSquareOfAverage(double Q) const
{
    auto result = std::inner_product(concentrations_.begin(), concentrations_.end(), beadMap_.begin(), 0.0, std::plus<>(),
                                     [Q](auto con, auto bead) { return con * bead.xRayFormFactorMagnitude(Q); });
    return result * result;
}

// Calculate and return Q-dependent average squared scattering (<b>**2) for supplied Q values
std::vector<double> CGXRayWeights::boundCoherentSquareOfAverage(const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> bbar(Q.size(), 0.0);

    for (auto typeI = 0; typeI < atomTypeMix_.nItems(); ++typeI)
    {
        const double ci = concentrations_[typeI];
        //auto &fi = formFactorData_[typeI].get();
        const CGBead &bi = beadMap_[typeI];

        std::transform(Q.begin(), Q.end(), bbar.begin(), bbar.begin(),
                       [ci, &bi](auto q, auto b) { return b + ci * bi.xRayFormFactorMagnitude(q); });
    }

    // Square the averages
    std::transform(bbar.begin(), bbar.end(), bbar.begin(), [](auto b) { return b * b; });

    return bbar;
}

// Calculate and return Q-dependent squared average scattering (<b**2>) for supplied Q value
double CGXRayWeights::boundCoherentAverageOfSquares(double Q) const
{
    return std::inner_product(concentrations_.begin(), concentrations_.end(), beadMap_.begin(), 0.0, std::plus<>(),
                              [Q](auto con, auto bead)
                              { return con * bead.xRayFormFactorMagnitude(Q) * bead.xRayFormFactorMagnitude(Q); });
}

// Calculate and return Q-dependent squared average scattering (<b**2>) for supplied Q values
std::vector<double> CGXRayWeights::boundCoherentAverageOfSquares(const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> bbar(Q.size(), 0.0);

    for (auto typeI = 0; typeI < atomTypeMix_.nItems(); ++typeI)
    {
        const double ci = concentrations_[typeI];
        auto &bi = beadMap_[typeI];

        std::transform(Q.begin(), Q.end(), bbar.begin(), bbar.begin(),
                       [&](auto q, auto b) { return b + ci * bi.xRayFormFactorMagnitude(q) * bi.xRayFormFactorMagnitude(q); });
    }

    return bbar;
}

// Return whether the structure is valid (i.e. has been finalised)
bool CGXRayWeights::isValid() const { return valid_; }

/*
 * Serialisation
 */

// Read data through specified LineParser
bool CGXRayWeights::deserialise(LineParser &parser, const CoreData &coreData)
{
    clear();

    // Read form factor dataset to use
    if (parser.getArgsDelim() != LineParser::Success)
        return false;
    formFactors_ = XRayFormFactors::xRayFormFactorData().enumeration(parser.argsv(0));

    // Read AtomTypeMix
    if (!atomTypeMix_.deserialise(parser, coreData))
        return false;

    return finalise(formFactors_);
}

// Write data through specified LineParser
bool CGXRayWeights::serialise(LineParser &parser) const
{
    // Write x-ray form factor dataset
    if (!parser.writeLineF("{}\n", XRayFormFactors::xRayFormFactorData().keyword(formFactors_)))
        return false;

    // Write AtomTypeMix
    if (!atomTypeMix_.serialise(parser))
        return false;

    return true;
}

void CGXRayWeights::setAtomTypes(const AtomTypeMix &atomTypes) { atomTypeMix_ = atomTypes; }

void CGXRayWeights::calculateSelfScatteringTerms() {
    std::vector<double> fractions(atomTypeMix_.nItems());
    for (int i = 0; i < atomTypeMix_.nItems(); ++i)
    {
        fractions.at(i) = atomTypeMix_[i].fraction();
    }
    beadMap_.calculateXRaySelfInteractionTerms(fractions);
}

CGBeadMap &CGXRayWeights::beadMap() { return beadMap_; }
const CGBeadMap &CGXRayWeights::beadMap() const { return beadMap_; }