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
AtomTypesBlock::AtomTypesKeyword AtomTypesBlock::keyword(const char* s)
{
	for (int n=0; n<nAtomTypesKeywords; ++n) if (DUQSys::sameString(s,AtomTypesBlockData[n].name)) return (AtomTypesBlock::AtomTypesKeyword) n;
	return nAtomTypesKeywords;
}

// Convert AtomTypesKeyword to text string
const char* AtomTypesBlock::keyword(AtomTypesBlock::AtomTypesKeyword id)
{
	return AtomTypesBlockData[id].name;
}

// Return minimum number of expected arguments
int AtomTypesBlock::nArguments(AtomTypesBlock::AtomTypesKeyword id)
{
	return AtomTypesBlockData[id].nArguments;
}

// Parse AtomTypes block
bool AtomTypesBlock::parse(LineParser& parser, DUQ* duq)
{
	Messenger::print("\nParsing %s block...\n", InputBlocks::inputBlock(InputBlocks::AtomTypesBlock));
	
	int el;
	AtomType* at;
	Parameters* params;
	bool blockDone = false, error = false;
	
	while (!parser.eofOrBlank(duq->worldPool()))
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(duq->worldPool(), LineParser::SkipBlanks+LineParser::UseQuotes);
		AtomTypesBlock::AtomTypesKeyword atKeyword = AtomTypesBlock::keyword(parser.argc(0));
		if ((atKeyword != AtomTypesBlock::nAtomTypesKeywords) && ((parser.nArgs()-1) < AtomTypesBlock::nArguments(atKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", AtomTypesBlock::keyword(atKeyword));
			error = true;
			break;
		}
		switch (atKeyword)
		{
			case (AtomTypesBlock::AtomTypeKeyword):
				el = PeriodicTable::find(parser.argc(2));
				if (el == -1)
				{
					Messenger::error("Unrecognised element symbol '%s' found in %s keyword.\n", parser.argc(2), AtomTypesBlock::keyword(AtomTypesBlock::AtomTypeKeyword));
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
			case (AtomTypesBlock::EndAtomTypesKeyword):
				blockDone = true;
				break;
			case (AtomTypesBlock::nAtomTypesKeywords):
				Messenger::error("Unrecognised %s block keyword found - '%s'\n", InputBlocks::inputBlock(InputBlocks::AtomTypesBlock), parser.argc(0));
				InputBlocks::printValidKeywords(InputBlocks::AtomTypesBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", InputBlocks::inputBlock(InputBlocks::AtomTypesBlock), AtomTypesBlock::keyword(atKeyword));
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
