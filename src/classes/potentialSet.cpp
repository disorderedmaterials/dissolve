// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/potentialSet.h"
#include "base/lineParser.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "io/export/data1D.h"
#include "items/deserialisers.h"
#include "items/serialisers.h"
#include "templates/algorithms.h"

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

// Return full map of potentials specified
std::map<std::string, PotentialSet::PotentialData> &PotentialSet::potentialMap() { return potentials_; }
const std::map<std::string, PotentialSet::PotentialData> &PotentialSet::potentialMap() const { return potentials_; }

/*
 * Operators
 */

PotentialSet &PotentialSet::operator+=(const double delta)
{
    for (auto &[key, pot] : potentials_)
        pot.potential += delta;
    return (*this);
}

PotentialSet &PotentialSet::operator+=(const PotentialSet &source)
{
    for (auto &[key, pot] : source.potentialMap())
    {
        auto it = potentials_.find(key);
        if (it != potentials_.end())
            it->second.potential += pot.potential;
        else
            potentials_[key] = pot;
    }
    return (*this);
}

PotentialSet &PotentialSet::operator*=(const double factor)
{
    for (auto &[key, pot] : potentials_)
        pot.potential *= factor;
    return (*this);
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool PotentialSet::deserialise(LineParser &parser, const CoreData &coreData)
{
    if (parser.readNextLine(LineParser::Defaults, fingerprint_) != LineParser::Success)
        return false;

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto size = parser.argli(0);
    for (auto n = 0; n < size; ++n)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        PotentialData value;
        auto key = parser.args(0);
        value.count = parser.argi(1);
        value.at1 = coreData.findAtomType(parser.args(2));
        value.at2 = coreData.findAtomType(parser.args(3));

        if (!value.potential.deserialise(parser))
            return false;

        potentials_[key] = value;
    }

    return true;
}

// Write data through specified LineParser
bool PotentialSet::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("{}\n", fingerprint_))
        return false;
    if (!parser.writeLineF("{}\n", potentials_.size()))
        return false;
    for (auto &[key, value] : potentials_)
    {
        if (!parser.writeLineF("{} {} {} {}\n", key, value.count, value.at1->name(), value.at2->name()))
            return false;
        if (!value.potential.serialise(parser))
            return false;
    }
    return true;
}
