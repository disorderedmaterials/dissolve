// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/modifierOSites.h"
#include "analyser/dataOperator1D.h"
#include "analyser/siteFilter.h"
#include "analyser/siteSelector.h"
#include "math/integrator.h"

ModifierOSitesNode::ModifierOSitesNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the module", configuration_)->setFlags({ParameterBase::Required});

    // Options
    addOption("Modifier", "Set the modifier site(s) for which the distribution of oxygens should be calculated",
              modifierSpeciesSites_);
    addOption("BondingOxygen", "Set the site(s) 'BO' which are to represent the bonding oxygen", bridgingOxygenSpeciesSites_);
    addOption("NetworkFormer", "Set the site(s) 'NF' for which the distribution around the modifier sites should be calculated",
              networkFormerSpeciesSites_);
    addOption("DistanceRange", "Distance range (min, max) over which to calculate from the central site", distanceRange_);
    addOption("ModifierDistanceRange", "Distance range (min, max) over which to calculate from the central site",
              modifierDistanceRange_);
}

/*
 * Definition
 */

std::string_view ModifierOSitesNode::type() const { return "ModifierOSites"; }

std::string_view ModifierOSitesNode::summary() const
{
    return "Calculate the percentage FO, BO and NBO bonded to a modifier atom";
}

/*
 * Data
 */

// Clear any local data
void ModifierOSitesNode::clearData()
{
    totalOxygensHistogram_.reset();
    totalOxygens_.clear();
    oxygenSitesHistogram_.reset();
    oxygenSites_.clear();
    histMFO_.reset();
    distanceMFO_.clear();
    histMNBO_.reset();
    distanceMNBO_.clear();
    histMBO_.reset();
    distanceMBO_.clear();
    histMOtherO_.reset();
    distanceMOtherO_.clear();
}

// Temporary accessors to data for testing
const IntegerHistogram1D &ModifierOSitesNode::oxygenSitesHistogram() const { return *oxygenSitesHistogram_; }
const Data1D &ModifierOSitesNode::oxygenSites() const { return oxygenSites_; }
const IntegerHistogram1D &ModifierOSitesNode::totalOxygensHistogram() const { return *totalOxygensHistogram_; }
const Data1D &ModifierOSitesNode::totalOxygens() const { return totalOxygens_; }
const Data1D &ModifierOSitesNode::distanceMFO() const { return distanceMFO_; }
const Data1D &ModifierOSitesNode::distanceMNBO() const { return distanceMNBO_; }
const Data1D &ModifierOSitesNode::distanceMBO() const { return distanceMBO_; }
const Data1D &ModifierOSitesNode::distanceMOtherO() const { return distanceMOtherO_; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult ModifierOSitesNode::process()
{
    // Select all potential bridging oxygen sites - we will determine which actually are
    // involved in NF-BO-NF interactions once we have the available NF sites
    SiteSelector allOxygenSites(configuration_, bridgingOxygenSpeciesSites_.getSpeciesSites());

    // Select all NF centres
    const SiteSelector NF(configuration_, networkFormerSpeciesSites_.getSpeciesSites());

    // Select all modifier centres
    const SiteSelector modifier(configuration_, modifierSpeciesSites_.getSpeciesSites());

    // Filter the oxygen sites into those surrounded by up to two NF sites
    SiteFilter ofilter(configuration_, allOxygenSites.sites());
    auto &&[filteredOSites, neighbourMap] = ofilter.filterBySiteProximity(NF.sites(), distanceRange_, 0, 2);

    SiteFilter mfilter(configuration_, modifier.sites());
    auto &&[filteredMSites, mNeighbourMapO] =
        mfilter.filterBySiteProximity(allOxygenSites.sites(), modifierDistanceRange_, 0, 99);

    // Initialise data storage if required
    if (!oxygenSitesHistogram_)
        oxygenSitesHistogram_.emplace().initialise();
    if (!totalOxygensHistogram_)
        totalOxygensHistogram_.emplace().initialise();
    if (!histMFO_)
        histMFO_.emplace().initialise(distanceRange_.minimum(), modifierDistanceRange_.maximum(), 0.05);
    if (!histMNBO_)
        histMNBO_.emplace().initialise(distanceRange_.minimum(), modifierDistanceRange_.maximum(), 0.05);
    if (!histMBO_)
        histMBO_.emplace().initialise(distanceRange_.minimum(), modifierDistanceRange_.maximum(), 0.05);
    if (!histMOtherO_)
        histMOtherO_.emplace().initialise(distanceRange_.minimum(), distanceRange_.maximum(), 0.05);

    // Create an ordered set of references to histograms with increasing oxygen bond patterns
    std::vector<std::reference_wrapper<Histogram1D>> histogramsMO = {*histMFO_, *histMNBO_, *histMBO_, *histMOtherO_};

    // Clear the temporary bins
    totalOxygensHistogram_->zeroBins();
    oxygenSitesHistogram_->zeroBins();
    histMFO_->zeroBins();
    histMNBO_->zeroBins();
    histMBO_->zeroBins();
    histMOtherO_->zeroBins();

    // For each modifier site, bin the number of neighbour oxygens, then for each of those oxygen bin its type
    std::map<const Site *, int> qSpecies;
    std::map<int, int> oxygenSites;
    for (const auto &[siteM, nearO] : mNeighbourMapO)
    {
        totalOxygensHistogram_->bin(nearO.size());
        for (auto &&[oSite, index] : nearO)
        {
            oxygenSitesHistogram_->bin(neighbourMap[oSite].size());

            int size = neighbourMap[oSite].size();
            histogramsMO[std::min(size, 3)].get().bin(configuration_->box().minimumDistance(siteM->origin(), oSite->origin()));
        }
    }

    // Accumulate histogram averages
    oxygenSitesHistogram_->accumulate();
    totalOxygensHistogram_->accumulate();
    histMFO_->accumulate();
    histMNBO_->accumulate();
    histMBO_->accumulate();
    histMOtherO_->accumulate();

    // Averaged values for OSites
    oxygenSites_ = oxygenSitesHistogram_->accumulatedData();
    auto sum = Integrator::absSum(oxygenSitesHistogram_->data());
    oxygenSites_ /= sum;

    // Average values for total O sites
    totalOxygens_ = totalOxygensHistogram_->accumulatedData();
    auto totalOSites = Integrator::absSum(totalOxygensHistogram_->data());
    totalOxygens_ /= totalOSites;

    // Normalise HistMFO
    distanceMFO_ = histMFO_->accumulatedData();
    DataOperator1D histMFONormaliser(distanceMFO_);
    // Normalise by value
    histMFONormaliser.normaliseSumTo();

    // Normalise HistMNBO
    distanceMNBO_ = histMNBO_->accumulatedData();
    DataOperator1D histMNBONormaliser(distanceMNBO_);
    // Normalise by value
    histMNBONormaliser.normaliseSumTo();

    // Normalise HistMBO
    distanceMBO_ = histMBO_->accumulatedData();
    DataOperator1D histMBONormaliser(distanceMBO_);
    // Normalise by value
    histMBONormaliser.normaliseSumTo();

    // Normalise HistMOtherO
    distanceMOtherO_ = histMOtherO_->accumulatedData();
    DataOperator1D histMOtherONormaliser(distanceMOtherO_);
    // Normalise by value
    histMOtherONormaliser.normaliseSumTo();

    return NodeConstants::ProcessResult::Success;
}
