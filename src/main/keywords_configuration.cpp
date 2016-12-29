/*
	*** Keyword Parsing - Configuration Block
	*** src/main/keywords_configuration.cpp
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
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/sysfunc.h"

// Configuration Block Keywords
KeywordData ConfigurationBlockData[] = {
	{ "BoxNormalisationFile", 	1,	"Specifies a file from which to load the RDF normalisation array" },
	{ "BraggMaximumQ",		1,	"Sets the maximum Q value for Bragg calculation" },
	{ "CellAngles", 		3,	"Gives the angles of the unit cell" },
	{ "CellLengths",		3,	"Gives the relative lengths of the unit cell" },
	{ "Density",			2,	"Specifies the density of the Configuration, along with its units" },
	{ "EndConfiguration",		0,	"Signals the end of the Configuration block" },
	{ "InputCoordinates",		1,	"Specifies the file which contains the starting coordinates" },
	{ "Module",			1,	"Starts the setup of a Module for this Configuration" },
	{ "Multiplier",			1,	"Specifies the factor by which relative populations are multiplied when generating the Configuration data" },
	{ "NonPeriodic",		0,	"States that the simulation should be treated as non-periodic" },
	{ "OutputCoordinates",		1,	"Specifies the file which should contain output coordinates, and optionally the output frequency" },
	{ "RDFBinWidth",		1,	"Specified bin width for all radial distribution functions" },
	{ "RDFRange",			1,	"Requested extent for calculated radial distribution functions" },
	{ "Species",			2,	"Specifies a Species and its relative population to add to this Configuration" },
	{ "Temperature",		1,	"Defines the temperature of the Configuration" },
	{ "UseOutputAsInput",		0,	"Use output coordinates file as input (if it exists)" }
};

// Convert text string to ConfigurationKeyword
ConfigurationBlock::ConfigurationKeyword ConfigurationBlock::keyword(const char* s)
{
	for (int n=0; n<ConfigurationBlock::nConfigurationKeywords; ++n) if (DUQSys::sameString(s,ConfigurationBlockData[n].name)) return (ConfigurationBlock::ConfigurationKeyword) n;
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
bool ConfigurationBlock::parse(LineParser& parser, DUQ* duq, Configuration* cfg)
{
	Messenger::print("\nParsing %s block '%s'...\n", InputBlocks::inputBlock(InputBlocks::ConfigurationBlock), cfg->name());

	Sample* sam;
	Species* sp;
	Module* masterInstance, *module;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank(duq->worldPool()))
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(duq->worldPool(), LineParser::SkipBlanks+LineParser::UseQuotes);
		ConfigurationBlock::ConfigurationKeyword conKeyword = ConfigurationBlock::keyword(parser.argc(0));
		if ((conKeyword != ConfigurationBlock::nConfigurationKeywords) && ((parser.nArgs()-1) < ConfigurationBlock::nArguments(conKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", ConfigurationBlock::keyword(conKeyword));
			error = true;
			break;
		}
		switch (conKeyword)
		{
			case (ConfigurationBlock::BoxNormalisationFileKeyword):
				cfg->setBoxNormalisationFile(parser.argc(1));
				break;
			case (ConfigurationBlock::BraggMaximumQKeyword):
				cfg->setBraggMaximumQ(parser.argd(1));
				break;
			case (ConfigurationBlock::CellAnglesKeyword):
				cfg->setBoxAngles(Vec3<double>(parser.argd(1),  parser.argd(2), parser.argd(3)));
				break;
			case (ConfigurationBlock::CellLengthsKeyword):
				cfg->setRelativeBoxLengths(Vec3<double>(parser.argd(1), parser.argd(2), parser.argd(3)));
				break;
			case (ConfigurationBlock::DensityKeyword):
				// Determine units given
				if (DUQSys::sameString(parser.argc(2),"atoms/A3")) cfg->setAtomicDensity(parser.argd(1));
				else if (DUQSys::sameString(parser.argc(2),"g/cm3")) cfg->setChemicalDensity(parser.argd(1));
				else
				{
					Messenger::error("Unrecognised density unit given - '%s'\nValid values are 'atoms/A3' or 'g/cm3'.\n", parser.argc(2));
					error = true;
				}
				break;
			case (ConfigurationBlock::EndConfigurationKeyword):
				Messenger::print("Found end of %s block.\n", InputBlocks::inputBlock(InputBlocks::ConfigurationBlock));
				blockDone = true;
				break;
			case (ConfigurationBlock::InputCoordinatesKeyword):
				cfg->setInputCoordinatesFile(parser.argc(1));
				cfg->setRandomConfiguration(false);
				Messenger::print("--> Initial coordinates will be loaded from file '%s'\n", parser.argc(1));
				break;
			case (ConfigurationBlock::ModuleKeyword):
				// The argument following the keyword is the module name
				masterInstance = ModuleList::findMasterInstance(parser.argc(1));
				if (!masterInstance)
				{
					Messenger::error("No Module named '%s' exists.\n", parser.argc(1));
					error = true;
					break;
				}

				// Try to add this module (or an instance of it) to the current Configuration
				module = cfg->addModule(masterInstance, duq->autoAddDependentModules());
				if (module)
				{
					// Add our pointer to the Module's list of associated Configurations
					if (!module->addConfigurationTarget(cfg))
					{
						Messenger::error("Failed to add Configuration '%s' to Module '%s' as a target.\n", cfg->name(), module->name());
						error = true;
					}
				}
				else
				{
					Messenger::error("Failed to add Module '%s' to Configuration.\n", parser.argc(1));
					error = true;
				}
				if (error) break;

				// Parse rest of Module block
				if (!ModuleBlock::parse(parser, duq, masterInstance, cfg->moduleData(), true)) error = true;

				// Now finished parsing the Module block, so must update target Samples and Configurations in any auto-added Modules
				masterInstance->updateDependentTargets();
				break;
			case (ConfigurationBlock::MultiplierKeyword):
				cfg->setMultiplier(parser.argd(1));
				Messenger::print("--> Set Configuration contents multiplier to %i.\n", cfg->multiplier());
				break;
			case (ConfigurationBlock::NonPeriodicKeyword):
				cfg->setNonPeriodic(true);
				Messenger::print("--> Flag set for a non-periodic calculation.\n");
				break;
			case (ConfigurationBlock::OutputCoordinatesKeyword):
				cfg->setOutputCoordinatesFile(parser.argc(1));
				if (parser.hasArg(2)) cfg->setCoordinatesOutputFrequency(parser.argi(2));
				if (cfg->coordinatesOutputFrequency() == 1) Messenger::print("--> Output coordinates will be saved to file '%s' every iteration.\n", parser.argc(1));
				else Messenger::print("--> Output coordinates will be saved to file '%s' every %i iterations.\n", parser.argc(1), cfg->coordinatesOutputFrequency());
				break;
			case (ConfigurationBlock::RDFBinWidthKeyword):
				cfg->setRDFBinWidth(parser.argd(1));
				break;
			case (ConfigurationBlock::RDFRangeKeyword):
				cfg->setRequestedRDFRange(parser.argd(1));
				break;
			case (ConfigurationBlock::SpeciesAddKeyword):
				sp = duq->findSpecies(parser.argc(1));
				if (sp == NULL)
				{
					Messenger::error("Configuration refers to Species '%s', but no such Species is defined.\n", parser.argc(1));
					error = true;
				}
				else
				{
					// Add this species to the list of those used by the Configuration
					if (!cfg->addUsedSpecies(sp, parser.argd(2)))
					{
						Messenger::error("Configuration already uses Species '%s' - cannot add it a second time.\n", sp->name());
						error = true;
					}
					else Messenger::print("--> Set composition for Species '%s' (relative population = %f).\n", sp->name(), parser.argd(2));
				}
				break;
			case (ConfigurationBlock::TemperatureKeyword):
				cfg->setTemperature(parser.argd(1));
				break;
			case (ConfigurationBlock::UseOutputAsInputKeyword):
				cfg->setUseOutputCoordinatesAsInput(true);
				break;
			case (ConfigurationBlock::nConfigurationKeywords):
				Messenger::error("Unrecognised %s block keyword found - '%s'\n", InputBlocks::inputBlock(InputBlocks::ConfigurationBlock), parser.argc(0));
				InputBlocks::printValidKeywords(InputBlocks::ConfigurationBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", InputBlocks::inputBlock(InputBlocks::ConfigurationBlock), ConfigurationBlock::keyword(conKeyword));
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
