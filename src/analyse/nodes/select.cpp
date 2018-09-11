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
#include "analyse/nodes/sequence.h"
#include "analyse/nodecontextstack.h"
#include "analyse/sitereference.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AnalysisSelectNode::AnalysisSelectNode(SpeciesSite* site) : AnalysisNode(AnalysisNode::SelectNode)
{
	speciesSite_ = site;
	species_ = (speciesSite_ ? speciesSite_->parent() : NULL);
	forEachBranch_ = NULL;
	currentSiteIndex_ = -1;
	nCumulativeSites_ = 0;
	nSelections_ = 0;
}

// Destructor
AnalysisSelectNode::~AnalysisSelectNode()
{
	if (forEachBranch_) delete forEachBranch_;
}

/*
 * Node Keywords
 */

// Node Keywords
const char* SelectNodeKeywords[] = { "EndSelect", "ExcludeSameMolecule", "ExcludeSameSite", "ForEach", "Site" };

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
 * Data
 */

// Add "same molecule" exclusion
bool AnalysisSelectNode::addSameMoleculeExclusion(AnalysisSelectNode* node)
{
	if (sameMoleculeExclusions_.contains(node)) return false;
	else sameMoleculeExclusions_.add(node);

	return true;
}

// Add "same site" exclusion
bool AnalysisSelectNode::addSameSiteExclusion(AnalysisSelectNode* node)
{
	if (sameSiteExclusions_.contains(node)) return false;
	else sameSiteExclusions_.add(node);

	return true;
}

// Add and return ForEach sequence
AnalysisSequenceNode* AnalysisSelectNode::addForEachBranch()
{
	if (!forEachBranch_) forEachBranch_ = new AnalysisSequenceNode();

	return forEachBranch_;
}

// Add specified node to ForEach sequence
void AnalysisSelectNode::addToForEachBranch(AnalysisNode* node)
{
	addForEachBranch()->addNode(node);
}

/*
 * Site Information
 */

// Return whether the node has available site information
bool AnalysisSelectNode::hasSites() const
{
	return true;
}

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
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisSelectNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	nSelections_ = 0;
	nCumulativeSites_ = 0;

	// If one exists, prepare the ForEach branch nodes
	if (forEachBranch_ && (!forEachBranch_->prepare(cfg, prefix, targetList))) return false;

	return true;
}

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisSelectNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Create our array of sites from the source Configuration
	sites_.clear();
	
	const SiteStack* siteStack = cfg->siteStack(speciesSite_);
	if (siteStack == NULL) return AnalysisNode::Failure;

	// Create our exclusion lists
	RefList<const Molecule,bool> excludedMolecules;
	RefListIterator<AnalysisSelectNode,bool> moleculeExclusionIterator(sameMoleculeExclusions_);
	while (AnalysisSelectNode* node = moleculeExclusionIterator.iterate()) if (node->currentSite()) excludedMolecules.addUnique(node->currentSite()->molecule());

	RefList<const Site,bool> excludedSites;
	RefListIterator<AnalysisSelectNode,bool> siteExclusionIterator(sameSiteExclusions_);
	while (AnalysisSelectNode* node = siteExclusionIterator.iterate()) if (node->currentSite()) excludedSites.addUnique(node->currentSite());

	sites_.createEmpty(siteStack->nSites());
	for (int n=0; n<siteStack->nSites(); ++n)
	{
		const Site* site = &siteStack->site(n);

		// Check Molecule exclusions
		if (excludedMolecules.contains(site->molecule())) continue;

		// Check Site exclusions
		if (excludedSites.contains(site)) continue;

		// All OK, so add site
		sites_.add(site);
	}
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

	// Print out summary information
	Messenger::print("Select - Site '%s': Number of selections made = %i (last contained %i sites).\n", name(), nSelections_ , sites_.nItems());
	Messenger::print("Select - Site '%s': Average number of sites selected per selection = %.2f.\n", name(), double(nCumulativeSites_)/nSelections_);
	Messenger::print("Select - Site '%s': Cumulative number of sites selected = %i.\n", name(), nCumulativeSites_);

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisSelectNode::read(LineParser& parser, NodeContextStack& contextStack)
{
	// The current line in the parser may contain a specific label for the sites we are to select, which we set as our node name
	setName(parser.nArgs() == 2 ? parser.argc(1) : contextStack.nextSelectName());

	// Add ourselves to the context stack
	if (!contextStack.add(this)) return Messenger::error("Error adding Select node '%s' to context stack.\n", name());

	// Read until we encounter the EndSelect keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		SelectNodeKeyword nk = selectNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (SelectNodeKeyword::EndSelectKeyword):
				return true;
			case (SelectNodeKeyword::ExcludeSameMoleculeKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					AnalysisSelectNode* otherNode = contextStack.selectNodeInScope(parser.argc(n));
					if (!otherNode) return Messenger::error("Unrecognised Select node '%s' given to %s keyword.\n", parser.argc(n), selectNodeKeyword(SelectNodeKeyword::ExcludeSameMoleculeKeyword));
					if (!addSameMoleculeExclusion(otherNode)) return Messenger::error("Duplicate site given to %s keyword.\n", selectNodeKeyword(SelectNodeKeyword::ExcludeSameMoleculeKeyword));
				}
				break;
			case (SelectNodeKeyword::ExcludeSameSiteKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					AnalysisSelectNode* otherNode = contextStack.selectNodeInScope(parser.argc(n));
					if (!otherNode) return Messenger::error("Unrecognised Select node '%s' given to %s keyword.\n", parser.argc(n), selectNodeKeyword(SelectNodeKeyword::ExcludeSameSiteKeyword));
					if (!addSameSiteExclusion(otherNode)) return Messenger::error("Duplicate site given to %s keyword.\n", selectNodeKeyword(SelectNodeKeyword::ExcludeSameSiteKeyword));
				}
				break;
			case (SelectNodeKeyword::ForEachKeyword):
				// Check that a ForEach branch hasn't already been defined
				if (forEachBranch_) return Messenger::error("Only one ForEach branch may be defined.\n");

				// Create and parse a new branch
				forEachBranch_ = new AnalysisSequenceNode("EndForEach");
				if (!forEachBranch_->read(parser, contextStack)) return false;
				break;
			case (SelectNodeKeyword::SiteKeyword):
				// If we already have a species/site reference, bail out now
				if (species_) return Messenger::error("The '%s' keyword must appear exactly once in a Select node.\n", selectNodeKeyword(SelectNodeKeyword::SiteKeyword));

				// First argument is the target Species, second is a site within it
				// Find the named Species
				for (species_ = List<Species>::masterInstance().first(); species_ != NULL; species_ = species_->next)
				{
					if (!DissolveSys::sameString(species_->name(), parser.argc(1))) continue;

					// Found the Species, so see if it has a site with the correct name
					speciesSite_ = species_->findSite(parser.argc(2));
					if (!speciesSite_) return Messenger::error("Species '%s' contains no site named '%s'.\n", species_->name(), parser.argc(2));

					Messenger::printVerbose("Select node %p uses site label '%s' ('%s' in Species '%s').\n", this, name(), speciesSite_->name(), species_->name());
					break;
				}
				// If we reach here and don't have a valid species_ pointer, we couldn't find the named Species
				if (!species_) return Messenger::error("Couldn't find named Species '%s'.\n", parser.argc(1));
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
