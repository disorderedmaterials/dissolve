/*
	*** Procedure Node - Select
	*** src/procedure/nodes/select.cpp
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

#include "procedure/nodes/select.h"
#include "procedure/nodes/dynamicsite.h"
#include "procedure/nodes/sequence.h"
#include "procedure/nodescopestack.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/sitereference.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructors
SelectProcedureNode::SelectProcedureNode(SpeciesSite* site) : ProcedureNode(ProcedureNode::SelectNode)
{
	if (site) speciesSites_.add(site);

	sameMolecule_= NULL;
	forEachBranch_ = NULL;
	currentSiteIndex_ = -1;
	nCumulativeSites_ = 0;
	nSelections_ = 0;
}

SelectProcedureNode::SelectProcedureNode(const RefList<SpeciesSite,bool>& sites) : ProcedureNode(ProcedureNode::SelectNode)
{
	speciesSites_ = sites;

	sameMolecule_= NULL;
	forEachBranch_ = NULL;
	currentSiteIndex_ = -1;
	nCumulativeSites_ = 0;
	nSelections_ = 0;
}

// Destructor
SelectProcedureNode::~SelectProcedureNode()
{
	// Remove the forEach branch
	if (forEachBranch_) delete forEachBranch_;

	// Delete any dynamic site nodes (as we are the owner)
	RefListIterator<DynamicSiteProcedureNode,bool> dynamicIterator(dynamicSites_);
	while (DynamicSiteProcedureNode* dynamicNode = dynamicIterator.iterate()) delete dynamicNode;
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
 * Node Keywords
 */

// Return enum option info for SelectNodeKeyword
EnumOptions<SelectProcedureNode::SelectNodeKeyword> SelectProcedureNode::selectNodeKeywords()
{
	static EnumOptionsList SelectNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(SelectProcedureNode::DynamicSiteKeyword,		"DynamicSite") <<
		EnumOption(SelectProcedureNode::EndSelectKeyword,		"EndSelect") <<
		EnumOption(SelectProcedureNode::ExcludeSameMoleculeKeyword,	"ExcludeSameMolecule",	EnumOption::OneOrMoreArguments) <<
		EnumOption(SelectProcedureNode::ExcludeSameSiteKeyword,		"ExcludeSameSite",	EnumOption::OneOrMoreArguments) <<
		EnumOption(SelectProcedureNode::ForEachKeyword,			"ForEach") <<
		EnumOption(SelectProcedureNode::SameMoleculeAsSiteKeyword,	"SameMoleculeAsSite",	1) <<
		EnumOption(SelectProcedureNode::SiteKeyword,			"Site",			2);

	static EnumOptions<SelectProcedureNode::SelectNodeKeyword> options("SelectNodeKeyword", SelectNodeTypeKeywords);

	return options;
}

/*
 * Selection Targets
 */

/*
 * Selection Control
 */

// Add "same molecule" exclusion
bool SelectProcedureNode::addSameMoleculeExclusion(SelectProcedureNode* node)
{
	if (sameMoleculeExclusions_.contains(node)) return false;
	else sameMoleculeExclusions_.add(node);

	return true;
}

// Return list of Molecules currently excluded from selection
const RefList<const Molecule,bool>& SelectProcedureNode::excludedMolecules() const
{
	return excludedMolecules_;
}

// Add "same site" exclusion
bool SelectProcedureNode::addSameSiteExclusion(SelectProcedureNode* node)
{
	if (sameSiteExclusions_.contains(node)) return false;
	else sameSiteExclusions_.add(node);

	return true;
}

// List of Sites currently excluded from selection
const RefList<const Site,bool>& SelectProcedureNode::excludedSites() const
{
	return excludedSites_;
}

// Set node containing molecule from which our site must also be contained within
bool SelectProcedureNode::setSameMolecule(SelectProcedureNode* node)
{
	if (sameMolecule_) return Messenger::error("Same molecule restriction has already been set, and cannot be set again.\n");
	sameMolecule_ = node;

	return true;
}

// Return Molecule (from site) in which the site must exist
const Molecule* SelectProcedureNode::sameMoleculeMolecule()
{
	if (!sameMolecule_) return NULL;

	const Site* site = sameMolecule_->currentSite();
	if (!site)
	{
		Messenger::warn("Requested Molecule from SelectProcedureNode::sameMolecule_, but there is no current site.\n");
		return NULL;
	}

	return site->molecule();
}

/*
 * Selected Sites
 */

// Return the number of available sites in the current stack, if any
int SelectProcedureNode::nSitesInStack() const
{
	return sites_.nItems();
}

// Return the average number of sites selected
double SelectProcedureNode::nAverageSites() const
{
	return double(nCumulativeSites_) / nSelections_;
}

// Return the cumulative number of sites ever selected
int SelectProcedureNode::nCumulativeSites() const
{
	return nCumulativeSites_;
}

// Return current site
const Site* SelectProcedureNode::currentSite() const
{
	return (currentSiteIndex_ == -1 ? NULL : sites_.constAt(currentSiteIndex_));
}

/*
 * Branch
 */

// Add and return ForEach sequence
SequenceProcedureNode* SelectProcedureNode::addForEachBranch(ProcedureNode::NodeContext context_)
{
	if (!forEachBranch_) forEachBranch_ = new SequenceProcedureNode(context_);

	return forEachBranch_;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool SelectProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	nSelections_ = 0;
	nCumulativeSites_ = 0;

	// If one exists, prepare the ForEach branch nodes
	if (forEachBranch_ && (!forEachBranch_->prepare(cfg, prefix, targetList))) return false;

	// Prepare any dynamic site nodes
	RefListIterator<DynamicSiteProcedureNode,bool> dynamicIterator(dynamicSites_);
	while (DynamicSiteProcedureNode* dynamicNode = dynamicIterator.iterate()) if (!dynamicNode->prepare(cfg, prefix, targetList)) return false;

	return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult SelectProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Create our arrays of sites
	sites_.clear();

	// Update our exclusion lists
	excludedMolecules_.clear();
	RefListIterator<SelectProcedureNode,bool> moleculeExclusionIterator(sameMoleculeExclusions_);
	while (SelectProcedureNode* node = moleculeExclusionIterator.iterate()) if (node->currentSite()) excludedMolecules_.addUnique(node->currentSite()->molecule());

	excludedSites_.clear();
	RefListIterator<SelectProcedureNode,bool> siteExclusionIterator(sameSiteExclusions_);
	while (SelectProcedureNode* node = siteExclusionIterator.iterate()) if (node->currentSite()) excludedSites_.addUnique(node->currentSite());

	// Get required Molecule parent, if requested
	const Molecule* moleculeParent = sameMolecule_ ? sameMoleculeMolecule() : NULL;

	/*
	 * Add sites from specified Species/Sites
	 */
	RefListIterator<SpeciesSite, bool> siteIterator(speciesSites_);
	while (SpeciesSite* site = siteIterator.iterate())
	{
		const SiteStack* siteStack = cfg->siteStack(site);
		if (siteStack == NULL) return ProcedureNode::Failure;

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
	 * Call each DynamicSiteProcedureNode's execute function in turn, adding any generated sites to our reference array afterwards.
	 */
	RefListIterator<DynamicSiteProcedureNode,bool> dynamicIterator(dynamicSites_);
	while (DynamicSiteProcedureNode* dynamicNode = dynamicIterator.iterate())
	{
		if (dynamicNode->execute(procPool, cfg, prefix, targetList) == ProcedureNode::Failure) return ProcedureNode::Failure;

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
			if (forEachBranch_->execute(procPool, cfg, prefix, targetList) == ProcedureNode::Failure) return ProcedureNode::Failure;
		}
	}

	return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool SelectProcedureNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// If one exists, finalise the ForEach branch nodes
	if (forEachBranch_ && (!forEachBranch_->finalise(procPool, cfg, prefix, targetList))) return false;

	// Finalise any dynamic site nodes
	RefListIterator<DynamicSiteProcedureNode,bool> dynamicIterator(dynamicSites_);
	while (DynamicSiteProcedureNode* dynamicNode = dynamicIterator.iterate()) if (!dynamicNode->finalise(procPool, cfg, prefix, targetList)) return false;

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
bool SelectProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// The current line in the parser may contain a specific label for the sites we are to select, which we set as our node name
	setName(parser.nArgs() == 2 ? parser.argc(1) : scopeStack.nextSelectName());

	// Add ourselves to the scope stack
	if (!scopeStack.add(this)) return Messenger::error("Error adding Select node '%s' to scope stack.\n", name());

	DynamicSiteProcedureNode* dynamicSiteNode;
	SelectProcedureNode* selectNode;
	Species* sp;

	// Read until we encounter the EndSelect keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim() != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!selectNodeKeywords().isValid(parser.argc(0))) return selectNodeKeywords().errorAndPrintValid(parser.argc(0));
		SelectNodeKeyword nk = selectNodeKeywords().enumeration(parser.argc(0));
		if (!selectNodeKeywords().validNArgs(nk, parser.nArgs()-1)) return false;

		// All OK, so process it
		switch (nk)
		{
			case (SelectProcedureNode::DynamicSiteKeyword):
				dynamicSiteNode = new DynamicSiteProcedureNode(this);
				dynamicSites_.add(dynamicSiteNode);
				if (!dynamicSiteNode->read(parser, coreData, scopeStack)) return false;
				break;
			case (SelectProcedureNode::EndSelectKeyword):
				return true;
			case (SelectProcedureNode::ExcludeSameMoleculeKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					SelectProcedureNode* otherNode = dynamic_cast<SelectProcedureNode*>(scopeStack.nodeInScope(parser.argc(n), ProcedureNode::SelectNode));
					if (!otherNode) return Messenger::error("Unrecognised selection node '%s' given to %s keyword.\n", parser.argc(n), selectNodeKeywords().keyword(nk));
					if (!addSameMoleculeExclusion(otherNode)) return Messenger::error("Duplicate site given to %s keyword.\n", selectNodeKeywords().keyword(nk));
				}
				break;
			case (SelectProcedureNode::ExcludeSameSiteKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					SelectProcedureNode* otherNode = dynamic_cast<SelectProcedureNode*>(scopeStack.nodeInScope(parser.argc(n), ProcedureNode::SelectNode));
					if (!otherNode) return Messenger::error("Unrecognised selection node '%s' given to %s keyword.\n", parser.argc(n), selectNodeKeywords().keyword(nk));
					if (!addSameSiteExclusion(otherNode)) return Messenger::error("Duplicate site given to %s keyword.\n", selectNodeKeywords().keyword(nk));
				}
				break;
			case (SelectProcedureNode::ForEachKeyword):
				// Check that a ForEach branch hasn't already been defined
				if (forEachBranch_) return Messenger::error("Only one ForEach branch may be defined in a selection node.\n");

				// Create and parse a new branch
				forEachBranch_ = new SequenceProcedureNode(scopeStack.currentContext(), "EndForEach");
				if (!forEachBranch_->read(parser, coreData, scopeStack)) return false;
				break;
			case (SelectProcedureNode::SameMoleculeAsSiteKeyword):
				selectNode = dynamic_cast<SelectProcedureNode*>(scopeStack.nodeInScope(parser.argc(1), ProcedureNode::SelectNode));
				if (!selectNode) return Messenger::error("Unrecognised selection node '%s' given to %s keyword.\n", parser.argc(1), selectNodeKeywords().keyword(nk));
				if (!setSameMolecule(selectNode)) return Messenger::error("Failed to set same molecule specification.\n");
				break;
			case (SelectProcedureNode::SiteKeyword):
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
			case (SelectProcedureNode::nSelectNodeKeywords):
				return Messenger::error("Unrecognised Select node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Select node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool SelectProcedureNode::write(LineParser& parser, const char* prefix)
{
	// Block Start
	if (!parser.writeLineF("%s%s\n", ProcedureNode::nodeTypes().keyword(type_))) return false;

	// Normal Sites
	RefListIterator<SpeciesSite,bool> siteIterator(speciesSites_);
	while (SpeciesSite* site = siteIterator.iterate())
	{
		if (!parser.writeLineF("%s  %s  '%s'  '%s'\n", prefix, selectNodeKeywords().keyword(SelectProcedureNode::SiteKeyword), site->name(), site->parent()->name())) return false;
	}

	// Dynamic Sites
	RefListIterator<DynamicSiteProcedureNode,bool> dynamicSiteIterator(dynamicSites_);
	while (DynamicSiteProcedureNode* site = dynamicSiteIterator.iterate()) if (!site->write(parser, CharString("%s  ", prefix))) return false;

	// Same Molecule Exclusions
	if (sameMoleculeExclusions_.nItems() > 0)
	{
		CharString sameMolExclusions;
		RefListIterator<SelectProcedureNode,bool> exclusionIterator(sameMoleculeExclusions_);
		while (SelectProcedureNode* node = exclusionIterator.iterate()) sameMolExclusions.strcatf("  %s", node->name());
		if (!parser.writeLineF("%s  %s%s\n", prefix, selectNodeKeywords().keyword(SelectProcedureNode::ExcludeSameMoleculeKeyword), sameMolExclusions.get())) return false;
	}

	// Same Site Exclusions
	if (sameSiteExclusions_.nItems() > 0)
	{
		CharString sameSiteExclusions;
		RefListIterator<SelectProcedureNode,bool> exclusionIterator(sameSiteExclusions_);
		while (SelectProcedureNode* node = exclusionIterator.iterate()) sameSiteExclusions.strcatf("  %s", node->name());
		if (!parser.writeLineF("%s  %s%s\n", prefix, selectNodeKeywords().keyword(SelectProcedureNode::ExcludeSameSiteKeyword), sameSiteExclusions.get())) return false;
	}

	// Same Molecule As Site?
	if (sameMolecule_ && (!parser.writeLineF("%s  %s  %s\n", prefix, selectNodeKeywords().keyword(SelectProcedureNode::SameMoleculeAsSiteKeyword), sameMolecule_->name()))) return false;

	// ForEach Branch
	if (forEachBranch_ && (!forEachBranch_->write(parser, CharString("%s  ", prefix)))) return false;

	// Block End
	if (!parser.writeLineF("%s%s\n", selectNodeKeywords().keyword(SelectProcedureNode::EndSelectKeyword))) return false;

	return true;
}
