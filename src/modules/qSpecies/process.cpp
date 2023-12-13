// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/siteFilter.h"
#include "analyser/siteSelector.h"
#include "main/dissolve.h"
#include "math/integerHistogram1D.h"
#include "module/context.h"
#include "modules/qSpecies/qSpecies.h"

// Run main processing
Module::ExecutionResult QSpeciesModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }
    auto &processingData = moduleContext.dissolve().processingModuleData();

    // Select all potential bridging oxygen sites - we will determine which actually are
    // involved in NF-BO-NF interactions once we have the available NF sites
    SiteSelector allOxygenSites(targetConfiguration_, bridgingOxygenSpeciesSites_);

    // Select all NF centres
    SiteSelector NF(targetConfiguration_, networkFormerSpeciesSites_);

    // Filter the oxygen sites into those surrounded by exactly two NF sites
    SiteFilter filter(targetConfiguration_, allOxygenSites.sites());
    auto &&[BO, neighbourMap] = filter.filterBySiteProximity(NF.sites(), distanceRange_, 0, 2);

    // The returned 'neighbourMap' maps BO sites to nearby NF sites *only if* there were exactly two NF sites within range.
    // So, we can use this to determine the Q numbers for each NF by counting the number of times a NF site appears in the map.
    std::map<const Site *, int> qSpecies;
    std::map<int, int> oxygenSites;
    for (const auto &[siteBO, nbrNF] : neighbourMap)
    {
        ++oxygenSites[nbrNF.size()];
        if (nbrNF.size() == 2)
        {
            for (const auto &[nbr, nbrIndex] : nbrNF)
            {
                ++qSpecies[nbr];
            }
        }
    }

    // Retrieve storage for the Q-species histogram
    auto [qSpeciesHistogram, status] =
        processingData.realiseIf<IntegerHistogram1D>("QHistogram", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        qSpeciesHistogram.initialise();

    // Retrieve storage for the Oxygen Sites histogram
    auto [oxygenSitesHistogram, status1] =
        processingData.realiseIf<IntegerHistogram1D>("OSitesHistogram", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        oxygenSitesHistogram.initialise();

    // Clear the temporary bins
    qSpeciesHistogram.zeroBins();

    // Bin our mapped Q counts
    for (auto &[key, value] : qSpecies)
        qSpeciesHistogram.bin(value);

    // Bin our mapped O Sites
    for (auto &[key, value] : oxygenSites)
        oxygenSitesHistogram.bin(value);

    // Don't forget the Q=0 count - this is equivalent to the total number of NF sites minus the number mapped in 'qSpecies'
    qSpeciesHistogram.bin(0, NF.sites().size() - qSpecies.size());

    // Accumulate histogram averages
    qSpeciesHistogram.accumulate();
    oxygenSitesHistogram.accumulate();

    // Create the display data
    processingData.realise<Data1D>("QSpecies", name(), GenericItem::InRestartFileFlag) = qSpeciesHistogram.data();
    processingData.realise<Data1D>("OxygenSites", name(), GenericItem::InRestartFileFlag) = oxygenSitesHistogram.data();

    return ExecutionResult::Success;
}
