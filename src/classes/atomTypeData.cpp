// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include <utility>

#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/atomTypeMix.h"
#include "classes/isotopeData.h"
#include "data/isotopes.h"

AtomTypeData::AtomTypeData(std::shared_ptr<AtomType> type, double population, double fraction, double boundCoherent, int nIso)
    : atomType_(std::move(type)), exchangeable_(false), population_(population), fraction_(fraction),
      boundCoherent_(boundCoherent)
{
    isotopes_.resize(nIso, IsotopeData());
}

AtomTypeData::AtomTypeData(const AtomTypeData &source) : listIndex_(source.listIndex()), atomType_(source.atomType_)
{
    (*this) = source;
}

AtomTypeData::AtomTypeData(int listIndex, std::shared_ptr<AtomType> type, double population)
    : listIndex_(listIndex), atomType_(std::move(type)), exchangeable_(false), population_(population), fraction_(0.0),
      boundCoherent_(0.0)
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
void AtomTypeData::add(Sears91::Isotope tope, double nAdd)
{
    // Has this isotope already been added to the list?
    auto it =
        std::find_if(isotopes_.begin(), isotopes_.end(), [tope](const auto &topeData) { return topeData.isotope() == tope; });
    if (it == isotopes_.end())
        isotopes_.emplace_back(tope, nAdd);
    else
        it->add(nAdd);

    // Increase total population
    population_ += nAdd;
}

// Add/set full isotope data
void AtomTypeData::setIsotope(Sears91::Isotope tope, double pop, double fraction)
{
    if (std::find_if(isotopes_.begin(), isotopes_.end(), [tope](const auto &topeData) { return topeData.isotope() == tope; }) !=
        isotopes_.end())
        throw(std::runtime_error(fmt::format(
            "Tried to set IsotopeData for isotope A = {} in AtomTypeData for AtomType '{}', but existing data is present.\n",
            tope, Sears91::A(tope), atomTypeName())));

    isotopes_.emplace_back(tope, pop, fraction);

    population_ += pop;
}

// Zero populations
void AtomTypeData::zeroPopulations()
{
    // Zero individual isotope counts
    for (auto &topeData : isotopes_)
        topeData.zeroPopulation();

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
    for (auto &topeData : isotopes_)
        topeData.finalise(population_);

    // Determine bound coherent scattering for AtomType, based on Isotope populations
    boundCoherent_ = 0.0;
    for (auto &topeData : isotopes_)
        boundCoherent_ += topeData.fraction() * Sears91::boundCoherent(topeData.isotope());
}

// Remove any existing isotopes, and add only the natural isotope
void AtomTypeData::naturalise()
{
    // Clear the isotopes list and add on the natural isotope, keeping the current population
    isotopes_.clear();
    auto &topeData = isotopes_.emplace_back(Sears91::naturalIsotope(atomType_->Z()), population_);
    topeData.finalise(population_);
    boundCoherent_ = Sears91::boundCoherent(topeData.isotope());
}

// Return the number of defined Isotopes
int AtomTypeData::nIsotopes() const { return isotopes_.size(); }

// Return if specified Isotope is already in the list
bool AtomTypeData::hasIsotope(Sears91::Isotope tope) const
{
    return std::find_if(isotopes_.begin(), isotopes_.end(),
                        [tope](const auto &topeData) { return topeData.isotope() == tope; }) != isotopes_.end();
}

// Return IsotopeData vector
const std::vector<IsotopeData> &AtomTypeData::isotopeData() const { return isotopes_; };

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
