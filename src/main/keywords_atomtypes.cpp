/*
	*** Keyword Parsing - AtomTypes Block
	*** src/main/keywords_atomtypes.cpp
	Copyright T. Youngs 2012-2016

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main/keywords.h"
#include "main/duq.h"
#include "classes/atomtype.h"
#include "base/sysfunc.h"

// AtomTypes Block Keywords
KeywordData AtomTypesBlockData[] = {
	{ "AtomType",		3, 	"string name, string element, string parameters" },
	{ "EndAtomTypes",	0,	"" }
};

// Convert text string to AtomTypesKeyword
Keywords::AtomTypesKeyword Keywords::atomTypesKeyword(const char* s)
{
	for (int n=0; n<nAtomTypesKeywords; ++n) if (DUQSys::sameString(s,AtomTypesBlockData[n].name)) return (Keywords::AtomTypesKeyword) n;
	return nAtomTypesKeywords;
}

// Convert AtomTypesKeyword to text string
const char* Keywords::atomTypesKeyword(Keywords::AtomTypesKeyword id)
{
	return AtomTypesBlockData[id].name;
}

// Return minimum number of expected arguments
int Keywords::atomTypesBlockNArguments(Keywords::AtomTypesKeyword id)
{
	return AtomTypesBlockData[id].nArguments;
}

// Parse AtomTypes block
bool Keywords::parseAtomTypesBlock(LineParser& parser, DUQ* duq)
{
	Messenger::print("Parsing %s block\n", Keywords::inputBlock(Keywords::AtomTypesBlock));
	
	int el;
	AtomType* at;
	Parameters* params;
	bool blockDone = false, error = false;
	
	while (!parser.eofOrBlank(duq->worldPool()))
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(duq->worldPool(), LineParser::SkipBlanks+LineParser::UseQuotes);
		Keywords::AtomTypesKeyword atKeyword = Keywords::atomTypesKeyword(parser.argc(0));
		if ((atKeyword != Keywords::nAtomTypesKeywords) && ((parser.nArgs()-1) < Keywords::atomTypesBlockNArguments(atKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", Keywords::atomTypesKeyword(atKeyword));
			error = true;
			break;
		}
		switch (atKeyword)
		{
			case (Keywords::AtomTypeKeyword):
				el = PeriodicTable::find(parser.argc(2));
				if (el == -1)
				{
					Messenger::error("Unrecognised element symbol '%s' found in %s keyword.\n", parser.argc(2), Keywords::atomTypesKeyword(Keywords::AtomTypeKeyword));
					el = 0;
					error = true;
					break;
				}
				params = PeriodicTable::element(el).findParameters(parser.argc(3));
				if (!params)
				{
					Messenger::error("Couldn't find Parameters called '%s' when adding AtomType '%s'.\n", parser.argc(3), parser.argc(1));
					params = PeriodicTable::element(el).parameters();
					error = true;
					break;
				}
				at = duq->addAtomType(el);
				at->setName(parser.argc(1));
				at->setParameters(params);
				break;
			case (Keywords::EndAtomTypesKeyword):
				blockDone = true;
				break;
			case (Keywords::nAtomTypesKeywords):
				Messenger::error("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(Keywords::AtomTypesBlock), parser.argc(0));
				Keywords::printValidKeywords(Keywords::AtomTypesBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(Keywords::AtomTypesBlock), Keywords::atomTypesKeyword(atKeyword));
				error = true;
				break;
		}

		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	return (!error);
}
