// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include <utility>

#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/species.h"
#include "data/ff/ff.h"

/*
 * Public
 */

// Set Forcefield to source terms from
void Species::setForcefield(std::shared_ptr<Forcefield> ff) { forcefield_ = std::move(ff); }

// Return Forcefield to source terms from
std::shared_ptr<Forcefield> Species::forcefield() const { return forcefield_; }

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

// Clear forcefield terms
void Species::clearForcefieldTerms(bool nullifyAtomTypes)
{
    if (nullifyAtomTypes)
        clearAtomTypes();

    for (auto &b : bonds_)
        b.setFormAndParameters(BondFunctions::Form::None, std::vector<double>());

    for (auto &a : angles_)
        a.setFormAndParameters(AngleFunctions::Form::None, std::vector<double>());

    for (auto &t : torsions_)
        t.setFormAndParameters(TorsionFunctions::Form::None, std::vector<double>());

    impropers_.clear();
}
