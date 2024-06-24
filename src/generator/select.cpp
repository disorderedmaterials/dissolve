// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/select.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/siteReference.h"
#include "classes/species.h"
#include "expression/variable.h"
#include "generator/sequence.h"
#include "keywords/node.h"
#include "keywords/nodeBranch.h"
#include "keywords/nodeVector.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include <algorithm>

SelectGeneratorNode::SelectGeneratorNode(std::vector<const SpeciesSite *> sites, bool axesRequired)
    : GeneratorNode(NodeType::Select), speciesSites_(std::move(sites)), axesRequired_(axesRequired),
      forEachBranch_(*this, "ForEach")
{
    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<SpeciesSiteVectorKeyword>("Site", "Add target site(s) to the selection", speciesSites_, axesRequired_);

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<NodeKeyword<SelectGeneratorNode>>(
        "SameMoleculeAsSite",
        "Request that the selected site comes from the molecule containing the current site in the specified "
        "SelectNode",
        sameMolecule_, this, NodeTypeVector{NodeType::Select});
    keywords_.add<NodeVectorKeyword<SelectGeneratorNode>>(
        "ExcludeSameMolecule",
        "Exclude sites from selection if they are present in the same molecule as the current site in the specified "
        "SelectNode(s)",
        sameMoleculeExclusions_, this, NodeTypeVector{NodeType::Select});
    keywords_.add<NodeVectorKeyword<SelectGeneratorNode>>(
        "ExcludeSameSite", "Exclude sites from selection if they are the current site in the specified SelectNode(s)",
        sameSiteExclusions_, this, NodeTypeVector{NodeType::Select});
    keywords_.add<NodeKeyword<SelectGeneratorNode>>("ReferenceSite",
                                                    "Site to use as reference point when determining inclusions / exclusions",
                                                    distanceReferenceSite_, this, NodeTypeVector{NodeType::Select});
    keywords_.add<RangeKeyword>(
        "InclusiveRange",
        "Distance range (from reference site) within which sites are selected (only if ReferenceSite is defined)",
        inclusiveDistanceRange_, Vec3Labels::MinMaxBinwidthlabels);

    keywords_.addHidden<NodeBranchKeyword>("ForEach", "Branch to run on each site selected", forEachBranch_);

    nSelectedParameter_ = addParameter("nSelected");
    siteIndexParameter_ = addParameter("siteIndex");
    stackIndexParameter_ = addParameter("stackIndex");
    indexParameter_ = addParameter("index");
}

/*
 * Selection Targets
 */

// Return vector of sites to select
std::vector<const SpeciesSite *> &SelectGeneratorNode::speciesSites() { return speciesSites_; }

// Return whether sites must have a defined orientation
bool SelectGeneratorNode::axesRequired() { return axesRequired_; }

/*
 * Selection Control
 */

// Set other sites (nodes) which will exclude one of our sites if it has the same Molecule parent
void SelectGeneratorNode::setSameMoleculeExclusions(ConstNodeVector<SelectGeneratorNode> exclusions)
{
    sameMoleculeExclusions_ = std::move(exclusions);
}

// Set other sites (nodes) which will exclude one of our sites if it is the same site
void SelectGeneratorNode::setSameSiteExclusions(ConstNodeVector<SelectGeneratorNode> exclusions)
{
    sameSiteExclusions_ = std::move(exclusions);
}

// Return list of Molecules currently excluded from selection
const std::vector<std::shared_ptr<const Molecule>> &SelectGeneratorNode::excludedMolecules() const
{
    return excludedMolecules_;
}

// Return Molecule (from site) in which the site must exist
std::shared_ptr<const Molecule> SelectGeneratorNode::sameMoleculeMolecule()
{
    if (!sameMolecule_)
        return nullptr;

    const auto site = sameMolecule_->currentSite();
    if (!site)
    {
        Messenger::warn("Requested Molecule from SelectGeneratorNode::sameMolecule_, but there is no current site.\n");
        return nullptr;
    }

    return site->get().molecule();
}

// Set site to use for distance check
void SelectGeneratorNode::setDistanceReferenceSite(std::shared_ptr<const SelectGeneratorNode> site)
{
    distanceReferenceSite_ = site;
}

// Set range of distance to allow from distance reference site
void SelectGeneratorNode::setInclusiveDistanceRange(Range range) { inclusiveDistanceRange_ = range; }

/*
 * Selected Sites
 */

// Return EnumOptions for SelectionPopulation
EnumOptions<SelectGeneratorNode::SelectionPopulation> SelectGeneratorNode::selectionPopulations()
{
    return EnumOptions<SelectGeneratorNode::SelectionPopulation>(
        "SelectionPopulation", {{SelectGeneratorNode::SelectionPopulation::Available, "Available"},
                                {SelectGeneratorNode::SelectionPopulation::Average, "Average"}});
}

// Return vector of selected sites
const std::vector<std::tuple<const Site &, int, int>> &SelectGeneratorNode::sites() const { return sites_; }

// Return the number of available sites in the current stack, if any
int SelectGeneratorNode::nSitesInStack() const { return sites_.size(); }

// Return the average number of sites selected
double SelectGeneratorNode::nAverageSites() const { return double(nCumulativeSites_) / nSelections_; }

// Return the cumulative number of sites ever selected
unsigned long int SelectGeneratorNode::nCumulativeSites() const { return nCumulativeSites_; }

// Return average number of sites available per selection, before any distance pruning
double SelectGeneratorNode::nAvailableSitesAverage() const { return double(nAvailableSites_) / nSelections_; }

// Return current site
OptionalReferenceWrapper<const Site> SelectGeneratorNode::currentSite() const { return currentSite_; }

/*
 * Branch
 */

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<GeneratorNodeSequence> SelectGeneratorNode::branch() { return forEachBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool SelectGeneratorNode::prepare(const ProcedureContext &procedureContext)
{
    // Check for at least one site being defined
    if (speciesSites_.empty())
        return Messenger::error("No sites are defined in the Select node '{}'.\n", name());

    // Prep some variables
    nSelections_ = 0;
    nCumulativeSites_ = 0;
    nAvailableSites_ = 0;

    // If one exists, prepare the ForEach branch nodes
    if (!forEachBranch_.prepare(procedureContext))
        return false;

    return true;
}

// Execute node
bool SelectGeneratorNode::execute(const ProcedureContext &procedureContext)
{
    // Create our site vector
    sites_.clear();

    // Update our exclusion lists
    excludedMolecules_.clear();
    for (auto node : sameMoleculeExclusions_)
        if (node->currentSite())
            excludedMolecules_.emplace_back(node->currentSite()->get().molecule());

    excludedSites_.clear();
    for (auto node : sameSiteExclusions_)
        if (node->currentSite())
            excludedSites_.insert(&node->currentSite()->get());

    // Get required Molecule parent, if requested
    std::shared_ptr<const Molecule> moleculeParent = sameMolecule_ ? sameMoleculeMolecule() : nullptr;

    // Site to use as distance reference point (if any)
    const auto distanceRef = distanceReferenceSite_ ? distanceReferenceSite_->currentSite() : std::nullopt;

    /*
     * Add sites from specified Species/Sites
     */
    auto siteIndex = 0;
    for (auto *spSite : speciesSites_)
    {
        const auto *siteStack = procedureContext.configuration()->siteStack(spSite);
        if (siteStack == nullptr)
            return false;

        for (auto n = 0; n < siteStack->nSites(); ++n)
        {
            const auto &site = siteStack->site(n);
            ++siteIndex;

            // Check Molecule inclusion / exclusions
            if (moleculeParent)
            {
                if (site.molecule() != moleculeParent)
                    continue;
            }
            else if (std::find(excludedMolecules_.begin(), excludedMolecules_.end(), site.molecule()) !=
                     excludedMolecules_.end())
                continue;

            // Check Site exclusions
            if (std::find(excludedSites_.begin(), excludedSites_.end(), &site) != excludedSites_.end())
                continue;

            // Increment available sites now, before distance pruning
            ++nAvailableSites_;

            // Check distance from reference site (if defined)
            if (distanceRef)
            {
                if (!inclusiveDistanceRange_.contains(
                        procedureContext.configuration()->box()->minimumDistance(site.origin(), distanceRef->get().origin())))
                    continue;
            }

            // All OK, so add site, its global index among all site stacks, and index in its stack (1...N numbering)
            sites_.emplace_back(site, siteIndex, n + 1);
        }
    }

    // Increase selections counter
    ++nSelections_;

    // Update nSelected parameter
    nSelectedParameter_->setValue(int(sites_.size()));

    // If a ForEach branch has been defined, process it for each of our sites in turn. Otherwise, we're done.
    currentSite_ = std::nullopt;
    if (!forEachBranch_.empty())
    {
        auto index = 1;
        for (const auto &siteInfo : sites_)
        {
            currentSite_ = std::get<0>(siteInfo);
            siteIndexParameter_->setValue(std::get<1>(siteInfo));
            stackIndexParameter_->setValue(std::get<2>(siteInfo));
            indexParameter_->setValue(index++);

            ++nCumulativeSites_;

            // If the branch fails at any point, return failure here.  Otherwise, continue the loop
            if (!forEachBranch_.execute(procedureContext))
                return false;
        }
    }

    return true;
}

// Finalise any necessary data after execution
bool SelectGeneratorNode::finalise(const ProcedureContext &procedureContext)
{
    // If one exists, finalise the ForEach branch nodes
    if (!forEachBranch_.finalise(procedureContext))
        return false;

    // Print out summary information
    Messenger::print("[Select] - Site '{}': Number of selections made = {} (last contained {} sites).\n", name(), nSelections_,
                     sites_.size());
    Messenger::print("[Select] - Site '{}': Average number of sites selected per selection = {:.2f}.\n", name(),
                     nSelections_ == 0 ? 0 : double(nCumulativeSites_) / nSelections_);
    Messenger::print("[Select] - Site '{}': Average number of sites available per selection = {:.2f}.\n", name(),
                     nSelections_ == 0 ? 0 : double(nAvailableSites_) / nSelections_);

    Messenger::print("[Select] - Site '{}': Cumulative number of sites selected = {}.\n", name(), nCumulativeSites_);

    return true;
}
