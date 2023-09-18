// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/select.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/siteReference.h"
#include "classes/species.h"
#include "expression/variable.h"
#include "keywords/node.h"
#include "keywords/nodeBranch.h"
#include "keywords/nodeVector.h"
#include "keywords/range.h"
#include "keywords/speciesSiteVector.h"
#include "procedure/nodes/sequence.h"
#include <algorithm>

SelectProcedureNode::SelectProcedureNode(std::vector<const SpeciesSite *> sites, ProcedureNode::NodeContext forEachContext,
                                         bool axesRequired)
    : ProcedureNode(ProcedureNode::NodeType::Select, {ProcedureNode::AnalysisContext, ProcedureNode::GenerationContext}),
      speciesSites_(std::move(sites)), axesRequired_(axesRequired), forEachBranch_(forEachContext, *this, "ForEach")
{
    keywords_.setOrganisation("Options", "Sites");
    keywords_.add<SpeciesSiteVectorKeyword>("Site", "Add target site(s) to the selection", speciesSites_, axesRequired_);

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<NodeKeyword<SelectProcedureNode>>(
        "SameMoleculeAsSite",
        "Request that the selected site comes from the molecule containing the current site in the specified "
        "SelectNode",
        sameMolecule_, this, ProcedureNode::NodeType::Select, true);
    keywords_.add<NodeVectorKeyword<SelectProcedureNode>>(
        "ExcludeSameMolecule",
        "Exclude sites from selection if they are present in the same molecule as the current site in the specified "
        "SelectNode(s)",
        sameMoleculeExclusions_, this, ProcedureNode::NodeType::Select, true);
    keywords_.add<NodeVectorKeyword<SelectProcedureNode>>(
        "ExcludeSameSite", "Exclude sites from selection if they are the current site in the specified SelectNode(s)",
        sameSiteExclusions_, this, ProcedureNode::NodeType::Select, true);
    keywords_.add<NodeKeyword<SelectProcedureNode>>("ReferenceSite",
                                                    "Site to use as reference point when determining inclusions / exclusions",
                                                    distanceReferenceSite_, this, ProcedureNode::NodeType::Select, true);
    keywords_.add<RangeKeyword>(
        "InclusiveRange",
        "Distance range (from reference site) within which sites are selected (only if ReferenceSite is defined)",
        inclusiveDistanceRange_, Vec3Labels::MinMaxBinwidthlabels);

    keywords_.addHidden<NodeBranchKeyword>("ForEach", "Branch to run on each site selected", forEachBranch_);

    // Need to make parameters_ private as a next step, to prevent direct initialisation like this... (use addParameter()
    // instead)
    nSelectedParameter_ = addParameter("nSelected");
    siteIndexParameter_ = addParameter("siteIndex");
    stackIndexParameter_ = addParameter("stackIndex");
    indexParameter_ = addParameter("index");
}

/*
 * Selection Targets
 */

// Return vector of sites to select
std::vector<const SpeciesSite *> &SelectProcedureNode::speciesSites() { return speciesSites_; }

// Return whether sites must have a defined orientation
bool SelectProcedureNode::axesRequired() { return axesRequired_; }

/*
 * Selection Control
 */

// Set other sites (nodes) which will exclude one of our sites if it has the same Molecule parent
void SelectProcedureNode::setSameMoleculeExclusions(ConstNodeVector<SelectProcedureNode> exclusions)
{
    sameMoleculeExclusions_ = std::move(exclusions);
}

// Set other sites (nodes) which will exclude one of our sites if it is the same site
void SelectProcedureNode::setSameSiteExclusions(ConstNodeVector<SelectProcedureNode> exclusions)
{
    sameSiteExclusions_ = std::move(exclusions);
}

// Return list of Molecules currently excluded from selection
const std::vector<std::shared_ptr<const Molecule>> &SelectProcedureNode::excludedMolecules() const
{
    return excludedMolecules_;
}

// Return Molecule (from site) in which the site must exist
std::shared_ptr<const Molecule> SelectProcedureNode::sameMoleculeMolecule()
{
    if (!sameMolecule_)
        return nullptr;

    const auto site = sameMolecule_->currentSite();
    if (!site)
    {
        Messenger::warn("Requested Molecule from SelectProcedureNode::sameMolecule_, but there is no current site.\n");
        return nullptr;
    }

    return site->get().molecule();
}

// Set site to use for distance check
void SelectProcedureNode::setDistanceReferenceSite(std::shared_ptr<const SelectProcedureNode> site)
{
    distanceReferenceSite_ = site;
}

// Set range of distance to allow from distance reference site
void SelectProcedureNode::setInclusiveDistanceRange(Range range) { inclusiveDistanceRange_ = range; }

/*
 * Selected Sites
 */

// Return EnumOptions for SelectionPopulation
EnumOptions<SelectProcedureNode::SelectionPopulation> SelectProcedureNode::selectionPopulations()
{
    return EnumOptions<SelectProcedureNode::SelectionPopulation>(
        "SelectionPopulation", {{SelectProcedureNode::SelectionPopulation::Available, "Available"},
                                {SelectProcedureNode::SelectionPopulation::Average, "Average"}});
}

// Return vector of selected sites
const std::vector<std::tuple<const Site &, int, int>> &SelectProcedureNode::sites() const { return sites_; }

// Return the number of available sites in the current stack, if any
int SelectProcedureNode::nSitesInStack() const { return sites_.size(); }

// Return the average number of sites selected
double SelectProcedureNode::nAverageSites() const { return double(nCumulativeSites_) / nSelections_; }

// Return the cumulative number of sites ever selected
unsigned long int SelectProcedureNode::nCumulativeSites() const { return nCumulativeSites_; }

// Return average number of sites available per selection, before any distance pruning
double SelectProcedureNode::nAvailableSitesAverage() const { return double(nAvailableSites_) / nSelections_; }

// Return current site
OptionalReferenceWrapper<const Site> SelectProcedureNode::currentSite() const { return currentSite_; }

/*
 * Branch
 */

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> SelectProcedureNode::branch() { return forEachBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool SelectProcedureNode::prepare(const ProcedureContext &procedureContext)
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
bool SelectProcedureNode::execute(const ProcedureContext &procedureContext)
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
            else if (find(excludedMolecules_.begin(), excludedMolecules_.end(), site.molecule()) != excludedMolecules_.end())
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
bool SelectProcedureNode::finalise(const ProcedureContext &procedureContext)
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
