// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "analyser/typeDefs.h"
#include "base/sysFunc.h"
#include "data/elements.h"
#include "generator/box.h"
#include "generator/copy.h"
#include "math/regression.h"
#include "modules/clustering/clustering.h"

bool ClusteringModule::setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    // Check user definitions
    if (!(a_ && b_ && (cutoff_ > 0)))
        Messenger::error("Cluster definition invalid! Set both sites and a positive cutoff.");

    // If we have strict bonding, we need to check and determine index map for hydroxyl group
    if (strict_)
    {
        hydroxylIndexes_.clear();
        for (const auto &s : {a_, b_})
        {
            // Do some checks. For now, requiring the site to be static.
            if (s->type() != SpeciesSite::SiteType::Static || s->staticOriginAtoms().size() != 1)
            {
                Messenger::error("For directional hydrogen bonding, site must be a static type based on a single origin atom.");
                return false;
            }
            auto o = s->staticOriginAtoms()[0];
            if (!o)
            {
                Messenger::error("The origin atom for site is inaccessible.");
                return false;
            }
            if (o->Z() != Elements::O)
            {
                Messenger::error("For directional hydrogen bonding, the static origin atom for site must be an Oxygen.");
                return false;
            }
            // Find the hydroxyl hydrogens and add index to the map
            for (const auto &bond : o->bonds())
                for (const auto &atom : bond.get().atoms())
                {
                    if (!atom)
                    {
                        Messenger::error("Inaccessible bond partner found, skipping...");
                        continue;
                    }
                    if (atom->Z() == Elements::H)
                        hydroxylIndexes_[s].emplace(atom->index());
                }
        }
        // Complain if we don't find any valid hydrogens
        if (hydroxylIndexes_.empty())
        {
            Messenger::error("Failed to find hydroxyl hydrogens - check site set-up!");
            return false;
        }
    }

    return true;
}

Module::ExecutionResult ClusteringModule::process(ModuleContext &moduleContext)
{
    auto &moduleData = moduleContext.dissolve().processingModuleData();

    // Produce NeighbourMap
    neighbourMap_.clear();
    Analyser::SiteMap neighbourMapA, neighbourMapB;

    // Transform SiteObjects to instances ready for the filter function
    SiteSelector selectionA(targetConfiguration_, std::vector<const SpeciesSite *>{a_});
    const auto &siteVectorA = selectionA.sites();

    SiteSelector selectionB(targetConfiguration_, std::vector<const SpeciesSite *>{b_});
    const auto &siteVectorB = selectionB.sites();

    SiteFilter filterA(targetConfiguration_, siteVectorA);
    std::tie(std::ignore, neighbourMapA) = filterA.filterBySiteProximity(siteVectorB, Range(0.001, cutoff_), 1,
                                                                         100); // min of 0.01 to avoid self-selection

    // If we're dealing with a definition between the same site, it's already symmetric. Running twice will add dupes
    if (a_ != b_)
    {
        SiteFilter filterB(targetConfiguration_, siteVectorB);
        std::tie(std::ignore, neighbourMapB) = filterB.filterBySiteProximity(siteVectorA, Range(0.001, cutoff_), 1, 100);
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
    if (strict_)
    {
        Analyser::SiteMap tempMap;
        const auto box = targetConfiguration_->box();
        for (auto &[site, neighbours] : neighbourMap_)
        {
            const auto &hIdx = hydroxylIndexes_[site->parent()];
            // Check the site is hydroxyl
            if (hIdx.empty())
                continue;

            // Iterate neighbours
            for (auto it = neighbours.begin(); it != neighbours.end();)
            {
                auto oOVec = box->minimumVector(site->origin(), std::get<0>(*it)->origin());
                bool keep = false;
                for (const auto &h : hIdx)
                {
                    // Get the relevant vectors
                    auto oHVec = box->minimumVector(site->origin(), site->molecule()->atom(h)->r());
                    auto angle = box->angleInDegrees(oOVec / oOVec.magnitude(), oHVec / oHVec.magnitude());

                    // Make sure we have the smallest angle possible
                    if (360.0 - angle < angle)
                        angle = 360.0 - angle;

                    if (angle <= angleDev_)
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
        Messenger::error("No neighbours found!");

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

    // Cluster size distribution
    sizeDistribution_.clear();
    for (const auto &[clusterID, members] : clusterMap_)
        sizeDistribution_[members.size()].emplace_back(clusterID);

    auto &sizeData = moduleData.realise<Data1D>("SizeDist", name());
    sizeData.clear();
    for (const auto &[size, mems] : sizeDistribution_)
        sizeData.addPoint(std::log(size), std::log(mems.size()));

    // Cluster mass calculation
    clusterMasses_.clear();
    for (const auto &[clusterID, memberVec] : clusterMap_)
    {
        float clusterMass{0};
        for (const auto &member : memberVec)
            clusterMass += member->parent()->parent()->mass();

        clusterMasses_[clusterID] = clusterMass;
    }

    // Cluster mass distribution
    massDistribution_.clear();
    for (const auto &[clusterID, clusterMass] : clusterMasses_)
        massDistribution_[clusterMass].emplace_back(clusterID);

    auto &massData = moduleData.realise<Data1D>("MassDist", name());
    massData.clear();
    for (const auto &[mass, mems] : massDistribution_)
        massData.addPoint(std::log(mass), std::log(mems.size()));

    // Generation of radius of gyration
    radiusOfGyration_.clear();
    const auto *box = targetConfiguration_->box();
    for (const auto &[clusterID, clusterVec] : clusterMap_)
    {
        if (clusterVec.size() < gyrationMinSize_)
            continue;

        // CoM mass weighted calc from reference site
        Vec3<double> massWeightedTotalVec{0, 0, 0};
        const auto *refSite{clusterVec[0]}; // Reference as first member in cluster
        for (const auto &clusterMem : clusterVec)
            massWeightedTotalVec +=
                (box->minimumVector(refSite->origin(), clusterMem->origin())) * clusterMem->parent()->parent()->mass();

        massWeightedTotalVec /= clusterMasses_[clusterID];
        clusterCoM_[clusterID] = massWeightedTotalVec;
        auto massWeightedDistanceSqrd = 0.0;

        // Run through again for mass weighted distance squared
        for (const auto &clusterMem : clusterVec)
            massWeightedDistanceSqrd +=
                (box->minimumDistanceSquared(box->minimumVector(clusterMem->origin(), refSite->origin()),
                                             clusterCoM_[clusterID])) *
                clusterMem->parent()->parent()->mass();

        radiusOfGyration_[clusterID] = std::sqrt(massWeightedDistanceSqrd / clusterMasses_[clusterID]);
    }

    // Fractal Dimension: Create a Data1D object of the log log plot, perform linear regression, return gradient
    Data1D loglog;
    loglog.initialise(radiusOfGyration_.size(), false); // Add errors to this at some point?

    // Generate Data1D
    for (const auto &[clusterID, rg] : radiusOfGyration_)
        loglog.addPoint(std::log(radiusOfGyration_[clusterID]), std::log(clusterMasses_[clusterID]));

    // Perform linear regression
    fractalDimension_ = (Regression::linear(loglog, radiusOfGyration_.size()));

    if (saveSizeDist_)
    {
        LineParser parser;
        parser.openOutput(std::format("{}.sizedist.txt", targetConfiguration_->niceName()));
        parser.writeLineF("# Analysis for sites: {} - {}\n", a_->parent()->name(), b_->parent()->name());
        parser.writeLineF("\n=== Cluster Size Distribution ===\nCluster Size : no. of clusters\n");
        for (const auto &[clusterSize, mems] : sizeDistribution_)
            parser.writeLineF("{} : {}\n", clusterSize, mems.size());
    }
    if (saveMassDist_)
    {
        LineParser parser;
        parser.openOutput(std::format("{}.massdist.txt", targetConfiguration_->niceName()));
        parser.writeLineF("# Analysis for sites: {} - {}\n", a_->parent()->name(), b_->parent()->name());
        parser.writeLineF("\n=== Cluster Mass Distribution ===\nCluster Mass : no. of clusters\n");
        for (const auto &[clusterMass, mems] : massDistribution_)
            parser.writeLineF("{:.3f} : {}\n", clusterMass, mems.size());
    }
    if (saveRgMass_)
    {
        LineParser parser;
        parser.openOutput(std::format("{}.massRg.txt", targetConfiguration_->niceName()));
        parser.writeLineF("# Analysis for sites: {} - {}\n", a_->parent()->name(), b_->parent()->name());
        parser.writeLineF("# Fractal dimension: {}", fractalDimension_);
        parser.writeLineF("\n=== Mass - Radius of gyration ===\nCluster Mass : Radii\n");
        for (const auto &[clusterID, radius] : radiusOfGyration_)
            parser.writeLineF("{} : {}\n", clusterMasses_[clusterID], radius);
    }

    return ExecutionResult::Success;
}

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

void ClusteringModule::generateClustersConfig(Dissolve &dissolve, int displaySize, int displayID)
{
    if (clusterConfig_.generator().node("clusters"))
        clusterConfig_.clear();
    else
        clusterConfig_.setName("clusters");

    // Molecule transfer only works with a generator?
    clusterConfig_.generator().createRootNode<CopyGeneratorNode>("clusters", targetConfiguration_);
    clusterConfig_.generate({dissolve.worldPool(), dissolve});
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

    clusterConfig_.incrementContentsVersion();
    clusterConfig_.updateObjectRelationships();

    if (clusterConfig_.nAtoms() == 0)
        Messenger::error("No clusters!");
    else
        Messenger::print("Cluster visualisation generated");
}

void ClusteringModule::calculateCN(int displaySize, int displayID)
{
    std::map<const SpeciesSite *, int> instances;
    clusterSpeciesCoordNo_.clear();

    if (displaySize == 0)
    {
        // Start iterating through the cluster map.
        for (auto const &[_, mems] : clusterMap_)
        {
            // Iterate through each member of the cluster
            for (auto const &mem : mems)
            {
                // For each species found, increment the total number of that species by one.
                instances[mem->parent()]++;

                // Find the member in the neighbour map,
                for (auto const &[memNbr, index] : neighbourMap_[mem])
                    clusterSpeciesCoordNo_[mem->parent()][memNbr->parent()]++;
            }
        }
    }
    if (displaySize != 0 && displayID == 0)
    {
        // Same as above but only for given size
        for (const auto &[clusterID, mems] : clusterMap_)
            if (mems.size() == displaySize)
                for (auto const &mem : mems)
                {
                    instances[mem->parent()]++;
                    for (auto const &[memNbr, index] : neighbourMap_[mem])
                        clusterSpeciesCoordNo_[mem->parent()][memNbr->parent()]++;
                }
    }
    else if (displaySize != 0 && displayID != 0)
    {
        // Just the given clusterID
        for (auto const &mem : clusterMap_[displayID])
        {
            instances[mem->parent()]++;
            for (auto const &[memNbr, index] : neighbourMap_[mem])
                clusterSpeciesCoordNo_[mem->parent()][memNbr->parent()]++;
        }
    }
    // Average the coordination numbers
    for (const auto &[siteA, num] : instances)
        for (const auto &[siteB, coordNo] : clusterSpeciesCoordNo_[siteA])
            clusterSpeciesCoordNo_[siteA][siteB] /= num;
}