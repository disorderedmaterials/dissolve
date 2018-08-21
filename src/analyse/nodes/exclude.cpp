/*
	*** Analysis Node - Exclude
	*** src/analyse/nodes/exclude.cpp
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

#include "analyse/nodes/exclude.h"
#include "analyse/sitecontextstack.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AnalysisExcludeNode::AnalysisExcludeNode() : AnalysisNode()
{
	type_ = AnalysisNode::ExcludeNode;
}

// Destructor
AnalysisExcludeNode::~AnalysisExcludeNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords
const char* ExcludeNodeKeywords[] = { "EndExclude", "SameSpecies" };

// Convert string to node keyword
AnalysisExcludeNode::ExcludeNodeKeyword AnalysisExcludeNode::excludeNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisExcludeNode::nExcludeNodeKeywords; ++nk) if (DissolveSys::sameString(s, ExcludeNodeKeywords[nk])) return (AnalysisExcludeNode::ExcludeNodeKeyword) nk;

	return AnalysisExcludeNode::nExcludeNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisExcludeNode::excludeNodeKeyword(AnalysisExcludeNode::ExcludeNodeKeyword nk)
{
	return ExcludeNodeKeywords[nk];
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisExcludeNode::read(LineParser& parser, SiteContextStack& contextStack)
{
	int index;

	// Read until we encounter the EndExclude keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		ExcludeNodeKeyword nk = excludeNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (ExcludeNodeKeyword::EndExcludeKeyword):
				return true;
			case (ExcludeNodeKeyword::SameSpeciesKeyword):
				index = sameSpecies_.last() ? sameSpecies_.last()->data+1 : 0;
				for (int n=1; n<parser.nArgs(); ++n)
				{
					// If we find a '|' increase the index counter and continue
					if (DissolveSys::sameString("|", parser.argc(n)))
					{
						++index;
						continue;
					}

					// Each argument should be a named site on the stack, so find it...
					if (!contextStack.hasSite(parser.argc(n))) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(n), excludeNodeKeyword(ExcludeNodeKeyword::SameSpeciesKeyword));
					else sameSpecies_.add(contextStack.siteNode(parser.argc(n)), index);
				}
				break;
			case (ExcludeNodeKeyword::nExcludeNodeKeywords):
				return Messenger::error("Unrecognised Exclude node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Exclude node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisExcludeNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
