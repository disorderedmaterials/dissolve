// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/pickproximity.h"
#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/types.h"
#include "templates/algorithms.h"

PickProximityProcedureNode::PickProximityProcedureNode() : PickProcedureNodeBase(ProcedureNode::NodeType::PickProximity)
{
    keywords_.add("Control", new SpeciesVectorKeyword, "Species", "Species to count");
    keywords_.add("Control", new IntegerKeyword(0), "MinCount", "Minimum number");
    keywords_.add("Control", new IntegerKeyword(0), "MaxCount", "Maximum number");
    keywords_.add("Control", new DoubleKeyword(0.0, 0.0), "MinDistance", "Minimum distance");
    keywords_.add("Control", new DoubleKeyword(0.0, 0.0), "MaxDistance", "Maximum distance");
}

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
bool PickProximityProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                         GenericList &targetList)
{
    Messenger::print("[PickProximity] Molecules will be selected from {}.\n", moleculePoolName());

    // Clear our molecules vector
    pickedMolecules_.clear();

    // Retrieve control values
    auto rMin = keywords_.hasBeenSet("MinDistance") ? keywords_.asDouble("MinDistance") : 0.0;
    std::optional<double> rMax;
    if (keywords_.hasBeenSet("MaxDistance"))
        rMax = keywords_.asDouble("MaxDistance");
    auto nMin = keywords_.hasBeenSet("MinCount") ? keywords_.asInt("MinCount") : 0;
    std::optional<int> nMax;
    if (keywords_.hasBeenSet("MaxCount"))
        nMax = keywords_.asInt("MaxCount");
    auto &species = keywords_.retrieve<std::vector<const Species *>>("Species");

    // Print info
    if (!species.empty())
        Messenger::print("[PickProximity] Proximal species are: {}.\n",
                         joinStrings(species, " ", [](const auto &sp) { return sp->name(); }));
    if (rMax.has_value())
        Messenger::print("[PickProximity] Allowed distance range is {} <= r <= {} Angstroms.\n", rMin, rMax.value());
    else
        Messenger::print("[PickProximity] Allowed distance range is r >= {} Angstroms.\n", rMin);

    if (nMax.has_value())
        Messenger::print("[PickProximity] Allowed coordination count is {} <= N <= {}.\n", nMin, nMax.value());
    else
        Messenger::print("[PickProximity] Allowed coordination count is N >= {}.\n", nMin);

    const auto *box = cfg->box();

    // Loop over all target molecules
    for (const auto &molI : moleculePool(cfg))
    {
        auto count = 0;

        // Get centre of geometry of molecule
        auto iCog = molI->centreOfGeometry(box);

        // Loop over all molecules
        for (const auto &molJ : cfg->molecules())
        {
            // Count surrounding species (if defined)
            if (std::find(species.begin(), species.end(), molJ->species()) != species.end())
            {
                // Get centre of geometry of species
                auto jCog = molJ->centreOfGeometry(box);

                // Calculate i-j minimum distance
                auto r = box->minimumDistance(iCog, jCog);

                // Check distance criteria
                if (r >= rMin && (!rMax.has_value() || r <= rMax.value()))
                    ++count;
            }
        }

        // Check total count against criteria
        if (count >= nMin && (!nMax.has_value() || count <= nMax.value()))
            pickedMolecules_.push_back(molI);
    }

    Messenger::print("[PickProximity] Total molecules picked = {}.\n", pickedMolecules_.size());

    return true;
}
