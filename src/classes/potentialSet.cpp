// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/potentialSet.h"
#include "base/lineParser.h"
#include "classes/atomType.h"
#include "classes/configuration.h"
#include "items/deserialisers.h"

PotentialSet::PotentialSet() { fingerprint_ = "NO_FINGERPRINT"; }

PotentialSet::~PotentialSet() { potentials_.clear(); }

// Reset Potentials
void PotentialSet::reset()
{
    potentials_.clear();
    fingerprint_ = "NO_FINGERPRINT";
}

// Set new fingerprint
void PotentialSet::setFingerprint(std::string_view fingerprint) { fingerprint_ = fingerprint; }

// Return full set of potentials
DoubleKeyedMap<Data1D> &PotentialSet::potentials() { return potentials_; }
const DoubleKeyedMap<Data1D> &PotentialSet::potentials() const { return potentials_; }

/*
 * Operators
 */

PotentialSet &PotentialSet::operator+=(const double delta)
{
    for (auto &pot : std::views::values(potentials_))
        pot += delta;
    return (*this);
}

PotentialSet &PotentialSet::operator+=(const PotentialSet &source)
{
    for (auto &[key, pot] : source.potentials_)
        potentials_.map()[key] += pot;

    return (*this);
}

PotentialSet PotentialSet::operator*(double factor) const
{
    auto result = *this;
    result *= factor;
    return result;
}

PotentialSet &PotentialSet::operator*=(const double factor)
{
    for (auto &pot : std::views::values(potentials_))
        pot *= factor;
    return (*this);
}

/*
 * Serialisation
 */

// Express as a serialisable value
void PotentialSet::serialise(std::string tag, SerialisedValue &target) const
{
    auto &result = target[tag];

    potentials_.serialise("potentials", result);
}

// Read values from a serialisable value
void PotentialSet::deserialise(SerialisedValue node) { potentials_.deserialise(node["potentials"]); }