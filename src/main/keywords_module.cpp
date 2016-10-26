/*
	*** Keyword Parsing - Module Block
	*** src/main/keywords_module.cpp
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
#include <string.h>

// Module Block Keywords
KeywordData ModuleBlockData[] = {
	{ "Disabled",			0,	"" },
	{ "EndModule",			0,	"" },
	{ "Set",			2,	"string variable, int|double|string value" }
};

// Convert text string to ModuleKeyword
Keywords::ModuleKeyword Keywords::moduleKeyword(const char* s)
{
	for (int n=0; n<Keywords::nModuleKeywords; ++n) if (strcmp(s,ModuleBlockData[n].name) == 0) return (Keywords::ModuleKeyword) n;
	return Keywords::nModuleKeywords;
}

// Convert ModuleKeyword to text string
const char* Keywords::moduleKeyword(Keywords::ModuleKeyword id)
{
	return ModuleBlockData[id].name;
}

// Return minimum number of expected arguments
int Keywords::moduleBlockNArguments(Keywords::ModuleKeyword id)
{
	return ModuleBlockData[id].nArguments;
}

// Parse Module block
bool Keywords::parseModuleBlock(LineParser& parser, DUQ* duq, Configuration* cfg)
{
	Messenger::print("Parsing %s block\n", Keywords::inputBlock(Keywords::AtomTypesBlock));
	
	int el;
	AtomType* at;
	Parameters* params;
	bool blockDone = false, error = false;
	
	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
		Keywords::AtomTypesKeyword atKeyword = Keywords::atomTypesKeyword(parser.argc(0));
		if ((atKeyword != Keywords::nAtomTypesKeywords) && ((parser.nArgs()-1) < Keywords::atomTypesBlockNArguments(atKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", Keywords::atomTypesKeyword(atKeyword));
			error = true;
			break;
		}
		switch (atKeyword)
		{

			case (Keywords::EndAtomTypesKeyword):
				blockDone = true;
				break;
			case (Keywords::nAtomTypesKeywords):
				Messenger::error("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(Keywords::ModuleBlock), parser.argc(0));
				Keywords::printValidKeywords(Keywords::ModuleBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(Keywords::ModuleBlock), Keywords::atomTypesKeyword(atKeyword));
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
