// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/atomtypelist.h"
#include "classes/isotopedata.h"
#include "data/isotopes.h"

AtomTypeData::AtomTypeData(std::shared_ptr<AtomType> type, double population, double fraction, double boundCoherent, int nIso)
    : atomType_(type), exchangeable_(false), population_(population), fraction_(fraction), boundCoherent_(boundCoherent)
{
    isotopes_.clear();
    for (auto n = 0; n < nIso; ++n)
        isotopes_.add();
}

AtomTypeData::AtomTypeData(const AtomTypeData &source) : listIndex_(source.listIndex()), atomType_(source.atomType_)
{
    (*this) = source;
}

AtomTypeData::AtomTypeData(int listIndex, std::shared_ptr<AtomType> type, double population)
    : listIndex_(listIndex), atomType_(type), exchangeable_(false), population_(population), fraction_(0.0), boundCoherent_(0.0)
{
}

void AtomTypeData::operator=(const AtomTypeData &source)
{
    atomType_ = source.atomType_;
    exchangeable_ = source.exchangeable_;
    isotopes_ = source.isotopes_;
    population_ = source.population_;
    fraction_ = source.fraction_;
    boundCoherent_ = source.boundCoherent_;
}

/*
 * Properties
 */

// Add to population
void AtomTypeData::add(double nAdd) { population_ += nAdd; }

// Add to population of Isotope
void AtomTypeData::add(Isotope *tope, double nAdd)
{
    // Has this isotope already been added to the list?
    IsotopeData *topeData = nullptr;
    for (topeData = isotopes_.first(); topeData != nullptr; topeData = topeData->next())
        if (topeData->isotope() == tope)
            break;
    if (topeData == nullptr)
    {
        topeData = isotopes_.add();
        topeData->initialise(tope);
    }

    // Increase Isotope population
    topeData->add(nAdd);

    // Increase total population
    population_ += nAdd;
}

// Zero populations
void AtomTypeData::zeroPopulations()
{
    // Zero individual isotope counts
    for (auto *topeData = isotopes_.first(); topeData != nullptr; topeData = topeData->next())
        topeData->zeroPopulation();

    // Zero totals
    population_ = 0.0;
    fraction_ = 0.0;
}

// Return list index of AtomTypeData in AtomTypeList
int AtomTypeData::listIndex() const { return listIndex_; }

// Return reference AtomType
std::shared_ptr<AtomType> AtomTypeData::atomType() const { return atomType_; }

// Set exchangeable flag
void AtomTypeData::setAsExchangeable() { exchangeable_ = true; }

// Return whether the associated AtomType is exchangeable
bool AtomTypeData::exchangeable() const { return exchangeable_; }

// Finalise, calculating fractional populations etc.
void AtomTypeData::finalise(double totalAtoms)
{
    // Calculate fractional world population
    fraction_ = population_ / totalAtoms;

    // Calculate isotope fractional populations (of AtomType)
    for (auto *topeData = isotopes_.first(); topeData != nullptr; topeData = topeData->next())
        topeData->finalise(population_);

    // Determine bound coherent scattering for AtomType, based on Isotope populations
    boundCoherent_ = 0.0;
    for (auto *topeData = isotopes_.first(); topeData != nullptr; topeData = topeData->next())
        boundCoherent_ += topeData->fraction() * topeData->isotope()->boundCoherent();
}

// Remove any existing isotopes, and add only the natural isotope
void AtomTypeData::naturalise()
{
    // Clear the isotopes list and add on the natural isotope, keeping the current population
    isotopes_.clear();
    IsotopeData *topeData = isotopes_.add();
    topeData->initialise(Isotopes::naturalIsotope(atomType_->Z()));
    topeData->add(population_);
    topeData->finalise(population_);
    boundCoherent_ = topeData->isotope()->boundCoherent();
}

// Return the number of defined Isotopes
int AtomTypeData::nIsotopes() const { return isotopes_.nItems(); }

// Return if specified Isotope is already in the list
bool AtomTypeData::hasIsotope(Isotope *tope)
{
    for (auto *topeData = isotopes_.first(); topeData != nullptr; topeData = topeData->next())
        if (topeData->isotope() == tope)
            return true;

    return false;
}

// Set this AtomType to have only the single Isotope provided
void AtomTypeData::setSingleIsotope(Isotope *tope)
{
    isotopes_.clear();
    IsotopeData *topeData = isotopes_.add();
    topeData->initialise(tope);
    topeData->add(population_);
    topeData->finalise(population_);
    boundCoherent_ = topeData->isotope()->boundCoherent();
}

// Return Isotope
IsotopeData *AtomTypeData::isotopeData() const { return isotopes_.first(); }

// Return total population over all isotopes
int AtomTypeData::population() const { return population_; }

// Return total fractional population including all isotopes
double AtomTypeData::fraction() const { return fraction_; }

// Set calculated bond coherent scattering over all isotopes
void AtomTypeData::setBoundCoherent(double d) { boundCoherent_ = d; }

// Calculated bound coherent scattering over all Isotopes
double AtomTypeData::boundCoherent() const { return boundCoherent_; }

// Return referenced AtomType name
std::string_view AtomTypeData::atomTypeName() const { return atomType_->name(); }

/*
 * I/O
 */

// Write data through specified LineParser
bool AtomTypeData::serialise(LineParser &parser) const
{
    // Line Contains: AtomType name, exchangeable flag, population, fraction, boundCoherent, and nIsotopes
    if (!parser.writeLineF("{} {} {} {} {}\n", atomType_->name(), population_, fraction_, boundCoherent_, isotopes_.nItems()))
        return false;
    ListIterator<IsotopeData> isotopeIterator(isotopes_);
    while (IsotopeData *topeData = isotopeIterator.iterate())
        if (!topeData->serialise(parser))
            return false;
    return true;
}
