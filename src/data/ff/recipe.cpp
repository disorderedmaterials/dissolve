// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/recipe.h"
#include "classes/species.h"
#include "data/ff/ff.h"

// Apply to species
bool ForcefieldRecipe::apply(Species &species) const
{
    if (!ff)
        return Messenger::error("No forcefield supplied!\n");

    // Clear any existing forcefield data from the species
    species.clearIntramolecularForcefieldTerms();
    species.clearAtomTypes();
    if (commonTerms != AddAndReuse)
        species.clearCommonTerms();

    // Assign atom types to the species
    // if (!ff->assignAtomTypes(this, Forcefield::TypeAll, false).empty())
    // return false;

    // Assign intramolecular terms
    // FROM SPECIES
    // if (!ff->assignIntramolecular(this))
    // return false;

    // FROM AddForcefieldDialogModel
    // Detach any MasterTerm references, and delete the MasterTerms
    // modifiedSpecies_->detachFromCommonTerms();
    // temporaryCoreData_.clearMasterTerms();
    // TODO DISSOLVE2

    // Assign intramolecular terms
    // if (intramolecularRadio_ != Radio::None)
    // {
    //     auto flags = 0;
    //     if (ignoreCurrentTypes_)
    //         flags += Forcefield::DetermineTypesFlag;
    //     if (!noImproperTerms_)
    //         flags += Forcefield::GenerateImpropersFlag;
    //     if (intramolecularRadio_ == Radio::Selected)
    //         flags += Forcefield::SelectionOnlyFlag;
    //
    //     // Try to assign terms across the species
    //     if (!ff->assignIntramolecular(modifiedSpecies_.get(), flags))
    //         return;
    //
    //     // Reduce to master terms?
    //     // TODO DISSOLVE2
    //     // if (!noMasterTerms_)
    //     // modifiedSpecies_->reduceToCommonTerms(temporaryCoreData_, intramolecularRadio_ == Radio::Selected);
    // }

    // FROM AddFOrcefieldDialogModel
    // auto &currentOverrides = dissolve_->coreData().pairPotentialOverrides();
    // for (const auto &ffOverride : ff_->pairPotentialOverrides())
    // {
    //     auto it = std::find_if(
    //         currentOverrides.begin(), currentOverrides.end(),
    //         [&](const auto &ppOverride)
    //         {
    //             return (ppOverride->matchI() == ffOverride.matchI() && ppOverride.get()->matchJ() == ffOverride.matchJ()) ||
    //                    (ppOverride->matchI() == ffOverride.matchJ() && ppOverride.get()->matchJ() == ffOverride.matchI());
    //         });
    //     if (it == currentOverrides.end())
    //     {
    //         dissolve_->coreData().addPairPotentialOverride(ffOverride.matchI(), ffOverride.matchJ(), ffOverride.type(),
    //                                                        ffOverride.interactionPotential());
    //     }
    // }
    return true;
}