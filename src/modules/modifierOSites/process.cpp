// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser1D.h"
#include "analyser/siteFilter.h"
#include "analyser/siteSelector.h"
#include "main/dissolve.h"
#include "math/histogram1D.h"
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

    // Select all potential bridging oxygen sites - we will determine which actually are
    // involved in NF-BO-NF interactions once we have the available NF sites
    SiteSelector allOxygenSites(targetConfiguration_, bridgingOxygenSpeciesSites_);

    // Select all NF centres
    SiteSelector NF(targetConfiguration_, networkFormerSpeciesSites_);

    // Select all modifier centres
    SiteSelector modifier(targetConfiguration_, modifierSpeciesSites_);

    // Filter the oxygen sites into those surrounded by up to two NF sites
    SiteFilter ofilter(targetConfiguration_, allOxygenSites.sites());
    auto &&[filteredOSites, neighbourMap] = ofilter.filterBySiteProximity(NF.sites(), distanceRange_, 0, 2);

    SiteFilter mfilter(targetConfiguration_, modifier.sites());
    auto &&[filteredMSites, mNeighbourMapO] =
        mfilter.filterBySiteProximity(allOxygenSites.sites(), modifierDistanceRange_, 0, 99);

    // Retrieve storage for the Mofifier to Oxygen Type Sites histogram
    auto [oxygenSitesHistogram, status] =
        processingData.realiseIf<IntegerHistogram1D>("OTypeSitesHistogram", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        oxygenSitesHistogram.initialise();

    // Retrieve storage for the Total O Sites histogram
    auto [modifierHistogram, status2] =
        processingData.realiseIf<IntegerHistogram1D>("TotalOHistogram", name(), GenericItem::InRestartFileFlag);
    if (status2 == GenericItem::ItemStatus::Created)
        modifierHistogram.initialise();

    // Calculate rMFO
    auto [histMFO, status3] = processingData.realiseIf<Histogram1D>("Histo-MFO", name(), GenericItem::InRestartFileFlag);
    if (status3 == GenericItem::ItemStatus::Created)
        histMFO.initialise(distanceRange_.minimum(), distanceRange_.maximum(), 0.05);

    // Calculate rMNBO
    auto [histMNBO, status4] = processingData.realiseIf<Histogram1D>("Histo-MNBO", name(), GenericItem::InRestartFileFlag);
    if (status4 == GenericItem::ItemStatus::Created)
        histMNBO.initialise(distanceRange_.minimum(), distanceRange_.maximum(), 0.05);

    // Calculate rMBO
    auto [histMBO, status5] = processingData.realiseIf<Histogram1D>("Histo-MBO", name(), GenericItem::InRestartFileFlag);
    if (status5 == GenericItem::ItemStatus::Created)
        histMBO.initialise(distanceRange_.minimum(), distanceRange_.maximum(), 0.05);

    // Calculate rMOtherO
    auto [histMOtherO, status6] =
        processingData.realiseIf<Histogram1D>("Histo-MOtherO", name(), GenericItem::InRestartFileFlag);
    if (status6 == GenericItem::ItemStatus::Created)
        histMOtherO.initialise(distanceRange_.minimum(), distanceRange_.maximum(), 0.05);

    // Clear the temporary bins
    modifierHistogram.zeroBins();
    oxygenSitesHistogram.zeroBins();
    histMFO.zeroBins();
    histMNBO.zeroBins();
    histMBO.zeroBins();
    histMOtherO.zeroBins();

    // For each modifier site, bin the number of neighbour oxygens, then for each of those oxygen bin its type
    std::map<const Site *, int> qSpecies;
    std::map<int, int> oxygenSites;
    for (const auto &[siteM, nearO] : mNeighbourMapO)
    {
        modifierHistogram.bin(nearO.size());
        for (auto &&[oSite, index] : nearO)
        {
            oxygenSitesHistogram.bin(neighbourMap[oSite].size());

            switch (neighbourMap[oSite].size())
            {
                case 0:
                    histMFO.bin(targetConfiguration_->box()->minimumDistance(siteM->origin(), oSite->origin()));
                    break;
                case 1:
                    histMNBO.bin(targetConfiguration_->box()->minimumDistance(siteM->origin(), oSite->origin()));
                    break;
                case 2:
                    histMBO.bin(targetConfiguration_->box()->minimumDistance(siteM->origin(), oSite->origin()));
                    break;
                default:
                    histMOtherO.bin(targetConfiguration_->box()->minimumDistance(siteM->origin(), oSite->origin()));
            }
        }
    }

    // Accumulate histogram averages
    oxygenSitesHistogram.accumulate();
    modifierHistogram.accumulate();
    histMFO.accumulate();
    histMNBO.accumulate();
    histMBO.accumulate();
    histMOtherO.accumulate();

    // Averaged values for OSites
    Data1D accumulatedData = oxygenSitesHistogram.accumulatedData();
    auto sum = Integrator::absSum(oxygenSitesHistogram.data());
    accumulatedData /= sum;

    // Average values for total O sites
    Data1D accumulatedModifierData = modifierHistogram.accumulatedData();
    auto totalOSites = Integrator::absSum(modifierHistogram.data());
    accumulatedModifierData /= totalOSites;

    // Normalise HistMFO
    Data1D dataNormalisedHistMFO = histMFO.accumulatedData();
    DataNormaliser1D histMFONormaliser(dataNormalisedHistMFO);
    // Normalise by value
    histMFONormaliser.normaliseTo();

    // Normalise HistMNBO
    Data1D dataNormalisedHistMNBO = histMNBO.accumulatedData();
    DataNormaliser1D histMNBONormaliser(dataNormalisedHistMNBO);
    // Normalise by value
    histMNBONormaliser.normaliseTo();

    // Normalise HistMBO
    Data1D dataNormalisedHistMBO = histMBO.accumulatedData();
    DataNormaliser1D histMBONormaliser(dataNormalisedHistMBO);
    // Normalise by value
    histMBONormaliser.normaliseTo();

    // Normalise HistMOtherO
    Data1D dataNormalisedHistMOtherO = histMOtherO.accumulatedData();
    DataNormaliser1D histMOtherONormaliser(dataNormalisedHistMOtherO);
    // Normalise by value
    histMOtherONormaliser.normaliseTo();

    // Store the normalised data
    processingData.realise<Data1D>("OTypes", name(), GenericItem::InRestartFileFlag) = accumulatedData;
    processingData.realise<Data1D>("TotalOSites", name(), GenericItem::InRestartFileFlag) = accumulatedModifierData;
    processingData.realise<Data1D>("MFOBondLength", name(), GenericItem::InRestartFileFlag) = dataNormalisedHistMFO;
    processingData.realise<Data1D>("MNBOBondLength", name(), GenericItem::InRestartFileFlag) = dataNormalisedHistMNBO;
    processingData.realise<Data1D>("MBOBondLength", name(), GenericItem::InRestartFileFlag) = dataNormalisedHistMBO;
    processingData.realise<Data1D>("MOtherOBondLength", name(), GenericItem::InRestartFileFlag) = dataNormalisedHistMOtherO;

    // Save data?
    if (exportFileAndFormatOType_.hasFilename())
        if (moduleContext.processPool().isMaster())
            if (exportFileAndFormatOType_.exportData(accumulatedData))
                moduleContext.processPool().decideTrue();
            else
            {
                moduleContext.processPool().decideFalse();
            }

    if (exportFileAndFormatTotalOSites_.hasFilename())
        if (moduleContext.processPool().isMaster())
            if (exportFileAndFormatTotalOSites_.exportData(accumulatedModifierData))
                moduleContext.processPool().decideTrue();
            else
            {
                moduleContext.processPool().decideFalse();
            }

    return ExecutionResult::Success;
}
