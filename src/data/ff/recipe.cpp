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
    Messenger::print("Assigning atom types to species '{}' from forcefield '{}'...\n", species.name(), ff->name());
    const auto shortRangeForm = ff->shortRangeForm();
    auto nAtomTypesFailed = 0;
    std::map<SpeciesAtom *, std::reference_wrapper<const ForcefieldAtomType>> ffaMap;
    for (auto &i : species.atoms())
    {
        auto optFFAtomType = ff->determineAtomType(i);
        if (!optFFAtomType)
        {
            Messenger::error("No matching forcefield type for atom {} ({}).\n", i.index(), Elements::symbol(i.Z()));
            ++nAtomTypesFailed;
            continue;
        }

        // Check if an AtomType of the same name already exists - if it does, just use that one
        auto &ffa = optFFAtomType->get();
        auto at = i.parent()->findAtomType(ffa.name());
        if (!at)
        {
            at = i.parent()->addAtomType(i.Z(), ffa.name());
            Messenger::print("Adding atom type '{}' for atom {} ({}).\n", at->name(), i.index(), Elements::symbol(i.Z()));
        }
        else
            Messenger::print("Re-using atom type '{}' for atom {} ({}).\n", at->name(), i.index(), Elements::symbol(i.Z()));

        // Set type in the SpeciesAtom
        i.setAtomType(at);
        ffaMap.emplace(&i, ffa);

        // Copy parameters from the assigned atom type - we take only the required number for the specified shortRangeType.
        // This is to avoid copying e.g. generator data (stored after the short range parameters) and causing issues elsewhere
        std::vector<double> params;
        params.insert(params.begin(), ffa.parameters().begin(),
                      ffa.parameters().begin() + ShortRangeFunctions::forms().minArgs(shortRangeForm).value_or(0));
        at->interactionPotential().setFormAndParameters(shortRangeForm, params);
    }

    if (nAtomTypesFailed > 0)
        return false;

    // Assign intramolecular terms
    Messenger::print("Assigning intramolecular terms to species '{}' from forcefield '{}'...\n", species.name(), ff->name());

    // Get bond terms
    auto nBondsFailed = 0;
    for (auto &bond : species.bonds())
    {
        auto ffi = ffaMap.at(bond.i());
        auto ffj = ffaMap.at(bond.j());

        auto optBondTerm = ff->getBondTerm(ffi, ffj, bond);
        if (!optBondTerm)
        {
            Messenger::error("Failed to locate parameters for bond {}-{} ({}-{}).\n", bond.i()->index(), bond.j()->index(),
                             ffi.get().equivalentName(), ffj.get().equivalentName());
            ++nBondsFailed;
            continue;
        }

        const auto &term = *optBondTerm;
        bond.setInteractionFormAndParameters(term.form(), term.parameters());
    }

    // Get angle terms
    auto nAnglesFailed = 0;
    for (auto &angle : species.angles())
    {
        auto ffi = ffaMap.at(angle.i());
        auto ffj = ffaMap.at(angle.j());
        auto ffk = ffaMap.at(angle.k());

        auto optAngleTerm = ff->getAngleTerm(ffi, ffj, ffk, angle);
        if (!optAngleTerm)
        {
            Messenger::error("Failed to locate parameters for angle {}-{}-{} ({}-{}-{}).\n", angle.i()->index(),
                             angle.j()->index(), angle.k()->index(), ffi.get().equivalentName(), ffj.get().equivalentName(),
                             ffk.get().equivalentName());

            ++nAnglesFailed;
            continue;
        }

        const auto &term = *optAngleTerm;
        angle.setInteractionFormAndParameters(term.form(), term.parameters());
    }

    // Get torsion terms
    auto nTorsionsFailed = 0;
    for (auto &torsion : species.torsions())
    {
        auto ffi = ffaMap.at(torsion.i());
        auto ffj = ffaMap.at(torsion.j());
        auto ffk = ffaMap.at(torsion.k());
        auto ffl = ffaMap.at(torsion.l());

        auto optTorsionTerm = ff->getTorsionTerm(ffi, ffj, ffk, ffl, torsion);
        if (!optTorsionTerm)
        {
            Messenger::error("Failed to locate parameters for torsion {}-{}-{} ({}-{}-{}).\n", torsion.i()->index(),
                             torsion.j()->index(), torsion.k()->index(), ffi.get().equivalentName(), ffj.get().equivalentName(),
                             ffk.get().equivalentName());

            ++nTorsionsFailed;
            continue;
        }

        const auto &term = *optTorsionTerm;
        torsion.setInteractionFormAndParameters(term.form(), term.parameters());
    }

    if (nBondsFailed > 0 || nAnglesFailed > 0 || nTorsionsFailed > 0)
        return false;

    // Generate improper terms
    // Loop over potential improper sites in the Species and see if any match terms in the forcefield
    for (auto &i : species.atoms())
    {
        // If we don't have exactly three bonds to the central atom 'i', move on
        if (i.bonds().size() != 3)
            continue;

        auto ffi = ffaMap.at(&i);

        // Get SpeciesAtom 'j'
        auto *j = i.bonds().front()->partner(&i);
        auto ffj = ffaMap.at(j);

        for (auto indexK = 1; indexK < 2; ++indexK)
        {
            // Get SpeciesAtom 'k'
            auto *k = i.bonds()[indexK]->partner(&i);
            auto ffk = ffaMap.at(k);

            for (auto indexL = indexK + 1; indexL < 3; ++indexL)
            {
                // Get SpeciesAtom 'l'
                auto *l = i.bonds()[indexL]->partner(&i);
                auto ffl = ffaMap.at(l);

                // See if this improper term exists in the forcefield
                auto optImproperTerm = ff->getImproperTerm(ffi, ffj, ffk, ffl);
                if (optImproperTerm)
                {
                    auto &improper = species.addImproper(&i, j, k, l);

                    improper.setInteractionFormAndParameters(optImproperTerm->form(), optImproperTerm->parameters());
                }
            }
        }
    }

    // Re-assign common terms?
    if (commonTerms != CommonTermRecipe::None)
        species.reduceToCommonTerms();

    // TODO DISSOLVE2
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