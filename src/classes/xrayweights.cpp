// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/xrayweights.h"
#include "base/lineparser.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "classes/speciesinfo.h"
#include "templates/algorithms.h"
#include <functional>
#include <numeric>

XRayWeights::XRayWeights() : formFactors_(XRayFormFactors::NoFormFactorData), valid_(false) {}

XRayWeights::XRayWeights(const XRayWeights &source) { (*this) = source; }

void XRayWeights::operator=(const XRayWeights &source)
{
    formFactors_ = source.formFactors_;
    atomTypes_ = source.atomTypes_;
    concentrations_ = source.concentrations_;
    concentrationProducts_ = source.concentrationProducts_;
    formFactorData_ = source.formFactorData_;
    preFactors_ = source.preFactors_;
    valid_ = source.valid_;
}

/*
 * Source AtomTypes
 */

// Initialise form factor data for the current atom types
bool XRayWeights::initialiseFormFactors()
{
    formFactorData_.clear();

    for (auto &atd : atomTypes_)
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
void XRayWeights::clear()
{
    atomTypes_.clear();
    concentrations_.clear();
    concentrationProducts_.clear();
    preFactors_.clear();
    valid_ = false;
}

// Set-up from supplied SpeciesInfo list
bool XRayWeights::setUp(std::vector<SpeciesInfo> &speciesInfoList, XRayFormFactors::XRayFormFactorData formFactors)
{
    valid_ = false;

    // Fill atomTypes_ list with AtomType populations, based on Isotopologues relative populations and associated Species
    // populations
    atomTypes_.clear();
    for (auto &spInfo : speciesInfoList)
    {
        const Species *sp = spInfo.species();

        // Loop over Atoms in the Species
        for (const auto &i : sp->atoms())
            atomTypes_.add(i.atomType(), spInfo.population());
    }

    // Perform final setup based on now-completed atomtypes list
    return finalise(formFactors);
}

// Add Species to weights in the specified population
void XRayWeights::addSpecies(const Species *sp, int population)
{
    for (const auto &i : sp->atoms())
        atomTypes_.add(i.atomType(), population);

    valid_ = false;
}

// Finalise weights after addition of all individual Species
bool XRayWeights::finalise(XRayFormFactors::XRayFormFactorData formFactors)
{
    valid_ = false;

    atomTypes_.finalise();

    // Retrieve form factor data for the current atom types
    formFactors_ = formFactors;
    if (!initialiseFormFactors())
        return false;

    setUpMatrices();

    valid_ = true;

    return true;
}

// Return X-Ray form factors being used
XRayFormFactors::XRayFormFactorData XRayWeights::formFactors() const { return formFactors_; }

// Return AtomTypeList
const AtomTypeList &XRayWeights::atomTypes() const { return atomTypes_; }

// Return number of used AtomTypes
int XRayWeights::nUsedTypes() const { return atomTypes_.nItems(); }

// Print atomtype information
void XRayWeights::print() const
{
    // Print atomtypes table
    Messenger::print("\n");
    atomTypes_.print();
}

/*
 * Data
 */

// Set up matrices based on current AtomType information
void XRayWeights::setUpMatrices()
{
    concentrations_.clear();
    concentrations_.resize(atomTypes_.nItems());
    concentrationProducts_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
    preFactors_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);

    // Determine atomic concentration products and full pre-factor
    for_each_pair(atomTypes_.begin(), atomTypes_.end(),
                  [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2) {
                      double ci = atd1.fraction();
                      concentrations_.at(typeI) = ci;

                      double cj = atd2.fraction();

                      concentrationProducts_[{typeI, typeJ}] = ci * cj;
                      preFactors_[{typeI, typeJ}] = ci * cj * (typeI == typeJ ? 1 : 2);
                  });
}

// Return concentration product for type i
double XRayWeights::concentration(int typeIndexI) const { return concentrations_[typeIndexI]; }

// Return concentration product for types i and j
double XRayWeights::concentrationProduct(int typeIndexI, int typeIndexJ) const
{
    return concentrationProducts_[{typeIndexI, typeIndexJ}];
}

// Return pre-factor for types i and j
double XRayWeights::preFactor(int typeIndexI, int typeIndexJ) const { return preFactors_[{typeIndexI, typeIndexJ}]; }

// Return form factor for type i over supplied Q values
std::vector<double> XRayWeights::formFactor(int typeIndexI, const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> fiq(Q.size());

    auto &fi = formFactorData_[typeIndexI].get();

    for (auto n = 0; n < Q.size(); ++n)
        fiq[n] = fi.magnitude(Q[n]);

    return fiq;
}

// Return form factor product for types i and j at specified Q value
double XRayWeights::formFactorProduct(int typeIndexI, int typeIndexJ, double Q) const
{
    return formFactorData_[typeIndexI].get().magnitude(Q) * formFactorData_[typeIndexJ].get().magnitude(Q);
}

// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) at specified Q value
double XRayWeights::weight(int typeIndexI, int typeIndexJ, double Q) const
{
    return preFactors_[{typeIndexI, typeIndexJ}] * formFactorProduct(typeIndexI, typeIndexJ, Q);
}

// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) over supplied Q values
std::vector<double> XRayWeights::weight(int typeIndexI, int typeIndexJ, const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> fijq(Q.size());

    auto &fi = formFactorData_[typeIndexI].get();
    auto &fj = formFactorData_[typeIndexJ].get();
    auto preFactor = preFactors_[{typeIndexI, typeIndexJ}];

    std::transform(Q.begin(), Q.end(), fijq.begin(),
                   [preFactor, &fi, &fj](auto q) { return fi.magnitude(q) * fj.magnitude(q) * preFactor; });

    return fijq;
}

// Calculate and return Q-dependent average squared scattering (<b>**2) for supplied Q value
double XRayWeights::boundCoherentSquareOfAverage(double Q) const
{
    auto result = std::inner_product(concentrations_.begin(), concentrations_.end(), formFactorData_.begin(), 0.0,
                                     std::plus<>(), [Q](auto con, auto form) { return con * form.get().magnitude(Q); });
    return result * result;
}

// Calculate and return Q-dependent average squared scattering (<b>**2) for supplied Q values
std::vector<double> XRayWeights::boundCoherentSquareOfAverage(const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> bbar(Q.size());

    for (auto typeI = 0; typeI < atomTypes_.nItems(); ++typeI)
    {
        const double ci = concentrations_[typeI];
        auto &fi = formFactorData_[typeI].get();

        std::transform(Q.begin(), Q.end(), bbar.begin(), [ci, &fi](auto q) { return ci * fi.magnitude(q); });
    }

    // Square the averages
    std::transform(bbar.begin(), bbar.end(), bbar.begin(), [](auto b) { return b * b; });

    return bbar;
}

// Calculate and return Q-dependent squared average scattering (<b**2>) for supplied Q value
double XRayWeights::boundCoherentAverageOfSquares(double Q) const
{
    return std::inner_product(concentrations_.begin(), concentrations_.end(), formFactorData_.begin(), 0.0, std::plus<>(),
                              [Q](auto con, auto form) { return con * form.get().magnitude(Q) * form.get().magnitude(Q); });
}

// Calculate and return Q-dependent squared average scattering (<b**2>) for supplied Q values
std::vector<double> XRayWeights::boundCoherentAverageOfSquares(const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> bbar(Q.size());

    for (auto typeI = 0; typeI < atomTypes_.nItems(); ++typeI)
    {
        const double ci = concentrations_[typeI];
        auto &fi = formFactorData_[typeI].get();

        std::transform(Q.begin(), Q.end(), bbar.begin(), bbar.begin(),
                       [&](auto q, auto b) { return b + ci * fi.magnitude(q) * fi.magnitude(q); });
    }

    return bbar;
}

// Return whether the structure is valid (i.e. has been finalised)
bool XRayWeights::isValid() const { return valid_; }

/*
 * Serialisation
 */

// Read data through specified LineParser
bool XRayWeights::deserialise(LineParser &parser, const CoreData &coreData)
{
    clear();

    // Read form factor dataset to use
    if (!parser.getArgsDelim())
        return false;
    formFactors_ = XRayFormFactors::xRayFormFactorData().enumeration(parser.argsv(0));

    // Read AtomTypeList
    if (!atomTypes_.deserialise(parser, coreData))
        return false;

    return finalise(formFactors_);
}

// Write data through specified LineParser
bool XRayWeights::serialise(LineParser &parser) const
{
    // Write x-ray form factor dataset
    if (!parser.writeLineF("{}\n", XRayFormFactors::xRayFormFactorData().keyword(formFactors_)))
        return false;

    // Write AtomTypeList
    if (!atomTypes_.serialise(parser))
        return false;

    return true;
}
