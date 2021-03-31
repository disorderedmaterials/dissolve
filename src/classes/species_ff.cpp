// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/species.h"
#include "data/ff/ff.h"

/*
 * Public
 */

// Set Forcefield to source terms from
void Species::setForcefield(std::shared_ptr<Forcefield> ff) { forcefield_ = ff; }

// Return Forcefield to source terms from
std::shared_ptr<Forcefield> Species::forcefield() const { return forcefield_; }

// Set whether to auto-generate missing intramolecular terms, and remove invalid ones
void Species::setAutoUpdateIntramolecularTerms(bool b)
{
    autoUpdateIntramolecularTerms_ = b;

    if (autoUpdateIntramolecularTerms_)
        updateIntramolecularTerms();
}

// Return whether to auto-generate missing intramolecular terms, and remove invalid ones
bool Species::autoUpdateIntramolecularTerms() const { return autoUpdateIntramolecularTerms_; }

// Apply terms from source Forcefield
bool Species::applyForcefieldTerms(CoreData &coreData)
{
    if (!forcefield_)
        return Messenger::error("No forcefield set in Species '{}', so can't apply terms.\n", name());

    // Assign atom types to the species
    if (forcefield_->assignAtomTypes(this, coreData, Forcefield::TypeAll, false) != 0)
        return false;

    // Assign intramolecular terms
    if (!forcefield_->assignIntramolecular(this))
        return false;

    return true;
}
