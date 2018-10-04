/*
	*** Analysis Node - DynamicSelect
	*** src/analyse/nodes/dynamicselect.cpp
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

#include "analyse/nodes/dynamicselect.h"
#include "analyse/nodes/sequence.h"
#include "analyse/nodecontextstack.h"
#include "analyse/sitereference.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AnalysisDynamicSelectNode::AnalysisDynamicSelectNode() : AnalysisSelectBaseNode(), AnalysisNode(AnalysisNode::DynamicSelectNode)
{
}

// Destructor
AnalysisDynamicSelectNode::~AnalysisDynamicSelectNode()
{
	if (forEachBranch_) delete forEachBranch_;
}

/*
 * Node Keywords
 */

// Node Keywords
const char* DynamicSelectNodeKeywords[] = { "Elements", "EndDynamicSelect", "Site" };

// Convert string to node keyword
AnalysisDynamicSelectNode::DynamicSelectNodeKeyword AnalysisDynamicSelectNode::dynamicSelectNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisDynamicSelectNode::nDynamicSelectNodeKeywords; ++nk) if (DissolveSys::sameString(s, DynamicSelectNodeKeywords[nk])) return (AnalysisDynamicSelectNode::DynamicSelectNodeKeyword) nk;

	return AnalysisDynamicSelectNode::nDynamicSelectNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisDynamicSelectNode::dynamicSelectNodeKeyword(AnalysisDynamicSelectNode::DynamicSelectNodeKeyword nk)
{
	return DynamicSelectNodeKeywords[nk];
}

/*
 * Selection Target
 */

// Add elemental selection target
bool AnalysisDynamicSelectNode::addElementTarget(Element* el)
{
	if (targetElements_.contains(el)) return false;
	else targetElements_.add(el);

	return true;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisDynamicSelectNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	nCumulativeSites_ = 0;

	// If one exists, prepare the ForEach branch nodes
	if (forEachBranch_ && (!forEachBranch_->prepare(cfg, prefix, targetList))) return false;

	return true;
}

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisDynamicSelectNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Clear our site and reference arrays
	dynamicSites_.clear();
	sites_.clear();

	// Create our exclusion lists
	RefList<const Molecule,bool> excludedMolecules;
	RefListIterator<AnalysisSelectBaseNode,bool> moleculeExclusionIterator(sameMoleculeExclusions_);
	while (AnalysisSelectBaseNode* node = moleculeExclusionIterator.iterate()) if (node->currentSite()) excludedMolecules.addUnique(node->currentSite()->molecule());

	RefList<const Site,bool> excludedSites;
	RefListIterator<AnalysisSelectBaseNode,bool> siteExclusionIterator(sameSiteExclusions_);
	while (AnalysisSelectBaseNode* node = siteExclusionIterator.iterate()) if (node->currentSite()) excludedSites.addUnique(node->currentSite());

	// Create sites from the specified Configuration
// 	sites_.createEmpty(siteStack->nSites());
// 	for (int n=0; n<siteStack->nSites(); ++n)
// 	{
// 		const Site* site = &siteStack->site(n);
// 
// 		// Check Molecule exclusions
// 		if (excludedMolecules.contains(site->molecule())) continue;
// 
// 		// Check Site exclusions
// 		if (excludedSites.contains(site)) continue;
// 
// 		// All OK, so add site
// 		sites_.add(site);
// 	}
// 	currentSiteIndex_ = (sites_.nItems() == 0 ? -1 : 0);
// 
// 	++nSelections_;
// 
// 	// If a ForEach branch has been defined, process it for each of our sites in turn. Otherwise, we're done.
// 	if (forEachBranch_)
// 	{
// 		for (currentSiteIndex_ = 0; currentSiteIndex_ < sites_.nItems(); ++currentSiteIndex_)
// 		{
// 			++nCumulativeSites_;
// 
// 			// If the branch fails at any point, return failure here.  Otherwise, continue the loop
// 			if (forEachBranch_->execute(procPool, cfg, prefix, targetList) == AnalysisNode::Failure) return AnalysisNode::Failure;
// 		}
// 	}

	return AnalysisNode::Success;
}

// Finalise any necessary data after execution
bool AnalysisDynamicSelectNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// If one exists, finalise the ForEach branch nodes
	if (forEachBranch_ && (!forEachBranch_->finalise(procPool, cfg, prefix, targetList))) return false;

	// Print out summary information
	Messenger::print("DynamicSelect - Site '%s': Number of selections made = %i (last contained %i sites).\n", name(), nSelections_ , sites_.nItems());
	Messenger::print("DynamicSelect - Site '%s': Average number of sites selected per selection = %.2f.\n", name(), double(nCumulativeSites_)/nSelections_);
	Messenger::print("DynamicSelect - Site '%s': Cumulative number of sites selected = %i.\n", name(), nCumulativeSites_);

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisDynamicSelectNode::read(LineParser& parser, NodeContextStack& contextStack)
{
	// The current line in the parser may contain a specific label for the sites we are to select, which we set as our node name
	setName(parser.nArgs() == 2 ? parser.argc(1) : contextStack.nextSelectName());

	// Add ourselves to the context stack
	if (!contextStack.add(this, name())) return Messenger::error("Error adding DynamicSelect node '%s' to context stack.\n", name());

	// Read until we encounter the EndDynamicSelect keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Check if the current line contains a base keyword
		int baseResult = parseBaseKeyword(parser, contextStack);
		if (baseResult == 0) return false;
		else if (baseResult == 1) continue;

		// Is the first argument on the current line a valid control keyword?
		DynamicSelectNodeKeyword nk = dynamicSelectNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (DynamicSelectNodeKeyword::ElementsKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					Element* el = Elements::elementPointer(parser.argc(n));
					if (!el) return Messenger::error("Unrecognised element '%s' given to %s keyword.\n", parser.argc(n), dynamicSelectNodeKeyword(nk));
					if (!addElementTarget(el)) return Messenger::error("Duplicate site given to %s keyword.\n", dynamicSelectNodeKeyword(nk));
				}
				break;
			case (DynamicSelectNodeKeyword::EndDynamicSelectKeyword):
				return true;
			case (DynamicSelectNodeKeyword::SiteKeyword):
// 				// If we already have a species/site reference, bail out now
// 				if (species_) return Messenger::error("The '%s' keyword must appear exactly once in a DynamicSelect node.\n", selectNodeKeyword(DynamicSelectNodeKeyword::SiteKeyword));
// 
// 				// First argument is the target Species, second is a site within it
// 				// Find the named Species
// 				for (species_ = List<Species>::masterInstance().first(); species_ != NULL; species_ = species_->next)
// 				{
// 					if (!DissolveSys::sameString(species_->name(), parser.argc(1))) continue;
// 
// 					// Found the Species, so see if it has a site with the correct name
// 					speciesSite_ = species_->findSite(parser.argc(2));
// 					if (!speciesSite_) return Messenger::error("Species '%s' contains no site named '%s'.\n", species_->name(), parser.argc(2));
// 
// 					Messenger::printVerbose("DynamicSelect node %p uses site label '%s' ('%s' in Species '%s').\n", this, name(), speciesSite_->name(), species_->name());
// 					break;
// 				}
// 				// If we reach here and don't have a valid species_ pointer, we couldn't find the named Species
// 				if (!species_) return Messenger::error("Couldn't find named Species '%s'.\n", parser.argc(1));
				break;
			case (DynamicSelectNodeKeyword::nDynamicSelectNodeKeywords):
				return Messenger::error("Unrecognised DynamicSelect node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the DynamicSelect node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisDynamicSelectNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
