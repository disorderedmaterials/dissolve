// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "modules/checkspecies/checkspecies.h"
#include <numeric>

// Run main processing
bool CheckSpeciesModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Retrieve necessary keyword values
    Species *sp = keywords_.retrieve<Species *>("Species");
    if (!sp)
        return Messenger::error("No target species provided.\n");
    const auto tolerance = keywords_.asDouble("Tolerance");

    Messenger::print("CheckSpecies: Target species is '{}'.\n", sp->name());
    Messenger::print("CheckSpecies: Tolerance for parameter checks is {:.5e}.", tolerance);

    // Check atom types
    auto nAtomTypesFailed = 0;
    if (atomTypes_.size() != 0)
    {
        Messenger::print("\nChecking atom types...\n");

        // Each tuple should contain a single integer (the atom index) and a single string (the type name)
        for (const auto &indexName : atomTypes_)
        {
            // Get specified atom - tuple contains 'human-readable' indices from 1 - N...
            auto i = std::get<0>(indexName).at(0);
            if (i - 1 >= sp->nAtoms())
                return Messenger::error("Atom index {} is out of range ({} atoms in species).\n", i, sp->nAtoms());

            auto &spAtom = sp->atom(i - 1);
            auto at = spAtom.atomType();

            // Get type name to validate against
            std::string_view typeName = std::get<1>(indexName).at(0);
            if ((!at) || ((*at).name() != typeName))
            {
                Messenger::print("Type for atom {} is {}, which does not match that expected ({}).\n", i,
                                 at ? (*at).name() : "undefined", typeName);
                ++nAtomTypesFailed;
            }
            else
                Messenger::print("Atom {} has correct type ({}).\n", i, typeName);
        }

        if (nAtomTypesFailed == 0)
            Messenger::print("\nAll atom types validated successfully.");
        else
            Messenger::print("\n{} atoms have incorrect types.", nAtomTypesFailed);
    }

    // Check total charge
    auto nChargesFailed = 0;
    if (keywords_.isSet("TotalCharge"))
    {
        Messenger::print("\nChecking total charge...\n");

        auto q = sp->totalCharge(dissolve.pairPotentialsIncludeCoulomb());
        auto qDiff = fabs(q - keywords_.asDouble("TotalCharge"));
        if (qDiff > keywords_.asDouble("ChargeTolerance"))
        {
            ++nChargesFailed;
            Messenger::print("Total charge on species is incorrect at {} e (expected = {} e).\n", q,
                             keywords_.asDouble("TotalCharge"));
        }
        else
            Messenger::print("Total charge on species is {} e, which is within the tolerance ({:12.6e} e).\n", q,
                             keywords_.asDouble("ChargeTolerance"));
    }

    // Check bond parameters
    auto nBondsFailed = 0;
    if (bondParameters_.size() != 0)
    {
        Messenger::print("\nChecking bond parameters...\n");
        nBondsFailed =
            std::accumulate(bondParameters_.begin(), bondParameters_.end(), 0, [&](const auto &acc, const auto &bond) {
                auto &indices = std::get<0>(bond);
                return acc + parametersDiffer<SpeciesBond>("bond", sp->getBond(indices.at(0) - 1, indices.at(1) - 1), indices,
                                                           std::get<1>(bond), tolerance);
            });
    }

    // Check angle parameters
    auto nAnglesFailed = 0;
    if (angleParameters_.size() != 0)
    {
        Messenger::print("\nChecking angle parameters...\n");
        nAnglesFailed =
            std::accumulate(angleParameters_.begin(), angleParameters_.end(), 0, [&](const auto &acc, const auto &angle) {
                auto &indices = std::get<0>(angle);
                return acc + parametersDiffer<SpeciesAngle>(
                                 "angle", sp->getAngle(indices.at(0) - 1, indices.at(1) - 1, indices.at(2) - 1), indices,
                                 std::get<1>(angle), tolerance);
            });
    }

    // Check torsion parameters
    auto nTorsionsFailed = 0;
    if (torsionParameters_.size() != 0)
    {
        Messenger::print("\nChecking torsion parameters...\n");
        nTorsionsFailed =
            std::accumulate(torsionParameters_.begin(), torsionParameters_.end(), 0, [&](const auto &acc, const auto &torsion) {
                auto &indices = std::get<0>(torsion);
                return acc + parametersDiffer<SpeciesTorsion>(
                                 "torsion",
                                 sp->getTorsion(indices.at(0) - 1, indices.at(1) - 1, indices.at(2) - 1, indices.at(3) - 1),
                                 indices, std::get<1>(torsion), tolerance);
            });
    }

    // Check improper parameters
    auto nImpropersFailed = 0;
    if (improperParameters_.size() != 0)
    {
        Messenger::print("\nChecking improper parameters...\n");
        nImpropersFailed = std::accumulate(
            improperParameters_.begin(), improperParameters_.end(), 0, [&](const auto &acc, const auto &improper) {
                auto &indices = std::get<0>(improper);
                return acc + parametersDiffer<SpeciesImproper>(
                                 "improper",
                                 sp->getImproper(indices.at(0) - 1, indices.at(1) - 1, indices.at(2) - 1, indices.at(3) - 1),
                                 indices, std::get<1>(improper), tolerance);
            });
    }

    return (nAtomTypesFailed + nChargesFailed + nBondsFailed + nAnglesFailed + nTorsionsFailed + nImpropersFailed) == 0;
}
