// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/pickProximity.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/integer.h"
#include "keywords/optionalDouble.h"
#include "keywords/optionalInt.h"
#include "keywords/speciesVector.h"
#include "templates/algorithms.h"

PickProximityProcedureNode::PickProximityProcedureNode() : PickProcedureNodeBase(ProcedureNode::NodeType::PickProximity)
{
    keywords_.setOrganisation("Options", "Proximal Species");
    keywords_.add<SpeciesVectorKeyword>("Species", "Species to count", speciesToPick_);
    keywords_.add<OptionalIntegerKeyword>("MinCount", "Minimum number", minCount_, 0, std::nullopt, 1, "Off");
    keywords_.add<OptionalIntegerKeyword>("MaxCount", "Maximum number", maxCount_, 0, std::nullopt, 1, "Off");
    keywords_.add<OptionalDoubleKeyword>("MinDistance", "Minimum distance for picking (Angstroms)", minDistance_, 0.0,
                                         std::nullopt, 0.1, "Off");
    keywords_.add<OptionalDoubleKeyword>("MaxDistance", "Maximum distance for picking (Angstroms)", maxDistance_, 0.0,
                                         std::nullopt, 0.1, "Off");
}

/*
 * Execute
 */

// Execute node
bool PickProximityProcedureNode::execute(const ProcedureContext &procedureContext)
{
    Messenger::print("[PickProximity] Molecules will be selected from {}.\n", moleculePoolName());

    // Clear our molecules vector
    pickedMolecules_.clear();

    // Retrieve control values
    auto rMin = minDistance_.value_or(0.0);
    auto nMin = minCount_.value_or(0);

    // Print info
    if (!speciesToPick_.empty())
        Messenger::print("[PickProximity] Proximal species are: {}.\n",
                         joinStrings(speciesToPick_, " ", [](const auto &sp) { return sp->name(); }));
    if (maxDistance_)
        Messenger::print("[PickProximity] Allowed distance range is {} <= r <= {} Angstroms.\n", rMin, maxDistance_.value());
    else
        Messenger::print("[PickProximity] Allowed distance range is r >= {} Angstroms.\n", rMin);

    if (maxCount_.has_value())
        Messenger::print("[PickProximity] Allowed coordination count is {} <= N <= {}.\n", nMin, maxCount_.value());
    else
        Messenger::print("[PickProximity] Allowed coordination count is N >= {}.\n", nMin);

    const auto *box = procedureContext.configuration()->box();

    // Loop over all target molecules
    for (const auto &molI : moleculePool(procedureContext.configuration()))
    {
        auto count = 0;

        // Get centre of geometry of molecule
        auto iCog = molI->centreOfGeometry(box);

        // Loop over all molecules
        for (const auto &molJ : procedureContext.configuration()->molecules())
        {
            // Count surrounding species (if defined)
            if (std::find(speciesToPick_.begin(), speciesToPick_.end(), molJ->species()) != speciesToPick_.end())
            {
                // Get centre of geometry of species
                auto jCog = molJ->centreOfGeometry(box);

                // Calculate i-j minimum distance
                auto r = box->minimumDistance(iCog, jCog);

                // Check distance criteria
                if (r >= rMin && (!maxDistance_.has_value() || r <= maxDistance_.value()))
                    ++count;
            }
        }

        // Check total count against criteria
        if (count >= nMin && (!maxCount_.has_value() || count <= maxCount_.value()))
            pickedMolecules_.push_back(molI);
    }

    Messenger::print("[PickProximity] Total molecules picked = {}.\n", pickedMolecules_.size());

    return true;
}
