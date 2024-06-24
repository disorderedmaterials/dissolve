// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
#include "generator/sequence.h"
#include "math/range.h"
#include <memory>
#include <set>

// Forward Declarations
class Molecule;
class SiteStack;
class Site;
class SpeciesSite;

// Select Node
class SelectGeneratorNode : public GeneratorNode
{
    public:
    explicit SelectGeneratorNode(std::vector<const SpeciesSite *> sites = {}, bool axesRequired = false);

    /*
     * Parameters
     */
    private:
    // Pointers to individual parameters
    std::shared_ptr<ExpressionVariable> nSelectedParameter_, siteIndexParameter_, stackIndexParameter_, indexParameter_;

    /*
     * Selection Targets
     */
    private:
    // Vector of sites to select
    std::vector<const SpeciesSite *> speciesSites_;
    // Whether sites must have a defined orientation
    bool axesRequired_;

    public:
    // Return vector of sites to select
    std::vector<const SpeciesSite *> &speciesSites();
    // Return whether sites must have a defined orientation
    bool axesRequired();

    /*
     * Selection Control
     */
    private:
    // Other sites (nodes) which will exclude one of our sites if it has the same Molecule parent
    ConstNodeVector<SelectGeneratorNode> sameMoleculeExclusions_;
    // Molecules currently excluded from selection
    std::vector<std::shared_ptr<const Molecule>> excludedMolecules_;
    // Other sites (nodes) which will exclude one of our sites if it is the same site
    ConstNodeVector<SelectGeneratorNode> sameSiteExclusions_;
    // Sites currently excluded from selection
    std::set<const Site *> excludedSites_;
    // Molecule (from site) in which the site must exist
    std::shared_ptr<const SelectGeneratorNode> sameMolecule_;
    // Site to use for distance check
    std::shared_ptr<const SelectGeneratorNode> distanceReferenceSite_;
    // Range of distance to allow from distance reference site (if limiting)
    Range inclusiveDistanceRange_{0.0, 5.0};

    public:
    // Set other sites (nodes) which will exclude one of our sites if it has the same Molecule parent
    void setSameMoleculeExclusions(ConstNodeVector<SelectGeneratorNode> exclusions);
    // Set other sites (nodes) which will exclude one of our sites if it is the same site
    void setSameSiteExclusions(ConstNodeVector<SelectGeneratorNode> exclusions);
    // Return list of Molecules currently excluded from selection
    const std::vector<std::shared_ptr<const Molecule>> &excludedMolecules() const;
    // Return Molecule (from site) in which the site must exist
    std::shared_ptr<const Molecule> sameMoleculeMolecule();
    // Set site to use for distance check
    void setDistanceReferenceSite(std::shared_ptr<const SelectGeneratorNode> site);
    // Set range of distance to allow from distance reference site
    void setInclusiveDistanceRange(Range range);

    /*
     * Selected Sites
     */
    private:
    // Vector of selected sites
    std::vector<std::tuple<const Site &, int, int>> sites_;
    // Current site
    OptionalReferenceWrapper<const Site> currentSite_;
    // Number of selections made by the node
    int nSelections_{0};
    // Cumulative number of sites ever selected
    unsigned long int nCumulativeSites_{0};
    // Total number of sites available per selection
    unsigned long int nAvailableSites_{0};

    public:
    // Selection Populations
    enum class SelectionPopulation
    {
        Average,
        Available
    };
    // Return EnumOptions for SelectionPopulation
    static EnumOptions<SelectionPopulation> selectionPopulations();
    // Return vector of selected sites
    const std::vector<std::tuple<const Site &, int, int>> &sites() const;
    // Return the number of available sites in the current stack, if any
    int nSitesInStack() const;
    // Return the average number of sites selected
    double nAverageSites() const;
    // Return the cumulative number of sites ever selected
    unsigned long int nCumulativeSites() const;
    // Return average number of sites available per selection, before any distance pruning
    double nAvailableSitesAverage() const;
    // Return current site
    OptionalReferenceWrapper<const Site> currentSite() const;

    /*
     * Branch
     */
    private:
    // Branch for ForEach
    GeneratorNodeSequence forEachBranch_;

    public:
    // Return the branch from this node (if it has one)
    OptionalReferenceWrapper<GeneratorNodeSequence> branch() override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
