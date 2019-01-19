/*
	*** Keyword Parsing - Module Block
	*** src/main/keywords_module.cpp
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

#include "main/keywords.h"
#include "main/dissolve.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Module Block Keywords
KeywordData ModuleBlockData[] = {
	{ "Configuration",		1,	"Associates the named Configuration to this Module" },
	{ "Disabled",			0,	"Specifies that the Module should never be run" },
	{ "EndModule",			0,	"Marks the end of a Module block" },
	{ "Frequency",			1,	"Frequency, relative to the main loop, at which this Module is run" },
	{ "Isotopologue",		3,	"Sets the relative population of a Species Isotopologue for a specific Configuration" }
};

// Convert text string to ModuleKeyword
ModuleBlock::ModuleKeyword ModuleBlock::keyword(const char* s)
{
	for (int n=0; n<ModuleBlock::nModuleKeywords; ++n) if (DissolveSys::sameString(s,ModuleBlockData[n].name)) return (ModuleBlock::ModuleKeyword) n;
	return ModuleBlock::nModuleKeywords;
}

// Convert ModuleKeyword to text string
const char* ModuleBlock::keyword(ModuleBlock::ModuleKeyword id)
{
	return ModuleBlockData[id].name;
}

// Return minimum number of expected arguments
int ModuleBlock::nArguments(ModuleBlock::ModuleKeyword id)
{
	return ModuleBlockData[id].nArguments;
}

// Parse Module block
bool ModuleBlock::parse(LineParser& parser, Dissolve* dissolve, Module* module, GenericList& targetList, bool moduleInConfiguration)
{
	Messenger::print("\nParsing %s block '%s'...\n", BlockKeywords::blockKeyword(BlockKeywords::ModuleBlockKeyword), module->type());

	Configuration* targetCfg;
	CharString varName, dataName;
	bool blockDone = false, error = false;
	
	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);
		ModuleBlock::ModuleKeyword modKeyword = ModuleBlock::keyword(parser.argc(0));
		if ((modKeyword != ModuleBlock::nModuleKeywords) && ((parser.nArgs()-1) < ModuleBlock::nArguments(modKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", ModuleBlock::keyword(modKeyword));
			error = true;
			break;
		}
		else if (modKeyword != ModuleBlock::nModuleKeywords) switch (modKeyword)
		{
			case (ModuleBlock::ConfigurationKeyword):
				// Find the named Configuration
				targetCfg = dissolve->findConfiguration(parser.argc(1));
				if (!targetCfg)
				{
					Messenger::error("Can't associate Configuration '%s' to the Module '%s', since no Configuration by this name exists.\n", parser.argc(1), module->type());
					error = true;
					break;
				}

				// Add it is a target
				if (!module->addConfigurationTarget(targetCfg))
				{
					Messenger::error("Failed to add Configuration target in Module '%s'.\n", module->type());
					error = true;
					break;
				}
				break;
			case (ModuleBlock::DisableKeyword):
				module->setEnabled(false);
				break;
			case (ModuleBlock::EndModuleKeyword):
				Messenger::print("Found end of %s block.\n", BlockKeywords::blockKeyword(BlockKeywords::ModuleBlockKeyword));
				blockDone = true;
				break;
			case (ModuleBlock::FrequencyKeyword):
				module->setFrequency(parser.argi(1));
				break;
			case (ModuleBlock::nModuleKeywords):
				// Never used, since it is accounted for in the beginning 'if'
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::blockKeyword(BlockKeywords::ModuleBlockKeyword), ModuleBlock::keyword(modKeyword));
				error = true;
				break;
		}
		else
		{
			// Might be a keyword defined in the Module itself?
			int result = module->parseKeyword(parser, dissolve, targetList, module->uniqueName());
			if (result == 0) error = true;
			else if (result == -1)
			{
				Messenger::error("Unrecognised %s block keyword '%s' found, and the Module '%s' contains no option with this name.\n", BlockKeywords::blockKeyword(BlockKeywords::ModuleBlockKeyword), parser.argc(0), module->type());
				module->printValidKeywords();
				error = true;
			}
		}

		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	return (!error);
}
