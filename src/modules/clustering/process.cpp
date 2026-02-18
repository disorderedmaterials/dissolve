// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "analyser/dataOperator1D.h"
#include "analyser/typeDefs.h"
#include "data/elements.h"
#include "generator/copy.h"
#include "math/histogram1D.h"
#include "math/mathFunc.h"
#include "math/regression.h"
#include "math/vector3.h"
#include "modules/clustering/clustering.h"

bool ClusteringModule::setUp(Dissolve &dissolve, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    // Check user definitions
    if (!(a_ && b_ && (cutoff_ > 0)))
        Messenger::error("Cluster definition invalid! Set both sites and a positive cutoff.");

    // If we have directional bonding, we need to check and determine index map for hydroxyl group
    if (directional_)
    {
        directionIndexes_.clear();
        for (const auto &s : {a_, b_})
        {
            if (s->type() == SpeciesSite::SiteType::Fragment)
            {
                // For a fragment site, we're relying on "#origin, -H(#other)" or similar, with hydrogens (or whatever desired)
                // tagged as #other. If the number of hydrogens exceeds one, the definition should
                // reflect that else only one will be calculated i.e. -H(n=2,#other)
                auto instances = s->instances();
                for (const auto &instance : instances)
                {
                    if (instance.originIndices().size() != 1)
                        return Messenger::error(
                            "NETA defined sites for directional clustering must have a single origin atom!");

                    // For each instance find the tagged group
                    auto &origin = s->parent()->atom(instance.originIndices()[0]);
                    auto identifiers = s->fragment().matchedPath(&origin).identifiers();
                    auto it = identifiers.find("other");
                    if (it == identifiers.end())
                        Messenger::print("Can not find #other tagged group for fragment site {}. Skipping...", s->name());
                    else
                    {
                        // Make note of the tagged group's members' indexes
                        auto taggedGroup = it->second;
                        for (const auto &atom : taggedGroup)
                            directionIndexes_[s].emplace(atom->index());
                    }
                }
            }
            else
            {
                // We can handle non-fragment sites the same way: for these, just auto detect any hydrogens attached to the
                // origin
                auto instances = s->instances();
                for (const auto &instance : instances)
                {
                    if (instance.originIndices().size() != 1)
                        return Messenger::error(
                            "Static and dynamic sites for directional bonding must be created from a single origin "
                            "atom (bonded to at least one hydrogen)!");

                    // Find the hydrogens and add indexes to the map
                    auto &origin = s->parent()->atom(instance.originIndices()[0]);
                    for (const auto &bond : origin.bonds())
                        for (const auto &atom : bond.get().atoms())
                        {
                            if (!atom)
                            {
                                Messenger::error("Inaccessible bond partner found, skipping...");
                                continue;
                            }
                            if (atom->Z() == Elements::H)
                                directionIndexes_[s].emplace(atom->index());
                        }
                }
            }
        }
        // Complain if we don't find any valid hydrogens or atoms in the other group
        if (directionIndexes_.empty())
            return Messenger::error(
                "Failed to find hydroxyl hydrogens or atoms in the tagged #other group - check site set-up!");
    }
    return true;
}

Module::ExecutionResult ClusteringModule::process(Dissolve &dissolve)
{
    auto &moduleData = dissolve.processingModuleData();

    // Produce NeighbourMap - combining map A and B from two filters. base/filter vecs required for site selector
    neighbourMap_.clear();
    Analyser::SiteMap neighbourMapA, neighbourMapB;
    std::vector<const SpeciesSite *> baseSpeciesSites, filterSpeciesSites;

    // Decide how to filter the site maps
    if (a_ == b_ && (selfClusteringA_ || selfClusteringB_))
    {
        Messenger::error("Sites are the same! Disable IncludeAA, IncludeBB...");
        return ExecutionResult::Failed;
    }
    // If both self clustering, base and filter vectors are the same ({a_, b_})
    if (selfClusteringA_ && selfClusteringB_)
    {
        baseSpeciesSites.insert(baseSpeciesSites.end(), {a_, b_});
        filterSpeciesSites = baseSpeciesSites;
    }
    // If just A, we need to filter A sites by A and B
    else if (selfClusteringA_)
    {
        baseSpeciesSites.emplace_back(a_);
        filterSpeciesSites.insert(filterSpeciesSites.end(), {a_, b_});
    }
    // If B, filter Bs by A and B
    else if (selfClusteringB_)
    {
        baseSpeciesSites.emplace_back(b_);
        filterSpeciesSites.insert(filterSpeciesSites.end(), {a_, b_});
    }
    else
    {
        baseSpeciesSites.emplace_back(a_);
        filterSpeciesSites.emplace_back(b_);
    }

    SiteSelector baseSelection(targetConfiguration_, baseSpeciesSites);
    SiteSelector filterSelection(targetConfiguration_, filterSpeciesSites);
    const auto &baseSiteVector = baseSelection.sites();
    const auto &filterSiteVector = filterSelection.sites();

    // min of 0.01 to avoid self-selection, max neighbours 100 (should be enough!)
    SiteFilter base(targetConfiguration_, baseSiteVector);
    std::tie(std::ignore, neighbourMapA) = base.filterBySiteProximity(filterSiteVector, Range(0.001, cutoff_), 1, 100);

    // NeighbourMap needs to by symmetric (Every site has a key)
    // Need to be careful about duplicated entries when we come to combine the maps later
    // If the initial species sites vecs are the same, the map is already symmetric
    if (baseSpeciesSites != filterSpeciesSites)
    {
        // In this case, the A sites are symmetric but the Bs only exist as values - need to filter B by A
        if (selfClusteringA_)
        {
            SiteSelector bSelection(targetConfiguration_, std::vector<const SpeciesSite *>{b_});
            const auto &bSiteVector = bSelection.sites();
            SiteFilter bFilter(targetConfiguration_, bSiteVector);
            std::tie(std::ignore, neighbourMapB) = bFilter.filterBySiteProximity(baseSiteVector, Range(0.001, cutoff_), 1, 100);
        }
        // Same as above but swapping letters around
        else if (selfClusteringB_)
        {
            SiteSelector aSelection(targetConfiguration_, std::vector<const SpeciesSite *>{a_});
            const auto &aSiteVector = aSelection.sites();
            SiteFilter aFilter(targetConfiguration_, aSiteVector);
            std::tie(std::ignore, neighbourMapB) = aFilter.filterBySiteProximity(baseSiteVector, Range(0.001, cutoff_), 1, 100);
        }
        // No self clustering, just different sites, add B by A
        else
        {
            SiteFilter filter(targetConfiguration_, filterSiteVector);
            std::tie(std::ignore, neighbourMapB) = filter.filterBySiteProximity(baseSiteVector, Range(0.001, cutoff_), 1, 100);
        }
    }

    // Combining the neighbour maps into a single map. Because keys may already exist, need to check for them and add
    // neighbours if exists.
    for (auto neighbourMap : {neighbourMapA, neighbourMapB})
        for (const auto &[site, neighbours] : neighbourMap)
        {
            if (neighbourMap_.contains(site))
                neighbourMap_[site].insert(neighbourMap_[site].end(), neighbours.begin(), neighbours.end());
            else
                neighbourMap_.insert({site, neighbours});
        }

    // Now if we're looking at directionality, we check each site and it's neighbours
    if (directional_)
    {
        Analyser::SiteMap tempMap;
        const auto box = targetConfiguration_->box();
        for (auto &[site, neighbours] : neighbourMap_)
        {
            const auto &hIdx = directionIndexes_[site->parent()];

            // Check the site is hydroxyl
            if (hIdx.empty())
                continue;

            // Iterate neighbours
            for (auto it = neighbours.begin(); it != neighbours.end();)
            {
                auto oOVec = box->minimumVector(site->origin(), std::get<0>(*it)->origin());
                auto keep = false;
                for (const auto &h : hIdx)
                {
                    // Get the relevant vectors
                    auto oHVec = box->minimumVector(site->origin(), site->molecule()->atom(h)->r());
                    auto angle = DissolveMath::toDegrees(acos((oOVec / oOVec.magnitude()).dp(oHVec / oHVec.magnitude())));

                    // Make sure we have the smallest angle possible
                    if (360.0 - angle < angle)
                        angle = 360.0 - angle;

                    if (minAngleDev_ <= angle <= maxAngleDev_)
                        keep = true;
                }
                // Add to the temp map symmetrically. Not paying attention to site indexes but I suppose this method tags donors
                // with index = 0 (bar actual 0 index site)
                if (keep)
                {
                    tempMap[site].emplace_back(*it);
                    tempMap[std::get<0>(*it)].emplace_back(Analyser::SiteData(site, 0));
                }
                it++;
            }
        }
        neighbourMap_ = tempMap;
    }

    if (neighbourMap_.empty())
    {
        Messenger::error("No neighbours found!");
        return ExecutionResult::Failed;
    }

    // ClusterMap generation 2.0
    clusterMap_.clear();
    std::unordered_set<const Site *> processed;
    auto clusterTrack = 1;
    for (const auto &[clusterStart, _] : neighbourMap_)
    {
        if (!processed.contains(clusterStart))
        {
            std::unordered_set<const Site *> visited{clusterStart};
            buildCluster(clusterStart, visited);
            processed.insert(visited.begin(), visited.end());
            clusterMap_.insert({clusterTrack, std::vector<const Site *>(visited.begin(), visited.end())});
            clusterTrack++;
        }
    }

    // Molecule cluster map - ensures metrics calculated correctly when a molecule has multiple sites
    molClusterMap_.clear();
    for (const auto &[id, mems] : clusterMap_)
    {
        std::unordered_set<std::shared_ptr<const Molecule>> mols;
        for (const auto &mem : mems)
            mols.emplace(mem->molecule());

        molClusterMap_[id].insert(molClusterMap_[id].end(), mols.begin(), mols.end());
    }

    // Cluster size distribution
    sizeDistribution_.clear();
    for (const auto &[clusterID, members] : molClusterMap_)
        sizeDistribution_[members.size()].emplace_back(clusterID);

    // Size distribution histogram
    auto [histSizeData, status] = moduleData.realiseIf<Histogram1D>("SizeDist", name());
    if (status == GenericItem::ItemStatus::Created)
        histSizeData.initialise(0.5, targetConfiguration_->nMolecules() + 0.5, 1.0);

    histSizeData.zeroBins();

    // Figure out how many molecules of interest are in the configuration
    auto interestingMols = a_->parent() == b_->parent() ? targetConfiguration_->speciesPopulations().value(a_->parent())
                                                        : targetConfiguration_->speciesPopulations().value(a_->parent()) +
                                                              targetConfiguration_->speciesPopulations().value(b_->parent());

    // Find the number of molecules not in clusters
    auto totalMolsClustered = 0;
    for (const auto &[_, memVec] : molClusterMap_)
        totalMolsClustered += memVec.size();

    molsNotClustered_ = interestingMols - totalMolsClustered;

    // Fill histogram with number of mols per cluster size
    for (auto i = 0; i < molsNotClustered_; i++)
        histSizeData.bin(1);

    for (const auto &[size, clusterMems] : sizeDistribution_)
        for (const auto &Cmem : clusterMems)
            for (const auto &Mmem : clusterMap_[Cmem])
                histSizeData.bin(size);

    // Make the 1D and normalise the data to the number of molecules of interest in the system
    auto &sizeData = moduleData.realise<Data1D>("SizeData", name(), GenericItem::InRestartFileFlag);
    histSizeData.accumulate();
    sizeData = histSizeData.accumulatedData();
    sizeData /= interestingMols;

    // Create a percolation threshold line across data range to display P(nc) = nc^-2.189
    // (https://www.sciencedirect.com/science/article/abs/pii/S0378437198005779)
    auto &percLine = moduleData.realise<Data1D>("PercLine", name(), GenericItem::InRestartFileFlag);
    percLine.clear();
    percLine.addPoint(1.0, pow(1, fisher_));
    percLine.addPoint(*sizeData.xAxis().rbegin(), pow(*sizeData.xAxis().rbegin(), fisher_));

    // Cluster mass calculation
    clusterMasses_.clear();
    for (const auto &[clusterID, memberVec] : molClusterMap_)
    {
        auto clusterMass{0.0};
        for (const auto &member : memberVec)
            clusterMass += member->species()->mass();

        clusterMasses_[clusterID] = clusterMass;
    }

    // Cluster mass distribution
    massDistribution_.clear();
    for (const auto &[clusterID, clusterMass] : clusterMasses_)
        massDistribution_[clusterMass].emplace_back(clusterID);

    // Generation of radius of gyration
    radiusOfGyration_.clear();
    const auto *box = targetConfiguration_->box();
    for (const auto &[clusterID, clusterVec] : molClusterMap_)
    {
        if (clusterVec.size() < gyrationMinSize_)
            continue;

        // CoM mass weighted calc from reference site (first member of cluster in clusterMap)
        Vector3 massWeightedTotalVec{0, 0, 0};
        const auto refMol{clusterVec[0]};
        std::vector<int> refIdxs(refMol->nAtoms());
        std::iota(refIdxs.begin(), refIdxs.end(), 0);
        for (const auto &clusterMem : clusterVec)
        {
            std::vector<int> idxs(clusterMem->nAtoms());
            std::iota(idxs.begin(), idxs.end(), 0);
            massWeightedTotalVec +=
                box->minimumVector(refMol->centreOfMass(box, refIdxs), clusterMem->centreOfMass(box, idxs)) *
                clusterMem->species()->mass();
        }

        massWeightedTotalVec /= clusterMasses_[clusterID];
        clusterCoM_[clusterID] = massWeightedTotalVec;
        auto massWeightedDistanceSqrd = 0.0;

        // Run through again for mass weighted distance squared
        for (const auto &clusterMem : clusterVec)
        {
            std::vector<int> idxs(clusterMem->nAtoms());
            std::iota(idxs.begin(), idxs.end(), 0);
            massWeightedDistanceSqrd += (box->minimumDistanceSquared(box->minimumVector(refMol->centreOfMass(box, refIdxs),
                                                                                        clusterMem->centreOfMass(box, idxs)),
                                                                     clusterCoM_[clusterID])) *
                                        clusterMem->species()->mass();
        }
        radiusOfGyration_[clusterID] = std::sqrt(massWeightedDistanceSqrd / clusterMasses_[clusterID]);
    }

    // Fractal Dimension: Create a Data1D object of the log log plot, perform linear regression, return gradient
    Data1D loglog;
    loglog.initialise(radiusOfGyration_.size(), false);

    // Generate Data1D
    for (const auto &[clusterID, rg] : radiusOfGyration_)
        loglog.addPoint(std::log(radiusOfGyration_[clusterID]), std::log(clusterMasses_[clusterID]));

    // Perform linear regression
    fractalDimension_ = Regression::linearGradient(loglog, radiusOfGyration_.size());

    if (saveSizeDist_)
    {
        LineParser parser;
        parser.appendOutput(std::format("{}.{}.sizedist.txt", targetConfiguration_->name(), name()));
        parser.writeLineF("\n# Iteration: {}\n", dissolve.iteration());
        parser.writeLineF("# Cluster size : number of clusters\n");
        for (const auto &[clusterSize, mems] : sizeDistribution_)
            parser.writeLineF("{} {}\n", clusterSize, mems.size());
    }
    if (saveMassDist_)
    {
        LineParser parser;
        parser.appendOutput(std::format("{}.{}.massdist.txt", targetConfiguration_->name(), name()));
        parser.writeLineF("\n# Iteration: {}\n", dissolve.iteration());
        parser.writeLineF("# Cluster mass : number of clusters\n");
        for (const auto &[clusterMass, mems] : massDistribution_)
            parser.writeLineF("{:.3f} {}\n", clusterMass, mems.size());
    }
    if (saveRgMass_)
    {
        LineParser parser;
        parser.appendOutput(std::format("{}.{}.massRg.txt", targetConfiguration_->name(), name()));
        parser.writeLineF("\n# Iteration: {}\n", dissolve.iteration());
        parser.writeLineF("# Fractal dimension:\n{}\n", fractalDimension_);
        parser.writeLineF("# Cluster mass : radius of gyration\n");
        for (const auto &[clusterID, radius] : radiusOfGyration_)
            parser.writeLineF("{:.3f} {:.3f}\n", clusterMasses_[clusterID], radius);
    }
    if (saveCN_)
    {
        LineParser parser;
        calculateCN(0, 0);
        for (const auto &[base, map] : clusterSpeciesCoordNo_)
            for (const auto &[partner, cn] : map)
            {
                parser.appendOutput(std::format("{}.{}.{}{}.CN.txt", targetConfiguration_->name(), name(),
                                                base->name() == partner->name() ? base->parent()->name() : base->name(),
                                                base->name() == partner->name() ? partner->parent()->name() : partner->name()));

                parser.writeLineF("\n# Iteration: {}\n", dissolve.iteration());
                parser.writeLineF("{:.3f}\n", cn);
            }
    }

    return ExecutionResult::Success;
}

// Recursion for cluster building
void ClusteringModule::buildCluster(const Site *startSite, std::unordered_set<const Site *> &visited)
{
    for (const auto &[neighbour, _] : neighbourMap_[startSite])
    {
        if (!visited.contains(neighbour))
        {
            visited.emplace(neighbour);
            buildCluster(neighbour, visited);
        }
    }
}

// Generation of the viewing configuration for given clusters
void ClusteringModule::generateClustersConfig(Dissolve &dissolve, int displaySize, int displayID)
{
    if (clusterConfig_.generator().node("clusters"))
        clusterConfig_.clear();
    else
        clusterConfig_.setName("clusters");

    // Can only get molecule transfer working with a generator...
    clusterConfig_.generator().createRootNode<CopyGeneratorNode>("clusters", targetConfiguration_);
    clusterConfig_.generate(dissolve);
    clusterConfig_.removeMolecules(clusterConfig_.molecules());

    // Display all clusters
    if (displaySize == 0)
    {
        for (const auto &[clusterID, mems] : clusterMap_)
            for (const auto &site : mems)
            {
                auto mol = clusterConfig_.addMolecule(site->parent()->parent());
                for (auto &&[molAtom, sourceAtom] : zip(mol->atoms(), site->molecule()->atoms()))
                {
                    molAtom->setCoordinates(sourceAtom->r());
                    clusterConfig_.updateAtomLocation(molAtom);
                }
            }
    }

    // Display all clusters of size displaySize
    else if (displaySize != 0 && displayID == 0)
    {
        for (const auto &[clusterID, mems] : clusterMap_)
            if (mems.size() == displaySize)
                for (const auto &site : mems)
                {
                    auto mol = clusterConfig_.addMolecule(site->parent()->parent());
                    for (auto &&[molAtom, sourceAtom] : zip(mol->atoms(), site->molecule()->atoms()))
                    {
                        molAtom->setCoordinates(sourceAtom->r());
                        clusterConfig_.updateAtomLocation(molAtom);
                    }
                }
    }

    // Display cluster with ID displayID
    else if (displaySize != 0 && displayID != 0)
    {
        for (const auto &site : clusterMap_[displayID])
        {
            auto mol = clusterConfig_.addMolecule(site->parent()->parent());
            for (auto &&[molAtom, sourceAtom] : zip(mol->atoms(), site->molecule()->atoms()))
            {
                molAtom->setCoordinates(sourceAtom->r());
                clusterConfig_.updateAtomLocation(molAtom);
            }
        }
    }

    clusterConfig_.updateObjectRelationships();
}

// Calculates the contact numbers for the clusters being viewed. Considers only the sites on a molecule that are involved
// in the cluster
void ClusteringModule::calculateCN(int displaySize, int displayID)
{
    std::map<const SpeciesSite *, int> instances;
    clusterSpeciesCoordNo_.clear();

    if (displaySize == 0)
    {
        // Start iterating through the cluster map.
        for (auto const &[_, mems] : clusterMap_)
            for (auto const &mem : mems)
            {
                // With multiple sites per mol, we need to see if our current site has contact with sites from other mols in
                // order to count
                auto foreignNbr = false;

                // Find the member in the neighbour map,
                for (auto const &[memNbr, index] : neighbourMap_[mem])
                {
                    // Ensure this isnt a site from the same molecule
                    if (!(mem->molecule() == memNbr->molecule()))
                    {
                        clusterSpeciesCoordNo_[mem->parent()][memNbr->parent()]++;
                        foreignNbr = true;
                    }
                }
                if (foreignNbr)
                    instances[mem->parent()]++;
            }
    }
    else if (displaySize != 0 && displayID == 0)
    {
        // Same as above but only for given size
        for (const auto &[clusterID, mems] : clusterMap_)
            if (mems.size() == displaySize)
                for (auto const &mem : mems)
                {
                    auto foreignNbr = false;
                    for (auto const &[memNbr, index] : neighbourMap_[mem])
                        if (!(mem->molecule() == memNbr->molecule()))
                        {
                            clusterSpeciesCoordNo_[mem->parent()][memNbr->parent()]++;
                            foreignNbr = true;
                        }
                    if (foreignNbr)
                        instances[mem->parent()]++;
                }
    }
    else if (displaySize != 0 && displayID != 0)
    {
        // Just the given clusterID
        for (auto const &mem : clusterMap_[displayID])
        {
            auto foreignNbr = false;
            for (auto const &[memNbr, index] : neighbourMap_[mem])
            {
                if (!(mem->molecule() == memNbr->molecule()))
                {
                    clusterSpeciesCoordNo_[mem->parent()][memNbr->parent()]++;
                    foreignNbr = true;
                }
            }
            if (foreignNbr)
                instances[mem->parent()]++;
        }
    }
    // Average the contact numbers
    for (const auto &[siteA, num] : instances)
        for (const auto &[siteB, coordNo] : clusterSpeciesCoordNo_[siteA])
            clusterSpeciesCoordNo_[siteA][siteB] /= num;
}
