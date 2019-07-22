/*
	*** Keyword Parsing - Configuration Block
	*** src/main/keywords_configuration.cpp
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
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Configuration Block Keywords
KeywordData ConfigurationBlockData[] = {
	{ "CellDivisionLength",		1,	"Requested side length for regions when partitioning the unit cell" },
	{ "EndConfiguration",		0,	"Signals the end of the Configuration block" },
	{ "Generator",			0,	"Define the generator procedure for the Configuration" },
	{ "InputCoordinates",		2,	"Format and filename which contains the starting coordinates" },
	{ "Module",			1,	"Starts the set up of a Module for this Configuration" },
	{ "SizeFactor",			1,	"Scaling factor for Box lengths, Cell size, and Molecule centres-of-geometry" },
	{ "Temperature",		1,	"Simulation temperature of the Configuration" }
};

// Convert text string to ConfigurationKeyword
ConfigurationBlock::ConfigurationKeyword ConfigurationBlock::keyword(const char* s)
{
	for (int n=0; n<ConfigurationBlock::nConfigurationKeywords; ++n) if (DissolveSys::sameString(s,ConfigurationBlockData[n].name)) return (ConfigurationBlock::ConfigurationKeyword) n;
	return ConfigurationBlock::nConfigurationKeywords;
}

// Convert ConfigurationKeyword to text string
const char* ConfigurationBlock::keyword(ConfigurationBlock::ConfigurationKeyword id)
{
	return ConfigurationBlockData[id].name;
}

// Return minimum number of expected arguments
int ConfigurationBlock::nArguments(ConfigurationBlock::ConfigurationKeyword id)
{
	return ConfigurationBlockData[id].nArguments;
}

// Parse Configuration block
bool ConfigurationBlock::parse(LineParser& parser, Dissolve* dissolve, Configuration* cfg)
{
	Messenger::print("\nParsing %s block '%s'...\n", BlockKeywords::blockKeyword(BlockKeywords::ConfigurationBlockKeyword), cfg->name());

	Species* sp;
	Module* module;
	CharString niceName;
	SpeciesInfo* spInfo;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);
		ConfigurationBlock::ConfigurationKeyword conKeyword = ConfigurationBlock::keyword(parser.argc(0));
		if ((conKeyword != ConfigurationBlock::nConfigurationKeywords) && ((parser.nArgs()-1) < ConfigurationBlock::nArguments(conKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", ConfigurationBlock::keyword(conKeyword));
			error = true;
			break;
		}
		switch (conKeyword)
		{
			case (ConfigurationBlock::CellDivisionLengthKeyword):
				cfg->setRequestedCellDivisionLength(parser.argd(1));
				break;
			case (ConfigurationBlock::EndConfigurationKeyword):
				Messenger::print("Found end of %s block.\n", BlockKeywords::blockKeyword(BlockKeywords::ConfigurationBlockKeyword));
				blockDone = true;
				break;
			case (ConfigurationBlock::GeneratorKeyword):
				if (!cfg->readGenerator(parser, dissolve->coreData()))
				{
					Messenger::error("Failed to read generator procedure for Configuration.\n");
					error = true;
				}
				break;
			case (ConfigurationBlock::InputCoordinatesKeyword):
				if (!cfg->inputCoordinates().read(parser, 1))
				{
					Messenger::error("Failed to set input coordinates file / format.\n");
					error = true;
					break;
				}
				Messenger::print("Initial coordinates will be loaded from file '%s' (%s)\n", cfg->inputCoordinates().filename(), cfg->inputCoordinates().format());
				break;
			case (ConfigurationBlock::ModuleKeyword):
				// The argument following the keyword is the module name, so try to create an instance of that Module
				module = dissolve->createModuleInstance(parser.argc(1));
				if (!module)
				{
					error = true;
					break;
				}

				// Add the new instance to the current Configuration
				if (cfg->addModule(module))
				{
					// Add our pointer to the Module's list of associated Configurations
					if (!module->addTargetConfiguration(cfg))
					{
						Messenger::error("Failed to add Configuration '%s' to Module '%s' as a target.\n", cfg->name(), module->type());
						error = true;
					}
				}
				else
				{
					Messenger::error("Failed to add Module '%s' to Configuration.\n", parser.argc(1));
					error = true;
				}
				if (error) break;
				
				// Set unique name, if it was provided - need to check if it has been used elsewhere (in any Module or instance of it, or any Configuration)
				if (parser.hasArg(2))
				{
					niceName = DissolveSys::niceName(parser.argc(2));
					Module* existingModule = dissolve->findModuleInstance(niceName);
					if (existingModule && (existingModule != module))
					{
						Messenger::error("A Module with the unique name '%s' already exist.\n", niceName.get());
						error = true;
						break;
					}
					else if (dissolve->findConfigurationByNiceName(niceName))
					{
						Messenger::error("A Configuration with the unique name '%s' already exist, and so cannot be used as a Module name.\n", niceName.get());
						error = true;
						break;
					}
					else module->setUniqueName(niceName);
				}

				// Parse rest of Module block
				module->setConfigurationLocal(true);
				if (!ModuleBlock::parse(parser, dissolve, module, cfg->moduleData(), true)) error = true;
				break;
			case (ConfigurationBlock::SizeFactorKeyword):
				cfg->setRequestedSizeFactor(parser.argd(1));
				break;
			case (ConfigurationBlock::TemperatureKeyword):
				cfg->setTemperature(parser.argd(1));
				break;
			case (ConfigurationBlock::nConfigurationKeywords):
				Messenger::error("Unrecognised %s block keyword '%s' found.\n", BlockKeywords::blockKeyword(BlockKeywords::ConfigurationBlockKeyword), parser.argc(0));
				BlockKeywords::printValidKeywords(BlockKeywords::ConfigurationBlockKeyword);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::blockKeyword(BlockKeywords::ConfigurationBlockKeyword), ConfigurationBlock::keyword(conKeyword));
				error = true;
				break;
		}
		
		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	// If there's no error and the blockDone flag isn't set, return an error
	if (!error && !blockDone)
	{
		Messenger::error("Unterminated %s block found.\n", BlockKeywords::blockKeyword(BlockKeywords::ConfigurationBlockKeyword));
		error = true;
	}

	return (!error);
}
