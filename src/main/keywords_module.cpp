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
#include "genericitems/listhelper.h"

// Return enum option info for ModuleKeyword
EnumOptions<ModuleBlock::ModuleKeyword> ModuleBlock::keywords()
{
	static EnumOptionsList ModuleKeywords = EnumOptionsList() <<
		EnumOption(ModuleBlock::ConfigurationKeyword, 		"Configuration",	1) <<
		EnumOption(ModuleBlock::DisableKeyword, 		"Disabled") <<
		EnumOption(ModuleBlock::EndModuleKeyword,		"EndModule") <<
		EnumOption(ModuleBlock::FrequencyKeyword, 		"Frequency",		1);

	static EnumOptions<ModuleBlock::ModuleKeyword> options("ModuleKeyword", ModuleKeywords);

	return options;
}

// Parse Module block
bool ModuleBlock::parse(LineParser& parser, Dissolve* dissolve, Module* module, GenericList& targetList, bool moduleInConfiguration)
{
	Messenger::print("\nParsing %s block '%s'...\n", BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword), module->type());

	Configuration* targetCfg;
	CharString varName, dataName;
	bool blockDone = false, error = false;
	
	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		if (parser.getArgsDelim() != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (keywords().isValid(parser.argc(0)))
		{
			ModuleKeyword kwd = keywords().enumeration(parser.argc(0));
			if (!keywords().validNArgs(kwd, parser.nArgs()-1)) return false;

			// All OK, so process the keyword
			switch (kwd)
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

					// Add it as a target
					if (!module->addTargetConfiguration(targetCfg))
					{
						Messenger::error("Failed to add Configuration target in Module '%s'.\n", module->type());
						error = true;
						break;
					}

					// Create weight data if a second argument was provided
					if (parser.hasArg(2)) GenericListHelper<double>::add(targetList, CharString("ConfigurationWeight_%s", targetCfg->niceName()), module->uniqueName()) = parser.argd(2);
					break;
				case (ModuleBlock::DisableKeyword):
					module->setEnabled(false);
					break;
				case (ModuleBlock::EndModuleKeyword):
					Messenger::print("Found end of %s block.\n", BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword));
					blockDone = true;
					break;
				case (ModuleBlock::FrequencyKeyword):
					module->setFrequency(parser.argi(1));
					break;
				case (ModuleBlock::nModuleKeywords):
					// Never used, since it is accounted for in the beginning 'if'
					break;
				default:
					printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword), keywords().keyword(kwd));
					error = true;
					break;
			}
		}
		else
		{
			// Might be a keyword defined in the Module itself?
			int result = module->parseKeyword(parser, dissolve, targetList, module->uniqueName());
			if (result == 0) error = true;
			else if (result == -1)
			{
				Messenger::error("Unrecognised %s block keyword '%s' found, and the Module '%s' contains no option with this name.\n", BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword), parser.argc(0), module->type());
				keywords().errorAndPrintValid(parser.argc(0));
				module->printValidKeywords();
				error = true;
			}
		}

		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	// If there's no error and the blockDone flag isn't set, return an error
	if (!error && !blockDone)
	{
		Messenger::error("Unterminated %s block found.\n", BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword));
		error = true;
	}

	return (!error);
}
