/*
    *** Procedure Node - Select
    *** src/procedure/nodes/select.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "procedure/nodes/select.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/sitereference.h"
#include "classes/species.h"
#include "keywords/types.h"
#include "procedure/nodes/dynamicsite.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"

SelectProcedureNode::SelectProcedureNode(SpeciesSite *site, bool axesRequired) : ProcedureNode(ProcedureNode::SelectNode)
{
    if (site)
        speciesSites_.append(site);
    axesRequired_ = axesRequired;
    inclusiveDistanceRange_.set(0.0, 5.0);

    keywords_.add("Target", new SpeciesSiteRefListKeyword(speciesSites_, axesRequired_), "Site",
                  "Add target site(s) to the selection");
    keywords_.add("Target", new DynamicSiteNodesKeyword(this, dynamicSites_, axesRequired_), "DynamicSite",
                  "Add a new dynamic site to the selection");
    keywords_.add("Target", new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, true), "SameMoleculeAsSite",
                  "Request that the selected site comes from the molecule containing the current site in the specified "
                  "SelectNode");
    keywords_.add("Target",
                  new NodeRefListKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, true, sameMoleculeExclusions_),
                  "ExcludeSameMolecule",
                  "Exclude sites from selection if they are present in the same molecule as the current site in the specified "
                  "SelectNode(s)");
    keywords_.add(
        "Target", new NodeRefListKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, true, sameSiteExclusions_),
        "ExcludeSameSite", "Exclude sites from selection if they are the current site in the specified SelectNode(s)");
    keywords_.add("Target", new NodeKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, true), "ReferenceSite",
                  "Site to use as reference point when determining inclusions / exclusions");
    keywords_.add("Target", new RangeKeyword(inclusiveDistanceRange_, Vec3Labels::MinMaxBinwidthlabels), "InclusiveRange",
                  "Distance range (from reference site) within which sites are selected (only if ReferenceSite is defined)");
    keywords_.add("HIDDEN", new NodeBranchKeyword(this, &forEachBranch_, ProcedureNode::AnalysisContext), "ForEach",
                  "Branch to run on each site selected");

    forEachBranch_ = nullptr;

    currentSiteIndex_ = -1;
    nCumulativeSites_ = 0;
    nSelections_ = 0;
    sameMolecule_ = nullptr;
    distanceReferenceSite_ = nullptr;
}

SelectProcedureNode::~SelectProcedureNode()
{
    // Remove the forEach branch
    if (forEachBranch_)
        delete forEachBranch_;

    // Delete any dynamic site nodes (as we are the owner)
    for (DynamicSiteProcedureNode *dynamicNode : dynamicSites_)
        delete dynamicNode;
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool SelectProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Selection Control
 */

// Return list of Molecules currently excluded from selection
const OrderedVector<std::shared_ptr<const Molecule>> &SelectProcedureNode::excludedMolecules() const
{
    return excludedMolecules_;
}

// List of Sites currently excluded from selection
const RefList<const Site> &SelectProcedureNode::excludedSites() const { return excludedSites_; }

// Return Molecule (from site) in which the site must exist
std::shared_ptr<const Molecule> SelectProcedureNode::sameMoleculeMolecule()
{
    if (!sameMolecule_)
        return nullptr;

    const Site *site = sameMolecule_->currentSite();
    if (!site)
    {
        Messenger::warn("Requested Molecule from SelectProcedureNode::sameMolecule_, but there is no current site.\n");
        return nullptr;
    }

    return site->molecule();
}

/*
 * Selected Sites
 */

// Return the number of available sites in the current stack, if any
int SelectProcedureNode::nSitesInStack() const { return sites_.nItems(); }

// Return the average number of sites selected
double SelectProcedureNode::nAverageSites() const { return double(nCumulativeSites_) / nSelections_; }

// Return the cumulative number of sites ever selected
int SelectProcedureNode::nCumulativeSites() const { return nCumulativeSites_; }

// Return current site
const Site *SelectProcedureNode::currentSite() const
{
    return (currentSiteIndex_ == -1 ? nullptr : sites_.constAt(currentSiteIndex_));
}

/*
 * Branch
 */

// Return whether this node has a branch
bool SelectProcedureNode::hasBranch() const { return (forEachBranch_ != nullptr); }
// Return SequenceNode for the branch (if it exists)
SequenceProcedureNode *SelectProcedureNode::branch() { return forEachBranch_; }

// Add and return ForEach sequence
SequenceProcedureNode *SelectProcedureNode::addForEachBranch(ProcedureNode::NodeContext context_)
{
    if (!forEachBranch_)
        forEachBranch_ = new SequenceProcedureNode(context_, procedure(), this);

    return forEachBranch_;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool SelectProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Check for at least one site being defined
    if ((speciesSites_.nItems() == 0) && (dynamicSites_.nItems() == 0))
        return Messenger::error("No sites are defined in the Select node '{}'.\n", name());

    // Prep some variables
    nSelections_ = 0;
    nCumulativeSites_ = 0;

    // If one exists, prepare the ForEach branch nodes
    if (forEachBranch_ && (!forEachBranch_->prepare(cfg, prefix, targetList)))
        return false;

    // Prepare any dynamic site nodes
    for (DynamicSiteProcedureNode *dynamicNode : dynamicSites_)
        if (!dynamicNode->prepare(cfg, prefix, targetList))
            return false;

    // Retrieve data from keywords
    sameMolecule_ = keywords_.retrieve<SelectProcedureNode *>("SameMoleculeAsSite");
    distanceReferenceSite_ = keywords_.retrieve<SelectProcedureNode *>("ReferenceSite");
    inclusiveDistanceRange_ = keywords_.retrieve<Range>("InclusiveRange");

    return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult SelectProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
                                                                std::string_view prefix, GenericList &targetList)
{
    // Create our arrays of sites
    sites_.clear();

    // Update our exclusion lists
    excludedMolecules_.clear();
    for (SelectProcedureNode *node : sameMoleculeExclusions_)
        if (node->currentSite())
            excludedMolecules_.insert(node->currentSite()->molecule());

    excludedSites_.clear();
    for (SelectProcedureNode *node : sameSiteExclusions_)
        if (node->currentSite())
            excludedSites_.addUnique(node->currentSite());

    // Get required Molecule parent, if requested
    std::shared_ptr<const Molecule> moleculeParent = sameMolecule_ ? sameMoleculeMolecule() : nullptr;

    // Site to use as distance reference point (if any)
    const Site *distanceRef = distanceReferenceSite_ ? distanceReferenceSite_->currentSite() : nullptr;

    /*
     * Add sites from specified Species/Sites
     */
    double r;
    for (SpeciesSite *site : speciesSites_)
    {
        const SiteStack *siteStack = cfg->siteStack(site);
        if (siteStack == nullptr)
            return ProcedureNode::Failure;

        for (int n = 0; n < siteStack->nSites(); ++n)
        {
            const Site *site = &siteStack->site(n);

            // Check Molecule inclusion / exclusions
            if (moleculeParent)
            {
                if (site->molecule() != moleculeParent)
                    continue;
            }
            else if (find(excludedMolecules_.begin(), excludedMolecules_.end(), site->molecule()) != excludedMolecules_.end())
                continue;

            // Check Site exclusions
            if (excludedSites_.contains(site))
                continue;

            // Check distance from reference site (if defined)
            if (distanceRef)
            {
                r = cfg->box()->minimumDistance(site->origin(), distanceRef->origin());
                if (!inclusiveDistanceRange_.contains(r))
                    continue;
            }

            // All OK, so add site
            sites_.add(site);
        }
    }

    /*
     * Add dynamically-generated sites.
     * Call each DynamicSiteProcedureNode's execute function in turn, adding any generated sites to our reference array
     * afterwards.
     */
    for (DynamicSiteProcedureNode *dynamicNode : dynamicSites_)
    {
        if (dynamicNode->execute(procPool, cfg, prefix, targetList) == ProcedureNode::Failure)
            return ProcedureNode::Failure;

        const Array<Site> &generatedSites = dynamicNode->generatedSites();
        for (int n = 0; n < generatedSites.nItems(); ++n)
            sites_.add(&generatedSites.constAt(n));
    }

    // Set first site index and increase selections counter
    currentSiteIndex_ = (sites_.nItems() == 0 ? -1 : 0);
    ++nSelections_;

    // If a ForEach branch has been defined, process it for each of our sites in turn. Otherwise, we're done.
    if (forEachBranch_)
    {
        for (currentSiteIndex_ = 0; currentSiteIndex_ < sites_.nItems(); ++currentSiteIndex_)
        {
            ++nCumulativeSites_;

            // If the branch fails at any point, return failure here.  Otherwise, continue the loop
            if (forEachBranch_->execute(procPool, cfg, prefix, targetList) == ProcedureNode::Failure)
                return ProcedureNode::Failure;
        }
    }

    return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool SelectProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // If one exists, finalise the ForEach branch nodes
    if (forEachBranch_ && (!forEachBranch_->finalise(procPool, cfg, prefix, targetList)))
        return false;

    // Finalise any dynamic site nodes
    for (DynamicSiteProcedureNode *dynamicNode : dynamicSites_)
        if (!dynamicNode->finalise(procPool, cfg, prefix, targetList))
            return false;

    // Print out summary information
    Messenger::print("Select - Site '{}': Number of selections made = {} (last contained {} sites).\n", name(), nSelections_,
                     sites_.nItems());
    Messenger::print("Select - Site '{}': Average number of sites selected per selection = {:.2f}.\n", name(),
                     nSelections_ == 0 ? 0 : double(nCumulativeSites_) / nSelections_);
    Messenger::print("Select - Site '{}': Cumulative number of sites selected = {}.\n", name(), nCumulativeSites_);

    return true;
}
