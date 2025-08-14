// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include <utility>

#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/atomTypeMix.h"
#include "data/isotopes.h"

AtomTypeData::AtomTypeData(const AtomType *atomType, double population, double fraction, double boundCoherent)
    : atomType_(atomType), population_(population), fraction_(fraction), boundCoherent_(boundCoherent)
{
}

/*
 * Properties
 */

// Add to population
void AtomTypeData::add(double nAdd) { population_ += nAdd; }

// Add to population of Isotope
void AtomTypeData::add(Sears91::Isotope isotope, double population)
{
    if (isotopes_.contains(isotope))
        isotopes_[isotope] += population;
    else
        isotopes_[isotope] = population;

    // Increase total population
    population_ += population;
}

// Return reference AtomType
const AtomType *AtomTypeData::atomType() const { return atomType_; }

// Set exchangeable flag
void AtomTypeData::setAsExchangeable() { exchangeable_ = true; }

// Return whether the associated AtomType is exchangeable
bool AtomTypeData::exchangeable() const { return exchangeable_; }

// Finalise, calculating fractional populations etc.
void AtomTypeData::finalise(double totalAtoms)
{
    // Calculate fractional world population
    fraction_ = population_ / totalAtoms;

    // Determine bound coherent scattering for AtomType, based on Isotope populations
    boundCoherent_ = 0.0;
    for (auto &[isotope, isotopePopulation] : isotopes_)
        boundCoherent_ += (isotopePopulation / population_) * Sears91::boundCoherent(isotope);
}

// Return isotopes map
const std::map<Sears91::Isotope, double> &AtomTypeData::isotopes() const { return isotopes_; };

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
