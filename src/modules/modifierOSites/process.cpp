// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/siteFilter.h"
#include "analyser/siteSelector.h"
#include "main/dissolve.h"
#include "math/integerHistogram1D.h"
#include "math/integrator.h"
#include "module/context.h"
#include "modules/modifierOSites/modifierOSites.h"

// Run main processing
Module::ExecutionResult ModifierOSitesModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }
    auto &processingData = moduleContext.dissolve().processingModuleData();
    auto &processingData2 = moduleContext.dissolve().processingModuleData();

    // Select all potential bridging oxygen sites - we will determine which actually are
    // involved in NF-BO-NF interactions once we have the available NF sites
    SiteSelector allOxygenSites(targetConfiguration_, bridgingOxygenSpeciesSites_);

    // Select all NF centres
    SiteSelector NF(targetConfiguration_, networkFormerSpeciesSites_);

    // Select all modifier centres
    SiteSelector modifier(targetConfiguration_, modifierSpeciesSites_);

    // Filter the oxygen sites into those surrounded by exactly two NF sites
    SiteFilter filter(targetConfiguration_, allOxygenSites.sites());
    auto &&[BO, neighbourMap] = filter.filterBySiteProximity(NF.sites(), distanceRange_, 0, 2);

    SiteFilter filter2(targetConfiguration_, modifier.sites());
    auto &&[BO2, neighbourMap2] = filter2.filterBySiteProximity(allOxygenSites.sites(), modifierDistanceRange_, 0, 99);

    // Retrieve storage for the Oxygen Sites histogram
    auto [oxygenSitesHistogram, status] =
        processingData.realiseIf<IntegerHistogram1D>("OSitesHistogram", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        oxygenSitesHistogram.initialise();

    // Retrieve storage for the Q-species histogram
    auto [modifierHistogram, status2] =
        processingData.realiseIf<IntegerHistogram1D>("ModifierHistogram", name(), GenericItem::InRestartFileFlag);
    if (status2 == GenericItem::ItemStatus::Created)
        modifierHistogram.initialise();

    // Clear the temporary bins
    modifierHistogram.zeroBins();
    oxygenSitesHistogram.zeroBins();

    // The returned 'neighbourMap' maps BO sites to nearby NF sites *only if* there were exactly two NF sites within range.
    // So, we can use this to determine the Q numbers for each NF by counting the number of times a NF site appears in the map.
    std::map<const Site *, int> qSpecies;
    std::map<int, int> oxygenSites;
    for (const auto &[siteM, nearO] : neighbourMap2)
    {
        for (auto &&[oSite, index] : nearO)
        {
            oxygenSitesHistogram.bin(neighbourMap[oSite].size());
        }
    }

    // Accumulate histogram averages
    oxygenSitesHistogram.accumulate();
    modifierHistogram.accumulate();

    // Averaged values for OSites
    Data1D accumulatedData = oxygenSitesHistogram.accumulatedData();
    auto sum = Integrator::absSum(oxygenSitesHistogram.data());
    accumulatedData /= sum;

    // Create the display data
    processingData.realise<Data1D>("OSites", name(), GenericItem::InRestartFileFlag) = accumulatedData;
    processingData2.realise<Data1D>("ModifierSites", name(), GenericItem::InRestartFileFlag) = modifierHistogram.data();

    // Save data?
    if (exportFileAndFormat_.hasFilename())
    {
        if (moduleContext.processPool().isMaster())
        {
            if (exportFileAndFormat_.exportData(accumulatedData))
                moduleContext.processPool().decideTrue();
            else
            {
                moduleContext.processPool().decideFalse();
            }
        }
    }
    return ExecutionResult::Success;
}
