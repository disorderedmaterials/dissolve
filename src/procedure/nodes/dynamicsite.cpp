/*
	*** Procedure Node - Dynamic Site
	*** src/procedure/nodes/dynamicsite.cpp
	Copyright T. Youngs 2012-2019

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

#include "procedure/nodes/dynamicsite.h"
#include "procedure/nodes/select.h"
#include "procedure/nodescopestack.h"
#include "classes/atom.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/molecule.h"
#include "classes/site.h"
#include "data/elements.h"
#include "classes/atomtype.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/dynamicarray.h"

// Constructor
DynamicSiteProcedureNode::DynamicSiteProcedureNode(SelectProcedureNode* parent) : ProcedureNode(ProcedureNode::DynamicSiteNode)
{
	parent_ = parent;
}

// Destructor
DynamicSiteProcedureNode::~DynamicSiteProcedureNode()
{
}

/*
 * Identity
 */

// Return whether specified usage type is allowed for this node
bool DynamicSiteProcedureNode::isUsageTypeAllowed(ProcedureNode::NodeUsageType usageType)
{
	return (usageType == ProcedureNode::AnalysisUsageType);
}

/*
 * Node Keywords
 */

// Return enum option info for DynamicSiteNodeKeyword
EnumOptions<DynamicSiteProcedureNode::DynamicSiteNodeKeyword> DynamicSiteProcedureNode::dynamicSiteNodeKeywords()
{
	static EnumOptionsList DynamicSiteNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(DynamicSiteProcedureNode::AtomTypeKeyword,		"AtomType") <<
		EnumOption(DynamicSiteProcedureNode::ElementKeyword,		"Element") <<
		EnumOption(DynamicSiteProcedureNode::EndDynamicSiteKeyword,	"EndDynamicSite");

	static EnumOptions<DynamicSiteProcedureNode::DynamicSiteNodeKeyword> options("DynamicSiteNodeKeyword", DynamicSiteNodeTypeKeywords);

	return options;
}

/*
 * Site Generation
 */

// Generate sites from the specified Molecule
void DynamicSiteProcedureNode::generateSites(const Molecule* molecule)
{
	// Loop over Atoms in the Molecule
	for (int n=0; n<molecule->nAtoms(); ++n)
	{
		// If the element is listed in our target elements list, add this atom as a site
		if (elements_.contains(molecule->atom(n)->element()))
		{
			generatedSites_.add(Site(molecule, molecule->atom(n)->r()));
			continue;
		}

		// If the Atom's AtomType is listed in our target AtomTYpe list, add this atom as a site
		if (atomTypes_.containsData(molecule->atom(n)->masterTypeIndex()))
		{
			generatedSites_.add(Site(molecule, molecule->atom(n)->r()));
			continue;
		}
	}
}

// Return Array of generated sites
const Array<Site>& DynamicSiteProcedureNode::generatedSites() const
{
	return generatedSites_;
}

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult DynamicSiteProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
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
			generateSites(molecules[n]);
		}
	}

	return ProcedureNode::Success;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool DynamicSiteProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// Read until we encounter the EndExclude keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		DynamicSiteNodeKeyword nk = dynamicSiteNodeKeywords().enumeration(parser.argc(0));
		switch (nk)
		{
			case (DynamicSiteProcedureNode::AtomTypeKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					AtomType* at = coreData.findAtomType(parser.argc(n));
					if (!at) return Messenger::error("Unrecognised AtomType '%s' given to %s keyword.\n", parser.argc(n), dynamicSiteNodeKeywords().keyword(nk));
					if (atomTypes_.contains(at)) return Messenger::error("Duplicate AtomType target given to %s keyword.\n", dynamicSiteNodeKeywords().keyword(nk));
					atomTypes_.add(at, coreData.constAtomTypes().indexOf(at));
				}
				break;
			case (DynamicSiteProcedureNode::ElementKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					Element* el = Elements::elementPointer(parser.argc(n));
					if (!el) return Messenger::error("Unrecognised element '%s' given to %s keyword.\n", parser.argc(n), dynamicSiteNodeKeywords().keyword(nk));
					if (elements_.contains(el)) return Messenger::error("Duplicate Element target given to %s keyword.\n", dynamicSiteNodeKeywords().keyword(nk));
					elements_.add(el);
				}
				break;
			case (DynamicSiteProcedureNode::EndDynamicSiteKeyword):
				return true;
			case (DynamicSiteProcedureNode::nDynamicSiteNodeKeywords):
				return Messenger::error("Unrecognised DynamicSite node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the DynamicSite node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool DynamicSiteProcedureNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
