// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/atomTypeMix.h"
#include "data/isotopes.h"
#include <numeric>

// Add to population of Isotope
void AtomTypeData::add(Sears91::Isotope isotope, double population)
{
    if (isotopes_.contains(isotope))
        isotopes_[isotope] += population;
    else
        isotopes_[isotope] = population;
}

// Finalise, calculating fractional populations etc.
void AtomTypeData::finalise()
{
    // Determine bound coherent scattering for AtomType, based on Isotope populations
    boundCoherent_ = std::accumulate(isotopes_.begin(), isotopes_.end(), 0.0,
                                     [](const auto acc, const auto &isotope)
                                     { return acc + isotope.second * Sears91::boundCoherent(isotope.first); }) /
                     std::accumulate(isotopes_.begin(), isotopes_.end(), 0.0,
                                     [](const auto acc, const auto &isotope) { return acc + isotope.second; });
}

// Return isotopes map
const std::map<Sears91::Isotope, double> &AtomTypeData::isotopes() const { return isotopes_; };

// Set calculated bond coherent scattering over all isotopes
void AtomTypeData::setBoundCoherent(double d) { boundCoherent_ = d; }

// Calculated bound coherent scattering over all Isotopes
double AtomTypeData::boundCoherent() const { return boundCoherent_; }
