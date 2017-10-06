/*
	*** Keyword Parsing - SpeciesInfo Block
	*** src/main/keywords_speciesinfo.cpp
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
#include "classes/speciesinfo.h"
#include "classes/species.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// SpeciesInfo Block Keywords
KeywordData SpeciesInfoBlockData[] = {
	{ "EndSpeciesInfo",		0,	"Signals the end of the SpeciesInfo" },
	{ "NoRotation",			0,	"Flag that the Species should not be rotated when making a random configuration" },
	{ "NoTranslation",		0,	"Flag that the Species should not be translated when making a random configuration" },
	{ "Population",			1,	"Relative population of the Species" }
};

// Convert text string to SpeciesInfoKeyword
SpeciesInfoBlock::SpeciesInfoKeyword SpeciesInfoBlock::keyword(const char* s)
{
	for (int n=0; n<nSpeciesInfoKeywords; ++n) if (DUQSys::sameString(s,SpeciesInfoBlockData[n].name)) return (SpeciesInfoBlock::SpeciesInfoKeyword) n;
	return nSpeciesInfoKeywords;
}

// Convert SpeciesInfoKeyword to text string
const char* SpeciesInfoBlock::keyword(SpeciesInfoBlock::SpeciesInfoKeyword id)
{
	return SpeciesInfoBlockData[id].name;
}

// Return minimum number of expected arguments
int SpeciesInfoBlock::nArguments(SpeciesInfoBlock::SpeciesInfoKeyword id)
{
	return SpeciesInfoBlockData[id].nArguments;
}

// Parse SpeciesInfo block
bool SpeciesInfoBlock::parse(LineParser& parser, DUQ* duq, SpeciesInfo* speciesInfo)
{
	Messenger::print("\nParsing %s block '%s'...\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword), speciesInfo->species()->name());

	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);
		SpeciesInfoBlock::SpeciesInfoKeyword spInfoKeyword = SpeciesInfoBlock::keyword(parser.argc(0));
		if ((spInfoKeyword != SpeciesInfoBlock::nSpeciesInfoKeywords) && ((parser.nArgs()-1) < SpeciesInfoBlock::nArguments(spInfoKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", SpeciesInfoBlock::keyword(spInfoKeyword));
			error = true;
			break;
		}
		switch (spInfoKeyword)
		{
			case (SpeciesInfoBlock::EndSpeciesInfoKeyword):
				Messenger::print("Found end of %s block.\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword));
				blockDone = true;
				break;
			case (SpeciesInfoBlock::NoRotationKeyword):
				speciesInfo->setRotateOnInsertion(false);
				break;
			case (SpeciesInfoBlock::NoTranslationKeyword):
				speciesInfo->setTranslateOnInsertion(false);
				break;
			case (SpeciesInfoBlock::PopulationKeyword):
				speciesInfo->setPopulation(parser.argd(1));
				break;
			case (SpeciesBlock::nSpeciesKeywords):
				Messenger::error("Unrecognised %s block keyword found - '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword), parser.argc(0));
				InputBlocks::printValidKeywords(InputBlocks::SpeciesBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword), SpeciesInfoBlock::keyword(spInfoKeyword));
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
