/*
	*** Keyword Parsing - Module Block
	*** src/main/keywords_module.cpp
	Copyright T. Youngs 2012-2018

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
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Module Block Keywords
KeywordData ModuleBlockData[] = {
	{ "Configuration",		1,	"Associates the named Configuration to this Module" },
	{ "Data",			0,	"Data block that we are associating directly with this Module" },
	{ "Disabled",			0,	"Specifies that the Module should never be run" },
	{ "EndModule",			0,	"Marks the end of a Module block" },
	{ "Frequency",			1,	"Frequency, relative to the main loop, at which this Module is run" },
	{ "Isotopologue",		3,	"Sets the relative population of a Species Isotopologue for a specific Configuration" }
};

// Convert text string to ModuleKeyword
ModuleBlock::ModuleKeyword ModuleBlock::keyword(const char* s)
{
	for (int n=0; n<ModuleBlock::nModuleKeywords; ++n) if (DUQSys::sameString(s,ModuleBlockData[n].name)) return (ModuleBlock::ModuleKeyword) n;
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
bool ModuleBlock::parse(LineParser& parser, DUQ* duq, Module* module, GenericList& targetList, bool moduleInConfiguration)
{
	Messenger::print("\nParsing %s block '%s'...\n", InputBlocks::inputBlock(InputBlocks::ModuleBlock), module->name());

	Configuration* targetCfg;
	Parameters* params;
	Species* sp;
	Data* data;
	Isotopologue* tope;
	CharString varName, dataName;
	bool blockDone = false, error = false;
	int argIndex;
	
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
				targetCfg = duq->findConfiguration(parser.argc(1));
				if (!targetCfg)
				{
					Messenger::error("Can't associate Configuration '%s' to the Module '%s', since no Configuration by this name exists.\n", parser.argc(1), module->name());
					error = true;
					break;
				}

				// Add it is a target
				if (!module->addConfigurationTarget(targetCfg))
				{
					Messenger::error("Failed to add Configuration target in Module '%s'.\n", module->name());
					error = true;
					break;
				}
				break;
			case (ModuleBlock::DataKeyword):
				// Was a specific name for the Data provided? If so, we assume we are linking, and don't attempt to parse a block
				dataName = parser.hasArg(1) ? parser.argc(1) : module->uniqueName();
				data = duq->findData(dataName);
				if (data)
				{
					module->addDataTarget(data);
					Messenger::print("Added existing Data target '%s' to Module '%s'.\n", data->name(), module->name());
				}
				else
				{
					// Create new Data structure and parse it
					data = duq->addData();
					data->setAssociatedModule(module);
					if (!DataBlock::parse(parser, duq, data)) return false;
				}
				break;
			case (ModuleBlock::DisableKeyword):
				module->setEnabled(false);
				break;
			case (ModuleBlock::EndModuleKeyword):
				Messenger::print("Found end of %s block.\n", InputBlocks::inputBlock(InputBlocks::ModuleBlock));
				blockDone = true;
				break;
			case (ModuleBlock::FrequencyKeyword):
				module->setFrequency(parser.argi(1));
				break;
			case (ModuleBlock::nModuleKeywords):
				// Never used, since it is accounted for in the beginning 'if'
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", InputBlocks::inputBlock(InputBlocks::ModuleBlock), ModuleBlock::keyword(modKeyword));
				error = true;
				break;
		}
		else
		{
			// Might be a keyword defined in the Module itself?
			int result = module->parseKeyword(parser, duq, targetList, module->uniqueName());
			if (result != 1)
			{
				if (result == -1) Messenger::error("Unrecognised %s block keyword '%s' found, and the Module '%s' contains no option with this name.\n", InputBlocks::inputBlock(InputBlocks::ModuleBlock), parser.argc(0), module->name());
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
