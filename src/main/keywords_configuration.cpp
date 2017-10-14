/*
	*** Keyword Parsing - Configuration Block
	*** src/main/keywords_configuration.cpp
	Copyright T. Youngs 2012-2017

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
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Configuration Block Keywords
KeywordData ConfigurationBlockData[] = {
	{ "BoxNormalisationFile", 	1,	"File from which to load the RDF normalisation array" },
	{ "BraggMultiplicity",		3,	"Multiplicity of a primitive cell within the unit cell" },
	{ "BraggQMax",			1,	"Maximum Q value for Bragg calculation" },
	{ "BraggQMin",			1,	"Minimum Q value for Bragg calculation" },
	{ "CellAngles", 		3,	"Angles of the unit cell" },
	{ "CellDivisionLength",		1,	"Requested side length for regions when partitioning the unit cell" },
	{ "CellLengths",		3,	"Relative lengths of the unit cell" },
	{ "Density",			2,	"Density of the Configuration, along with its units" },
	{ "EndConfiguration",		0,	"Signals the end of the Configuration block" },
	{ "Ensemble",			1,	"Whether an ensemble file should be appended to" },
	{ "EnsembleFrequency",		1,	"Frequency at which to append ensemble file" },
	{ "InputCoordinates",		2,	"Format and filename which contains the starting coordinates" },
	{ "Module",			1,	"Starts the setup of a Module for this Configuration" },
	{ "Multiplier",			1,	"Factor by which relative populations are multiplied when generating the Configuration data" },
	{ "NonPeriodic",		0,	"States that the simulation should be treated as non-periodic" },
	{ "OutputCoordinates",		1,	"File which should contain output coordinates, followed by the (optional) output frequency" },
	{ "RDFBinWidth",		1,	"Bin width for all radial distribution functions" },
	{ "RDFRange",			1,	"Requested extent for calculated radial distribution functions" },
	{ "SpeciesInfo",		1,	"Specify a Species to add to this Configuration" },
	{ "Temperature",		1,	"Simulation temperature of the Configuration" },
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
			case (ConfigurationBlock::BoxNormalisationFileKeyword):
				cfg->setBoxNormalisationFile(parser.argc(1));
				break;
			case (ConfigurationBlock::BraggQMaxKeyword):
				cfg->setBraggQMax(parser.argd(1));
				break;
			case (ConfigurationBlock::BraggQMinKeyword):
				cfg->setBraggQMin(parser.argd(1));
				break;
			case (ConfigurationBlock::BraggMultiplicityKeyword):
				cfg->setBraggMultiplicity(parser.arg3i(1));
				break;
			case (ConfigurationBlock::CellAnglesKeyword):
				cfg->setBoxAngles(Vec3<double>(parser.argd(1),  parser.argd(2), parser.argd(3)));
				break;
			case (ConfigurationBlock::CellDivisionLengthKeyword):
				cfg->setRequestedCellDivisionLength(parser.argd(1));
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
			case (ConfigurationBlock::EnsembleKeyword):
				cfg->setAppendEnsemble(parser.argb(1));
				break;
			case (ConfigurationBlock::EnsembleFrequencyKeyword):
				cfg->setEnsembleFrequency(parser.argi(1));
				break;
			case (ConfigurationBlock::InputCoordinatesKeyword):
				cfg->setInputCoordinatesFormat(parser.argc(1));
				cfg->setInputCoordinatesFile(parser.argc(2));
				cfg->setRandomConfiguration(false);
				Messenger::print("--> Initial coordinates will be loaded from file '%s' (format: %s)\n", cfg->inputCoordinatesFile(), cfg->inputCoordinatesFormat());
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
				
				// Set unique name, if it was provided - need to check if it has been used elsewhere (in any Module or instance of it, or any Configuration)
				if (parser.hasArg(2))
				{
					niceName = DUQSys::niceName(parser.argc(2));
					if (ModuleList::findInstanceByUniqueName(niceName))
					{
						Messenger::error("A Module with the unique name '%s' already exist.\n", niceName.get());
						error = true;
						break;
					}
					else if (duq->findConfiguration(niceName, true))
					{
						Messenger::error("A Configuration with the unique name '%s' already exist, and so cannot be used as a Module name.\n", niceName.get());
						error = true;
						break;
					}
					else module->setUniqueName(niceName);
				}

				// Parse rest of Module block
				module->setConfigurationLocal(true);
				if (!ModuleBlock::parse(parser, duq, module, cfg->moduleData(), true)) error = true;

				// Now finished parsing the Module block, so must update target Samples and Configurations in any auto-added Modules
				module->updateDependentTargets();
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
			case (ConfigurationBlock::SpeciesInfoKeyword):
				sp = duq->findSpecies(parser.argc(1));
				if (sp == NULL)
				{
					Messenger::error("Configuration refers to Species '%s', but no such Species is defined.\n", parser.argc(1));
					error = true;
				}
				else
				{
					// Are we already using this species
					if (cfg->hasUsedSpecies(sp))
					{
						Messenger::error("Configuration already uses Species '%s' - cannot add it a second time.\n", sp->name());
						error = true;
					}

					// Add this species to the list of those used by the Configuration
					spInfo = cfg->addUsedSpecies(sp, parser.hasArg(2) ? parser.argd(2) : 1.0);

					// If no population was given, assume that a block will follow with additional information
					if (!parser.hasArg(2))
					{
						if (!SpeciesInfoBlock::parse(parser, duq, spInfo))
						{
							error = true;
							break;
						}
					}

					Messenger::print("--> Set composition for Species '%s' (relative population = %f).\n", sp->name(), spInfo->population());
				}
				break;
			case (ConfigurationBlock::TemperatureKeyword):
				cfg->setTemperature(parser.argd(1));
				break;
			case (ConfigurationBlock::UseOutputAsInputKeyword):
				cfg->setUseOutputCoordinatesAsInput(true);
				break;
			case (ConfigurationBlock::nConfigurationKeywords):
				Messenger::error("Unrecognised %s block keyword '%s' found. block keyword found - '%s'\n", InputBlocks::inputBlock(InputBlocks::ConfigurationBlock), parser.argc(0));
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
