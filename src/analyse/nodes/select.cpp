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
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AnalysisSelectNode::AnalysisSelectNode() : AnalysisNode()
{
}

// Destructor
AnalysisSelectNode::~AnalysisSelectNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords
const char* SelectNodeKeywords[] = { "_DUMMY_" };

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
bool AnalysisSelectNode::read(LineParser& parser)
{
	// Read until we encounter the EndAnalyser keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		SelectNodeKeyword nk = selectNodeKeyword(parser.argc(0));
		switch (nk)
		{
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
