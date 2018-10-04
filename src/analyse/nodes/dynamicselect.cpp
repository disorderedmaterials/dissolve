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

// Generate sites from the specified Molecule
void AnalysisDynamicSelectNode::generateSites(const Molecule* molecule)
{
	// If element targets are defined, 
	if (targetElements_.nItems() != 0)
	{
		// Loop over Atoms in the Molecule
		for (int n=0; n<molecule->nAtoms(); ++n)
		{
			// If the element is listed in our target elements list, add this atom as a site
			if (targetElements_.contains(molecule->atom(n)->element())) dynamicSites_.add(Site(molecule, molecule->atom(n)->r()));
		}
	}
}

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
	// Clear our site array
	dynamicSites_.clear();

	// Create our excluded Molecule lists - excluded site lists are not relevant
	RefList<const Molecule,bool> excludedMolecules;
	RefListIterator<AnalysisSelectBaseNode,bool> moleculeExclusionIterator(sameMoleculeExclusions_);
	while (AnalysisSelectBaseNode* node = moleculeExclusionIterator.iterate()) if (node->currentSite()) excludedMolecules.addUnique(node->currentSite()->molecule());

	/*
	 * We'll loop over all Molecules in the system, rather than all Atoms, since there are useful exclusions we can make based on the parent Molecule.
	 * If, however, a sameMolecule_ is defined then we can simply grab this Molecule and do the checks within it, rather than looping.
	 * Both use the local function generateSites(Molecule*) in order to extract site information.
	 */

	// Get required Molecule parent, if requested
	const Molecule* moleculeParent = sameMolecule_ ? sameMoleculeMolecule() : NULL;

	if (moleculeParent) generateSites(moleculeParent);
	else
	{
		// Loop over Molecules in the target Configuration
		DynamicArray<Molecule>& molecules = cfg->molecules();
		for (int n=0; n<molecules.nItems(); ++n)
		{
			// Check Molecule exclusions
			if (excludedMolecules.contains(molecules[n])) continue;

			// All OK, so generate sites
			generateSites(moleculeParent);
		}
	}

	// Construct the Site pointer array
	sites_.clear();
	for (int n=0; n<dynamicSites_.nItems(); ++n) sites_.add(&dynamicSites_[n]);

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
				// Check here that no site exclusions have been defined, as they cannot be enforced
				if (sameSiteExclusions_.nItems() != 0) return Messenger::error("Site exclusions cannot be enforced for DynamicSelect.\n");
				return true;
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
