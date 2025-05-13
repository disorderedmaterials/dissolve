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

    // If we have directional bonding, we need to check and determine index map for hydroxyl group
    if (directional_)
    {
        directionIndexes_.clear();
        for (const auto &s : {a_, b_})
        {
            if (s->type() == SpeciesSite::SiteType::Fragment)
            {
                // For a fragment site, we're relying on "#origin, -H(#other)" or similar, with hydrogens (or whatever desired
                // in theory should work) tagged as #other. If the number of hydrogens exceeds one, the definition should
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

Module::ExecutionResult ClusteringModule::process(ModuleContext &moduleContext)
{
    auto &moduleData = moduleContext.dissolve().processingModuleData();

    // Produce NeighbourMap
    neighbourMap_.clear();
    Analyser::SiteMap neighbourMapA, neighbourMapB;
    std::vector<const SpeciesSite *> aVec, bVec;

    // Decide if we're filtering everything by everything or one by the other
    if (a_ == b_ && selfClustering_)
    {
        Messenger::error("Sites are the same! Disabling self-clustering...");
        selfClustering_ = false;
    }
    else if (selfClustering_)
    {
        aVec.insert(aVec.end(), {a_, b_});
        bVec = aVec;
    }
    else
    {
        aVec.emplace_back(a_);
        bVec.emplace_back(b_);
    }

    SiteSelector selectionA(targetConfiguration_, std::vector<const SpeciesSite *>{aVec});
    SiteSelector selectionB(targetConfiguration_, std::vector<const SpeciesSite *>{bVec});
    const auto &siteVectorA = selectionA.sites();
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
                    auto angle = box->angleInDegrees(oOVec / oOVec.magnitude(), oHVec / oHVec.magnitude());

                    // Make sure we have the smallest angle possible
                    if (360.0 - angle < angle)
                        angle = 360.0 - angle;

                    if (angle <= angleDev_)
                    {
                        keep = true;
                        break;
                    }
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

    auto &sizeData = moduleData.realise<Data1D>("SizeDist", name());
    sizeData.clear();
    for (int i = sizeDistribution_.begin()->first; i <= sizeDistribution_.rbegin()->first; i++)
        sizeDistribution_.contains(i) ? sizeData.addPoint(i, sizeDistribution_[i].size()) : sizeData.addPoint(i, 0.1);

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

        // CoM mass weighted calc from reference site
        Vec3<double> massWeightedTotalVec{0, 0, 0};
        const auto refMol{clusterVec[0]}; // Reference as first member in cluster
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
        Messenger::error("No clusters! (Ignore at start-up)");
    else
        Messenger::print("Cluster visualisation generated");
}

// Calculates the coordination numbers for the clusters being viewed. Considers only the sites on a molecule that are involved
// in the cluster
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
    // Average the coordination numbers
    for (const auto &[siteA, num] : instances)
        for (const auto &[siteB, coordNo] : clusterSpeciesCoordNo_[siteA])
            clusterSpeciesCoordNo_[siteA][siteB] /= num;
}