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
#include "base/sysfunc.h"

// Module Block Keywords
KeywordData ModuleBlockData[] = {
	{ "Disabled",			0,	"" },
	{ "EndModule",			0,	"" }
};

// Convert text string to ModuleKeyword
Keywords::ModuleKeyword Keywords::moduleKeyword(const char* s)
{
	for (int n=0; n<Keywords::nModuleKeywords; ++n) if (DUQSys::sameString(s,ModuleBlockData[n].name)) return (Keywords::ModuleKeyword) n;
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
bool Keywords::parseModuleBlock(LineParser& parser, DUQ* duq, Module* module, Configuration* cfg, Sample* sam)
{
	Messenger::print("Parsing %s block\n", Keywords::inputBlock(Keywords::ModuleBlock));

	int el;
	AtomType* at;
	Parameters* params;
	bool blockDone = false, error = false;
	
	while (!parser.eofOrBlank(duq->worldPool()))
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(duq->worldPool(), LineParser::SkipBlanks+LineParser::UseQuotes);
		Keywords::ModuleKeyword modKeyword = Keywords::moduleKeyword(parser.argc(0));
		if ((modKeyword != Keywords::nModuleKeywords) && ((parser.nArgs()-1) < Keywords::moduleBlockNArguments(modKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", Keywords::moduleKeyword(modKeyword));
			error = true;
			break;
		}
		else if (modKeyword != Keywords::nModuleKeywords) switch (modKeyword)
		{
			case (Keywords::DisabledModuleKeyword):
				module->setEnabled(false);
				break;
			case (Keywords::EndModuleKeyword):
				blockDone = true;
				break;
			case (Keywords::nModuleKeywords):
				Messenger::error("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(Keywords::ModuleBlock), parser.argc(0));
				Keywords::printValidKeywords(Keywords::ModuleBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(Keywords::ModuleBlock), Keywords::moduleKeyword(modKeyword));
				error = true;
				break;
		}
		else
		{
			// Might be a Module variable?
			// First of all, does the named variable exist?
			Variable* var = module->findVariable(parser.argc(0));
			if (!var)
			{
				Messenger::error("Unrecognised %s block keyword found - '%s', and the Module '%s' contains no variable of this name.\n", Keywords::inputBlock(Keywords::ModuleBlock), parser.argc(0), module->name());
				error = true;
				break;
			}
			// Set variable in Configuration / Sample as appropriate
			if (cfg) cfg->setModuleVariable(var->name(), parser.argc(1), var->description(), module->uniqueName());
// 			if (sam) sam->setVariable(var->name(), parser.argc(1), var->description(), module->name());
		}

		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	return (!error);
}
