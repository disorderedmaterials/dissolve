// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "analyser/siteFilter.h"
#include "analyser/siteSelector.h"
#include "base/processPool.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "generator/context.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "module/module.h"
#include <unordered_set>

// Forward Declarations
class Atom;

// Clustering Module
class ClusteringModule : public Module
{
    public:
    ClusteringModule();
    ~ClusteringModule() override = default;

    /*
     * Definition
     */
    private:
    // Map of site origin to usually hydrogen indicies though does not strictly need to be to hydrogen if using NETA
    std::unordered_map<const SpeciesSite *, std::unordered_set<int>> directionIndexes_;
    // Whether we're using angle constraints for hydrogen bonding
    bool directional_{false};
    // Maximum acceptable angle when selecting directional bonds
    double angleDev_{20};
    // Specifies for two different selected sites (A, B) if A-A and B-B contribute as well
    bool selfClustering_{false};
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Sites involved in the cluster
    const SpeciesSite *a_{nullptr}, *b_{nullptr};
    // Maximum allowed distance for the two sites to be considered in the same cluster
    double cutoff_{3.0};
    // Symmetric map of all sites to all neighbour sites
    Analyser::SiteMap neighbourMap_{};
    // Map of every member in every cluster
    std::map<int, std::vector<const Site *>> clusterMap_{};
    // Map of every molecule member in every cluster
    std::map<int, std::vector<std::shared_ptr<const Molecule>>> molClusterMap_{};
    // Map of size to no. of clusters of that size
    std::map<int, std::vector<int>> sizeDistribution_{};
    // Map of cluster ID to mass of cluster
    std::map<int, double> clusterMasses_{};
    // Map of mass to vector of cluster IDs (useful for processing)
    std::map<double, std::vector<int>> massDistribution_{};
    // {Site A : {Site B, C ... : Coordination Number for each pair}} where Site A - B, C ... are all the combinations defined
    std::map<const SpeciesSite *, std::map<const SpeciesSite *, double>> clusterSpeciesCoordNo_;
    // Minimum cluster size to count in coordination number calc
    int minCNSize_{0};
    // Maximum cluster size to count in coordination number calc
    int maxCNSize_{0};
    // Map of cluster ID to radius of gyration
    std::map<int, double> radiusOfGyration_;
    // Minimum size to include in radius of gyration calculation
    int gyrationMinSize_{10};
    // Map of cluster ID to CoM vector from reference site (first member in cluster map) - Required in Rg calc. Might be useful
    // down the road
    std::map<int, Vec3<double>> clusterCoM_;
    // Fractal dimension from linear regression of LogLog Radius of gyration - cluster mass
    double fractalDimension_{-1};
    // Cluster display configuration
    Configuration clusterConfig_;
    // Export keywords
    bool saveSizeDist_{false};
    bool saveMassDist_{false};
    bool saveRgMass_{false};

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
    // Recursion for cluster generation
    void buildCluster(const Site *startSite, std::unordered_set<const Site *> &visited);

    public:
    // Set up module for processings
    bool setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals) override;
    // Generation of the cluster visualisation configuration
    void generateClustersConfig(Dissolve &dissolve, int displaySize, int displayID);
    // Calculate the coordination numbers for pairs in the current cluster config
    void calculateCN(int displaySize, int displayID);
    // Get the CNs
    std::map<const SpeciesSite *, std::map<const SpeciesSite *, double>> &getCNs() { return clusterSpeciesCoordNo_; }
    // Get the size distribution
    std::map<int, std::vector<int>> &getSizeDistribution() { return sizeDistribution_; }
    // Get the local configuration for viewing
    Configuration *getClusterConfig() { return &clusterConfig_; }
};
