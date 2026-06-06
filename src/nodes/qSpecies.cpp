// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/qSpecies.h"
#include "analyser/dataOperator1D.h"
#include "analyser/siteFilter.h"
#include "analyser/siteSelector.h"
#include "main/dissolve.h"
#include "math/integerHistogram1D.h"
#include "math/integrator.h"
#include "modules/qSpecies/qSpecies.h"

QSpeciesNode::QSpeciesNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the module", configuration_)->setFlags({ParameterBase::Required});

    // Options
    addOption("BondingOxygen", "Set the site(s) 'BO' which are to represent the bonding oxygen", bridgingOxygenSpeciesSites_);
    addOption("NetworkFormer",
              "Set the site(s) 'NF' for which the distribution around the bonding oxygen sites should be calculated",
              networkFormerSpeciesSites_);
    addOption("DistanceRange", "Distance range (min, max) over which to calculate Q-Species from central site", distanceRange_);
}

std::string_view QSpeciesNode::type() const { return "QSpecies"; }

std::string_view QSpeciesNode::summary() const { return "Calculate QSpecies of a network former"; }

/*
 * Data
 */

// Clear any local data
void QSpeciesNode::clearData()
{
    qSpeciesHistogram_.reset();
    qSpecies_.clear();
    oxygenSitesHistogram_.reset();
    oxygenSites_.clear();
}

// Temporary accessors to data for testing
const Data1D &QSpeciesNode::qSpecies() const { return qSpecies_; }

/*
 * Processing
 */

// Run main processing
NodeConstants::ProcessResult QSpeciesNode::process()
{
    // Initialise storage if required
    if (!qSpeciesHistogram_)
        qSpeciesHistogram_.emplace();
    if (!oxygenSitesHistogram_)
        oxygenSitesHistogram_.emplace();

    // Clear the temporary bins
    qSpeciesHistogram_->zeroBins();
    oxygenSitesHistogram_->zeroBins();

    // Select all potential bridging oxygen sites - we will determine which actually are
    // involved in NF-BO-NF interactions once we have the available NF sites
    const SiteSelector allOxygenSites(configuration_, bridgingOxygenSpeciesSites_.getSpeciesSites());

    // Select all NF centres
    const SiteSelector NF(configuration_, networkFormerSpeciesSites_.getSpeciesSites());

    // Filter the oxygen sites into those surrounded by exactly two NF sites
    SiteFilter filter(configuration_, allOxygenSites.sites());
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

    // Bin our mapped Q counts
    for (auto &[key, value] : qSpecies)
        qSpeciesHistogram_->bin(value);

    // Bin our mapped O Sites
    for (auto &[key, value] : oxygenSites)
        oxygenSitesHistogram_->bin(key, value);

    // Don't forget the Q=0 count - this is equivalent to the total number of NF sites minus the number mapped in 'qSpecies'
    qSpeciesHistogram_->bin(0, NF.sites().size() - qSpecies.size());

    // Accumulate histogram averages
    qSpeciesHistogram_->accumulate();
    oxygenSitesHistogram_->accumulate();

    // Averaged values for Q-Species
    qSpecies_ = qSpeciesHistogram_->accumulatedData();
    DataOperator1D normaliserQ(qSpecies_);
    normaliserQ.normaliseSumTo();

    oxygenSites_ = oxygenSitesHistogram_->accumulatedData();

    // // Save data?
    // if (!DataExporter::exportData(accumulatedQData, exportFileAndFormat_))
    //     return ExecutionResult::Failed;

    return NodeConstants::ProcessResult::Success;
}
