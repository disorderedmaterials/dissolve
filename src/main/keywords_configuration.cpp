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
#include <string.h>

// Configuration Block Keywords
KeywordData ConfigurationBlockData[] = {
	{ "BoxNormalisationFile", 	1,	"" },
	{ "Bragg", 			1, 	"" },
	{ "BraggBroadening", 		1,	"" },
	{ "BraggMaximumQ",		1,	"" },
	{ "CellAngles", 		3,	"" },
	{ "CellLengths",		3,	"" },
	{ "Density",			1,	"" },
	{ "EndConfiguration",		0,	"" },
	{ "File",			1,	"" },
	{ "Module",			1,	"" },
	{ "Multiplier",			1,	"" },
	{ "NonPeriodic",		0,	"" },
	{ "QDelta",			1,	"" },
	{ "QMax",			1,	"" },
	{ "RDFBinWidth",		1,	"" },
	{ "RDFMethod",			1,	"" },
	{ "RDFRange",			1,	"" },
	{ "RDFSmoothing",		1,	"" },
	{ "RMSEDeltaQ",			1,	"" },
	{ "Sample",			1,	"" },
	{ "Species",			2,	"" },
	{ "Temperature",		1,	"" }
};

// Convert text string to ConfigurationKeyword
Keywords::ConfigurationKeyword Keywords::configurationKeyword(const char* s)
{
	for (int n=0; n<Keywords::nConfigurationKeywords; ++n) if (strcmp(s,ConfigurationBlockData[n].name) == 0) return (Keywords::ConfigurationKeyword) n;
	return Keywords::nConfigurationKeywords;
}

// Convert ConfigurationKeyword to text string
const char* Keywords::configurationKeyword(Keywords::ConfigurationKeyword id)
{
	return ConfigurationBlockData[id].name;
}

// Return minimum number of expected arguments
int Keywords::configurationBlockNArguments(Keywords::ConfigurationKeyword id)
{
	return ConfigurationBlockData[id].nArguments;
}

// Parse Configuration block
bool Keywords::parseConfigurationBlock(LineParser& parser, DUQ* duq, Configuration* cfg)
{
	Messenger::print("Parsing %s block\n", Keywords::inputBlock(Keywords::ConfigurationBlock));

	Sample* sam;
	Species* sp;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
		Keywords::ConfigurationKeyword conKeyword = Keywords::configurationKeyword(parser.argc(0));
		if ((conKeyword != Keywords::nConfigurationKeywords) && ((parser.nArgs()-1) < Keywords::configurationBlockNArguments(conKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", Keywords::configurationKeyword(conKeyword));
			error = true;
			break;
		}
		switch (conKeyword)
		{
			case (Keywords::BoxNormalisationFileKeyword):
				cfg->setBoxNormalisationFile(parser.argc(1));
				break;
			case (Keywords::BraggKeyword):
				cfg->setBraggCalculationOn(parser.argb(1));
				break;
			case (Keywords::BraggBroadeningKeyword):
				cfg->setBraggBroadening(parser.argd(1));
				break;
			case (Keywords::BraggMaximumQKeyword):
				cfg->setBraggMaximumQ(parser.argd(1));
				break;
			case (Keywords::CellAnglesKeyword):
				cfg->setBoxAngles(Vec3<double>(parser.argd(1),  parser.argd(2), parser.argd(3)));
				break;
			case (Keywords::CellLengthsKeyword):
				cfg->setRelativeBoxLengths(Vec3<double>(parser.argd(1), parser.argd(2), parser.argd(3)));
				break;
			case (Keywords::DensityKeyword):
				// Determine units given
				if (strcmp(parser.argc(2),"atoms/A3") == 0) cfg->setAtomicDensity(parser.argd(1));
				else if (strcmp(parser.argc(2),"g/cm3") == 0) cfg->setChemicalDensity(parser.argd(1));
				else
				{
					Messenger::error("Unrecognised density unit given - '%s'\nValid values are 'atoms/A3' or 'g/cm3'.\n", parser.argc(2));
					error = true;
				}
				break;

			case (Keywords::FileModelKeyword):
				cfg->setInitialCoordinatesFile(parser.argc(1));
				cfg->setRandomConfiguration(false);
				Messenger::print("--> Initial coordinates will be loaded from file '%s'\n", parser.argc(1));
				break;
			case (Keywords::MultiplierKeyword):
				cfg->setMultiplier(parser.argd(1));
				Messenger::print("--> Set configuration contents multiplier to %i\n", cfg->multiplier());
				break;
			case (Keywords::NonPeriodicKeyword):
				cfg->setNonPeriodic(true);
				Messenger::print("--> Flag set for a non-periodic calculation.\n");
				break;
			case (Keywords::EndConfigurationKeyword):
				Messenger::print("Found end of %s block.\n", Keywords::inputBlock(Keywords::ConfigurationBlock));
				blockDone = true;
				break;
			case (Keywords::QDeltaKeyword):
				cfg->setQDelta(parser.argd(1));
				break;
			case (Keywords::QMaxKeyword):
				cfg->setQMax(parser.argd(1));
				break;
			case (Keywords::RDFBinWidthKeyword):
				cfg->setRDFBinWidth(parser.argd(1));
				break;
			case (Keywords::RDFMethodKeyword):
				if (Configuration::rdfMethod(parser.argc(1)) == Configuration::nRDFMethods)
				{
					Messenger::error("'%s' is not a valid calculation method.\n", parser.argc(1));
					error = true;
				}
				else cfg->setRDFMethod(Configuration::rdfMethod(parser.argc(1)));
				break;
			case (Keywords::RDFRangeKeyword):
				cfg->setRequestedRDFRange(parser.argd(1));
				break;
			case (Keywords::RDFSmoothingKeyword):
				cfg->setRDFSmoothing(parser.argi(1));
				break;
			case (Keywords::SampleAddKeyword):
				sam = duq->findSample(parser.argc(1));
				if (sam == NULL)
				{
					Messenger::error("Configuration refers to Sample '%s', but no such Sample is defined.\n", parser.argc(1));
					error = true;
				}
				else
				{
					// Add this species to the list of those used by the Configuration
					if (!cfg->addReferenceSample(sam))
					{
						Messenger::error("Configuration already references Sample '%s' - cannot add it a second time.\n", sam->name());
						error = true;
					}
					else Messenger::print("--> Added Sample '%s' as calculation target for Configuration.\n", sam->name());
				}
				break;
			case (Keywords::SpeciesAddKeyword):
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
					else Messenger::print("--> Set composition for Species '%s' (%f relative population).\n", sp->name(), parser.argd(2));
				}
				break;
			case (Keywords::TemperatureKeyword):
				cfg->setTemperature(parser.argd(1));
				break;
			case (Keywords::nConfigurationKeywords):
				Messenger::error("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(Keywords::ConfigurationBlock), parser.argc(0));
				Keywords::printValidKeywords(Keywords::ConfigurationBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(Keywords::ConfigurationBlock), Keywords::configurationKeyword(conKeyword));
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
