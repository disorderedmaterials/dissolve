/*
	*** Analysis Node - Dynamic Site
	*** src/analyse/nodes/dynamicsite.cpp
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

#include "analyse/nodes/dynamicsite.h"
#include "analyse/nodes/select.h"
#include "analyse/nodecontextstack.h"
#include "classes/atom.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/site.h"
#include "data/elements.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/dynamicarray.h"

// Constructor
AnalysisDynamicSiteNode::AnalysisDynamicSiteNode(AnalysisSelectNode* parent) : AnalysisNode(AnalysisNode::DynamicSiteNode)
{
	parent_ = parent;
}

// Destructor
AnalysisDynamicSiteNode::~AnalysisDynamicSiteNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords
const char* DynamicSiteNodeKeywords[] = { "Element", "EndDynamicSite" };

// Convert string to node keyword
AnalysisDynamicSiteNode::DynamicSiteNodeKeyword AnalysisDynamicSiteNode::dynamicSiteNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisDynamicSiteNode::nDynamicSiteNodeKeywords; ++nk) if (DissolveSys::sameString(s, DynamicSiteNodeKeywords[nk])) return (AnalysisDynamicSiteNode::DynamicSiteNodeKeyword) nk;

	return AnalysisDynamicSiteNode::nDynamicSiteNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisDynamicSiteNode::dynamicSiteNodeKeyword(AnalysisDynamicSiteNode::DynamicSiteNodeKeyword nk)
{
	return DynamicSiteNodeKeywords[nk];
}

/*
 * Site Criteria
 */

/*
 * Site Generation
 */

// Generate sites from the specified Molecule
void AnalysisDynamicSiteNode::generateSites(const Molecule* molecule)
{
	// Select by element
	if (elements_.nItems() > 0)
	{
		// Loop over Atoms in the Molecule
		for (int n=0; n<molecule->nAtoms(); ++n)
		{
			// If the element is listed in our target elements list, add this atom as a site
			if (elements_.contains(molecule->atom(n)->element())) generatedSites_.add(Site(molecule, molecule->atom(n)->r()));
		}
	}
}

// Return Array of generated sites
const Array<Site>& AnalysisDynamicSiteNode::generatedSites() const
{
	return generatedSites_;
}

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisDynamicSiteNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Clear our current list of sites
	generatedSites_.clear();

	// Grab exclusion lists and any specific Molecule parent
	const RefList<const Molecule, bool>& excludedMolecules = parent_->excludedMolecules();
	const Molecule* moleculeParent = parent_->sameMoleculeMolecule();

	/*
	 * We'll loop over all Molecules in the Configuration, rather than all Atoms, since there are useful exclusions we can make based on the parent Molecule.
	 * If, however, a sameMolecule_ is defined then we can simply grab this Molecule and do the checks within it, rather than looping.
	 * Both use the local function generateSites(Molecule*) in order to extract site information.
	 */

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


	return AnalysisNode::Success;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisDynamicSiteNode::read(LineParser& parser, NodeContextStack& contextStack)
{
	// Read until we encounter the EndExclude keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		DynamicSiteNodeKeyword nk = dynamicSiteNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (DynamicSiteNodeKeyword::ElementKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					Element* el = Elements::elementPointer(parser.argc(n));
					if (!el) return Messenger::error("Unrecognised element '%s' given to %s keyword.\n", parser.argc(n), dynamicSiteNodeKeyword(nk));
					if (elements_.contains(el)) return Messenger::error("Duplicate site given to %s keyword.\n", dynamicSiteNodeKeyword(nk));
					elements_.add(el);
				}
				break;
			case (DynamicSiteNodeKeyword::EndDynamicSiteKeyword):
				return true;
			case (DynamicSiteNodeKeyword::nDynamicSiteNodeKeywords):
				return Messenger::error("Unrecognised DynamicSite node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the DynamicSite node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisDynamicSiteNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
