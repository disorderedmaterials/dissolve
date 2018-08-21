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
#include "analyse/sitecontextstack.h"
#include "analyse/sitereference.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AnalysisSelectNode::AnalysisSelectNode() : AnalysisNode()
{
	forEachBranch_ = NULL;
	species_ = NULL;
	speciesSite_ = NULL;

	type_ = AnalysisNode::SelectNode;
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
const char* SelectNodeKeywords[] = { "EndSelect", "ForEach", "Site" };

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
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisSelectNode::read(LineParser& parser, SiteContextStack& contextStack)
{
	// The current line in the parser may contain a specific label for the sites we are to select...
	CharString siteLabel = (parser.nArgs() == 2 ? parser.argc(1) : contextStack.nextGenericName());

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

					// Create a new SiteReference
					if (!contextStack.addToCurrent(this, siteLabel)) return Messenger::error("Error creating site reference.\n");

					Messenger::printVerbose("Select node %p uses site label '%s' ('%s' in Species '%s').\n", this, siteLabel.get(), speciesSite_->name(), species_->name());
					break;
				}
				// If we reach here and don't have a siteRef pointer, we couldn't find the named Species
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
