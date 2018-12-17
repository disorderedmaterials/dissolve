/*
	*** Analysis Node - Select
	*** src/analyse/nodes/select.cpp
	Copyright T. Youngs 2012-2018

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

#include "analyse/nodes/select.h"
#include "analyse/nodes/dynamicsite.h"
#include "analyse/nodes/sequence.h"
#include "analyse/nodecontextstack.h"
#include "analyse/sitereference.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AnalysisSelectNode::AnalysisSelectNode(SpeciesSite* site) : AnalysisNode(AnalysisNode::SelectNode)
{
	if (site) speciesSites_.add(site);

	sameMolecule_= NULL;
	forEachBranch_ = NULL;
	currentSiteIndex_ = -1;
	nCumulativeSites_ = 0;
	nSelections_ = 0;
}

// Destructor
AnalysisSelectNode::~AnalysisSelectNode()
{
	// Remove the forEach branch
	if (forEachBranch_) delete forEachBranch_;

	// Delete any dynamic site nodes (as we are the owner)
	RefListIterator<AnalysisDynamicSiteNode,bool> dynamicIterator(dynamicSites_);
	while (AnalysisDynamicSiteNode* dynamicNode = dynamicIterator.iterate()) delete dynamicNode;
}

/*
 * Node Keywords
 */

// Node Keywords
const char* SelectNodeKeywords[] = { "DynamicSite", "EndSelect", "ExcludeSameMolecule", "ExcludeSameSite", "ForEach", "SameMoleculeAsSite", "Site" };

// Convert string to node keyword
AnalysisSelectNode::SelectNodeKeyword AnalysisSelectNode::selectNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisSelectNode::nSelectNodeKeywords; ++nk) if (DissolveSys::sameString(s, SelectNodeKeywords[nk])) return (AnalysisSelectNode::SelectNodeKeyword) nk;

	return AnalysisSelectNode::nSelectNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisSelectNode::selectNodeKeyword(AnalysisSelectNode::SelectNodeKeyword nk)
{
	return SelectNodeKeywords[nk];
}

/*
 * Selection Targets
 */

/*
 * Selection Control
 */

// Add "same molecule" exclusion
bool AnalysisSelectNode::addSameMoleculeExclusion(AnalysisSelectNode* node)
{
	if (sameMoleculeExclusions_.contains(node)) return false;
	else sameMoleculeExclusions_.add(node);

	return true;
}

// Return list of Molecules currently excluded from selection
const RefList<const Molecule,bool>& AnalysisSelectNode::excludedMolecules() const
{
	return excludedMolecules_;
}

// Add "same site" exclusion
bool AnalysisSelectNode::addSameSiteExclusion(AnalysisSelectNode* node)
{
	if (sameSiteExclusions_.contains(node)) return false;
	else sameSiteExclusions_.add(node);

	return true;
}

// List of Sites currently excluded from selection
const RefList<const Site,bool>& AnalysisSelectNode::excludedSites() const
{
	return excludedSites_;
}

// Return Molecule (from site) in which the site must exist
const Molecule* AnalysisSelectNode::sameMoleculeMolecule()
{
	if (!sameMolecule_) return NULL;

	const Site* site = sameMolecule_->currentSite();
	if (!site)
	{
		Messenger::warn("Requested Molecule from AnalysisSelectNode::sameMolecule_, but there is no current site.\n");
		return NULL;
	}

	return site->molecule();
}

/*
 * Selected Sites
 */

// Return the number of available sites in the current stack, if any
int AnalysisSelectNode::nSitesInStack() const
{
	return sites_.nItems();
}

// Return the average number of sites selected
double AnalysisSelectNode::nAverageSites() const
{
	return double(nCumulativeSites_) / nSelections_;
}

// Return the cumulative number of sites ever selected
int AnalysisSelectNode::nCumulativeSites() const
{
	return nCumulativeSites_;
}

// Return current site
const Site* AnalysisSelectNode::currentSite() const
{
	return (currentSiteIndex_ == -1 ? NULL : sites_.constAt(currentSiteIndex_));
}

/*
 * Branch
 */

// Add and return ForEach sequence
AnalysisSequenceNode* AnalysisSelectNode::addForEachBranch()
{
	if (!forEachBranch_) forEachBranch_ = new AnalysisSequenceNode();

	forEachBranch_->setParent(parent());

	return forEachBranch_;
}

// Add specified node to ForEach sequence
void AnalysisSelectNode::addToForEachBranch(AnalysisNode* node)
{
	addForEachBranch()->addNode(node);
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisSelectNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	nSelections_ = 0;
	nCumulativeSites_ = 0;

	// If one exists, prepare the ForEach branch nodes
	if (forEachBranch_ && (!forEachBranch_->prepare(cfg, prefix, targetList))) return false;

	// Prepare any dynamic site nodes
	RefListIterator<AnalysisDynamicSiteNode,bool> dynamicIterator(dynamicSites_);
	while (AnalysisDynamicSiteNode* dynamicNode = dynamicIterator.iterate()) if (!dynamicNode->prepare(cfg, prefix, targetList)) return false;

	return true;
}

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisSelectNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Create our arrays of sites
	sites_.clear();

	// Update our exclusion lists
	excludedMolecules_.clear();
	RefListIterator<AnalysisSelectNode,bool> moleculeExclusionIterator(sameMoleculeExclusions_);
	while (AnalysisSelectNode* node = moleculeExclusionIterator.iterate()) if (node->currentSite()) excludedMolecules_.addUnique(node->currentSite()->molecule());

	excludedSites_.clear();
	RefListIterator<AnalysisSelectNode,bool> siteExclusionIterator(sameSiteExclusions_);
	while (AnalysisSelectNode* node = siteExclusionIterator.iterate()) if (node->currentSite()) excludedSites_.addUnique(node->currentSite());

	// Get required Molecule parent, if requested
	const Molecule* moleculeParent = sameMolecule_ ? sameMoleculeMolecule() : NULL;

	/*
	 * Add sites from specified Species/Sites
	 */
	RefListIterator<SpeciesSite, bool> siteIterator(speciesSites_);
	while (SpeciesSite* site = siteIterator.iterate())
	{
		const SiteStack* siteStack = cfg->siteStack(site);
		if (siteStack == NULL) return AnalysisNode::Failure;

		for (int n=0; n<siteStack->nSites(); ++n)
		{
			const Site* site = &siteStack->site(n);

			// Check Molecule inclusion / exclusions
			if (moleculeParent)
			{
				if (site->molecule() != moleculeParent) continue; 
			}
			else if (excludedMolecules_.contains(site->molecule())) continue;

			// Check Site exclusions
			if (excludedSites_.contains(site)) continue;

			// All OK, so add site
			sites_.add(site);
		}
	}

	/*
	 * Add dynamically-generated sites.
	 * Call each AnalysisDynamicSiteNode's execute function in turn, adding any generated sites to our reference array afterwards.
	 */
	RefListIterator<AnalysisDynamicSiteNode,bool> dynamicIterator(dynamicSites_);
	while (AnalysisDynamicSiteNode* dynamicNode = dynamicIterator.iterate())
	{
		if (dynamicNode->execute(procPool, cfg, prefix, targetList) == AnalysisNode::Failure) return AnalysisNode::Failure;

		const Array<Site>& generatedSites = dynamicNode->generatedSites();
		for (int n=0; n<generatedSites.nItems(); ++n) sites_.add(&generatedSites.constAt(n));
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
			if (forEachBranch_->execute(procPool, cfg, prefix, targetList) == AnalysisNode::Failure) return AnalysisNode::Failure;
		}
	}

	return AnalysisNode::Success;
}

// Finalise any necessary data after execution
bool AnalysisSelectNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// If one exists, finalise the ForEach branch nodes
	if (forEachBranch_ && (!forEachBranch_->finalise(procPool, cfg, prefix, targetList))) return false;

	// Finalise any dynamic site nodes
	RefListIterator<AnalysisDynamicSiteNode,bool> dynamicIterator(dynamicSites_);
	while (AnalysisDynamicSiteNode* dynamicNode = dynamicIterator.iterate()) if (!dynamicNode->finalise(procPool, cfg, prefix, targetList)) return false;

	// Print out summary information
	Messenger::print("Select - Site '%s': Number of selections made = %i (last contained %i sites).\n", name(), nSelections_ , sites_.nItems());
	Messenger::print("Select - Site '%s': Average number of sites selected per selection = %.2f.\n", name(), nSelections_ == 0 ? 0 : double(nCumulativeSites_)/nSelections_);
	Messenger::print("Select - Site '%s': Cumulative number of sites selected = %i.\n", name(), nCumulativeSites_);

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisSelectNode::read(LineParser& parser, const CoreData& coreData, NodeContextStack& contextStack)
{
	// The current line in the parser may contain a specific label for the sites we are to select, which we set as our node name
	setName(parser.nArgs() == 2 ? parser.argc(1) : contextStack.nextSelectName());

	// Add ourselves to the context stack
	if (!contextStack.add(this)) return Messenger::error("Error adding Select node '%s' to context stack.\n", name());

	AnalysisDynamicSiteNode* dynamicSiteNode;
	Species* sp;

	// Read until we encounter the EndSelect keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		SelectNodeKeyword nk = selectNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (SelectNodeKeyword::DynamicSiteKeyword):
				dynamicSiteNode = new AnalysisDynamicSiteNode(this);
				dynamicSites_.add(dynamicSiteNode);
				if (!dynamicSiteNode->read(parser, coreData, contextStack)) return false;
				break;
			case (SelectNodeKeyword::EndSelectKeyword):
				return true;
			case (SelectNodeKeyword::ExcludeSameMoleculeKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					AnalysisSelectNode* otherNode = (AnalysisSelectNode*) contextStack.nodeInScope(parser.argc(n), AnalysisNode::SelectNode);
					if (!otherNode) return Messenger::error("Unrecognised selection node '%s' given to %s keyword.\n", parser.argc(n), selectNodeKeyword(nk));
					if (!addSameMoleculeExclusion(otherNode)) return Messenger::error("Duplicate site given to %s keyword.\n", selectNodeKeyword(nk));
				}
				break;
			case (SelectNodeKeyword::ExcludeSameSiteKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					AnalysisSelectNode* otherNode = (AnalysisSelectNode*) contextStack.nodeInScope(parser.argc(n), AnalysisNode::SelectNode);
					if (!otherNode) return Messenger::error("Unrecognised selection node '%s' given to %s keyword.\n", parser.argc(n), selectNodeKeyword(nk));
					if (!addSameSiteExclusion(otherNode)) return Messenger::error("Duplicate site given to %s keyword.\n", selectNodeKeyword(nk));
				}
				break;
			case (SelectNodeKeyword::ForEachKeyword):
				// Check that a ForEach branch hasn't already been defined
				if (forEachBranch_) return Messenger::error("Only one ForEach branch may be defined in a selection node.\n");

				// Create and parse a new branch
				forEachBranch_ = new AnalysisSequenceNode("EndForEach");
				forEachBranch_->setParent(parent());
				if (!forEachBranch_->read(parser, coreData, contextStack)) return false;
				break;
			case (SelectNodeKeyword::SameMoleculeAsSiteKeyword):
				if (sameMolecule_) return Messenger::error("Same molecule restriction has already been set, and cannot be set again.\n");
				sameMolecule_ = (AnalysisSelectNode*) contextStack.nodeInScope(parser.argc(1), AnalysisNode::SelectNode);
				if (!sameMolecule_) return Messenger::error("Unrecognised selection node '%s' given to %s keyword.\n", parser.argc(1), selectNodeKeyword(nk));
				break;
			case (SelectNodeKeyword::SiteKeyword):
				// First argument is the target Species, second is a site within it
				sp = coreData.findSpecies(parser.argc(1));

				// Did we find the named Species?
				if (sp)
				{
					// Found the Species, so see if it has a site with the correct name
					SpeciesSite* site = sp->findSite(parser.argc(2));
					if (!site) return Messenger::error("Species '%s' contains no site named '%s'.\n", sp->name(), parser.argc(2));
					if (speciesSites_.contains(site)) return Messenger::error("Site '%s' on Species '%s' specified twice in Select node.\n", site->name(), sp->name());

					Messenger::printVerbose("Select node %p uses site label '%s' ('%s' in Species '%s').\n", this, name(), site->name(), sp->name());

					speciesSites_.add(site);
					break;
				}
				else return Messenger::error("Couldn't find named Species '%s'.\n", parser.argc(1));
				break;
			case (SelectNodeKeyword::nSelectNodeKeywords):
				return Messenger::error("Unrecognised Select node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Select node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisSelectNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
