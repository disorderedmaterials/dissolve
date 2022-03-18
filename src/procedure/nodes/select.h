// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "math/range.h"
#include "procedure/nodes/node.h"
#include <memory>
#include <set>

// Forward Declarations
class DynamicSiteProcedureNode;
class SequenceProcedureNode;
class Element;
class Molecule;
class SiteStack;
class Species;
class SpeciesSite;

// Select Node
class SelectProcedureNode : public ProcedureNode
{
    public:
    explicit SelectProcedureNode(std::vector<const SpeciesSite *> sites = {}, bool axesRequired = false);

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;

    /*
     * Selection Targets
     */
    private:
    // Vector of sites to select
    std::vector<const SpeciesSite *> speciesSites_;
    // Vector of DynamicSites to select, if any
    std::vector<std::shared_ptr<DynamicSiteProcedureNode>> dynamicSites_;
    // Whether sites must have a defined orientation
    bool axesRequired_;

    public:
    // Return vector of sites to select
    std::vector<const SpeciesSite *> &speciesSites();
    // Return whether sites must have a defined orientation
    bool axesRequired();
    // Return Nodes owned by this node
    std::vector<ConstNodeRef> children() const override;

    /*
     * Selection Control
     */
    private:
    // Other sites (nodes) which will exclude one of our sites if it has the same Molecule parent
    std::vector<std::shared_ptr<const SelectProcedureNode>> sameMoleculeExclusions_;
    // Molecules currently excluded from selection
    std::vector<std::shared_ptr<const Molecule>> excludedMolecules_;
    // Other sites (nodes) which will exclude one of our sites if it is the same site
    std::vector<std::shared_ptr<const SelectProcedureNode>> sameSiteExclusions_;
    // Sites currently excluded from selection
    std::set<const Site *> excludedSites_;
    // Molecule (from site) in which the site must exist
    std::shared_ptr<const SelectProcedureNode> sameMolecule_;
    // Site to use for distance check
    std::shared_ptr<const SelectProcedureNode> distanceReferenceSite_;
    // Range of distance to allow from distance reference site (if limiting)
    Range inclusiveDistanceRange_;

    public:
    // Set other sites (nodes) which will exclude one of our sites if it has the same Molecule parent
    void setSameMoleculeExclusions(std::vector<std::shared_ptr<const SelectProcedureNode>> exclusions);
    // Set other sites (nodes) which will exclude one of our sites if it is the same site
    void setSameSiteExclusions(std::vector<std::shared_ptr<const SelectProcedureNode>> exclusions);
    // Return list of Molecules currently excluded from selection
    const std::vector<std::shared_ptr<const Molecule>> &excludedMolecules() const;
    // Return Molecule (from site) in which the site must exist
    std::shared_ptr<const Molecule> sameMoleculeMolecule();
    // Set site to use for distance check
    void setDistanceReferenceSite(std::shared_ptr<const SelectProcedureNode> site);
    // Set range of distance to allow from distance reference site
    void setInclusiveDistanceRange(Range range);

    /*
     * Selected Sites
     */
    private:
    // Vector of selected sites
    std::vector<const Site *> sites_;
    // Current Site index
    int currentSiteIndex_;
    // Number of selections made by the node
    int nSelections_;
    // Cumulative number of sites ever selected
    int nCumulativeSites_;

    public:
    // Return the number of available sites in the current stack, if any
    int nSitesInStack() const;
    // Return the average number of sites selected
    double nAverageSites() const;
    // Return the cumulative number of sites ever selected
    int nCumulativeSites() const;
    // Return current site
    const Site *currentSite() const;

    /*
     * Branch
     */
    private:
    // Branch for ForEach (if defined)
    std::shared_ptr<SequenceProcedureNode> forEachBranch_;

    public:
    // Return whether this node has a branch
    bool hasBranch() const override;
    // Return SequenceNode for the branch (if it exists)
    std::shared_ptr<SequenceProcedureNode> branch() override;
    // Add and return ForEach sequence
    std::shared_ptr<SequenceProcedureNode> addForEachBranch(ProcedureNode::NodeContext context);

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
    // Finalise any necessary data after execution
    bool finalise(const ProcedureContext &procedureContext) override;
};
