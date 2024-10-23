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

/*
 * Set of Partials
 */

// Set up PartialSet
bool PotentialSet::setUp(const AtomTypeMix &atomTypeMix, double rdfRange, double binWidth)
{
    // Set up partial arrays
    if (!setUpPotentials(atomTypeMix))
        return false;

    fingerprint_ = "NO_FINGERPRINT";

    return true;
}

// Set up PartialSet without initialising histogram arrays
bool PotentialSet::setUpPotentials(const AtomTypeMix &atomTypeMix)
{

    fingerprint_ = "NO_FINGERPRINT";

    return true;
}

// Reset partial arrays
void PotentialSet::reset()
{
    potentials_.clear();
    fingerprint_ = "NO_FINGERPRINT";
}

// Set new fingerprint
void PotentialSet::setFingerprint(std::string_view fingerprint) { fingerprint_ = fingerprint; }

// Return full atom-atom partial specified
std::map<std::string, PotentialSet::EPData> &PotentialSet::potential() { return potentials_; }
const std::map<std::string, PotentialSet::EPData> &PotentialSet::potential() const { return potentials_; }

/*
 * Operators
 */

void PotentialSet::operator+=(const double delta) {}

void PotentialSet::operator+=(const PotentialSet &source) {}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool PotentialSet::deserialise(LineParser &parser, const CoreData &coreData)
{
    if (parser.readNextLine(LineParser::Defaults, fingerprint_) != LineParser::Success)
        return false;

    return true;
}

// Write data through specified LineParser
bool PotentialSet::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("{}\n", fingerprint_))
        return false;

    return true;
}
