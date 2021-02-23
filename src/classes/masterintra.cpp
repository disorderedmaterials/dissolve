// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/masterintra.h"
#include "base/messenger.h"
#include "classes/atom.h"

MasterIntra::MasterIntra(const std::string_view name, const SpeciesIntra::InteractionType type)
    : SpeciesIntra(), name_(name), type_(type)
{
}

/*
 * Basic Data
 */

// Set name for interaction (if relevant)
void MasterIntra::setName(std::string_view name) { name_ = name; }

// Return name for interaction
std::string_view MasterIntra::name() const { return name_; }

/*
 * Interaction Parameters
 */

// Set up any necessary parameters
void MasterIntra::setUp() {}

// Calculate and return fundamental frequency for the interaction
double MasterIntra::fundamentalFrequency(double reducedMass) const
{
    Messenger::warn("A MasterIntra can't return a fundamental frequency, so don't ask for one!\n");
    return 0.0;
}

// Return type of this interaction
SpeciesIntra::InteractionType MasterIntra::type() const { return type_; }
