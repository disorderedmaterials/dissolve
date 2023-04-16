// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "kernels/potentials/base.h"

AdditionalPotential::AdditionalPotential() {}

/*
 * Keywords
 */

// Return keywords for this potential
KeywordStore &ExternalPotential::keywords() { return keywords_; }
const KeywordStore &ExternalPotential::keywords() const { return keywords_; }

/*
 * Potential Calculation
 */

// Calculate energy on specified atom
double ExternalPotential::energy(const Atom &i, const Box *box) const { return 0.0; }

// Calculate force on specified atom, summing in to supplied vector
void ExternalPotential::force(const Atom &i, const Box *box, Vec3<double> &f) const {}

/*
 * Read / Write
 */

// Read data from specified LineParser
bool AdditionalPotential::deserialise(LineParser &parser, const CoreData &coreData) {}

// Write data to specified LineParser
bool AdditionalPotential::serialise(LineParser &parser, std::string_view prefix) const {}
