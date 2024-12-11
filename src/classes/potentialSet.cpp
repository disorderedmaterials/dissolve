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

// Reset partial arrays
void PotentialSet::reset()
{
    potentials_.clear();
    fingerprint_ = "NO_FINGERPRINT";
}

// Set new fingerprint
void PotentialSet::setFingerprint(std::string_view fingerprint) { fingerprint_ = fingerprint; }

// Return full map of potentials specified
std::map<std::string, PotentialSet::EPData> &PotentialSet::potentialMap() { return potentials_; }
const std::map<std::string, PotentialSet::EPData> &PotentialSet::potentialMap() const { return potentials_; }

/*
 * Operators
 */

PotentialSet &PotentialSet::operator+=(const double delta)
{
    for (auto &[key, potential] : potentials_)
        potential.ep += delta;
    return (*this);
}

PotentialSet &PotentialSet::operator+=(const PotentialSet &source)
{
    for (auto &[key, potential] : source.potentialMap())
    {
        auto it = potentials_.find(key);
        if (it != potentials_.end())
            it->second.ep += potential.ep;
        else
            potentials_[key] = potential;
    }
    return (*this);
}

PotentialSet &PotentialSet::operator*=(const double factor)
{
    for (auto &[key, potential] : potentials_)
        potential.ep *= factor;
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
        EPData value;
        auto key = parser.args(0);
        value.count = parser.argi(1);
        value.at1 = coreData.findAtomType(parser.args(2));
        value.at2 = coreData.findAtomType(parser.args(3));

        if (!value.ep.deserialise(parser))
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
        if (!value.ep.serialise(parser))
            return false;
    }
    return true;
}
