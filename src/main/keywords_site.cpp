/*
	*** Keyword Parsing - Site Block
	*** src/main/keywords_site.cpp
	Copyright T. Youngs 2012-2018

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
#include "classes/speciessite.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Site Block Keywords
KeywordData SiteBlockData[] = {
	{ "EndSite",			0,	"Signals the end of the Site" },
	{ "Origin",			1,	"Set the atom indices whose average coordinates reflect the site origin" },
	{ "OriginMassWeighted",		1,	"Control whether the origin should be calculated with mass-weighted coordinates" },
	{ "XAxis",			1,	"Define one or more atoms whose average coordinates reflect the direction of the x axis" },
	{ "YAxis",			1,	"Define one or more atoms whose average coordinates reflect the direction of the y axis" }
};

// Convert text string to SiteKeyword
SiteBlock::SiteKeyword SiteBlock::keyword(const char* s)
{
	for (int n=0; n<nSiteKeywords; ++n) if (DissolveSys::sameString(s,SiteBlockData[n].name)) return (SiteBlock::SiteKeyword) n;
	return nSiteKeywords;
}

// Convert SiteKeyword to text string
const char* SiteBlock::keyword(SiteBlock::SiteKeyword id)
{
	return SiteBlockData[id].name;
}

// Return minimum number of expected arguments
int SiteBlock::nArguments(SiteBlock::SiteKeyword id)
{
	return SiteBlockData[id].nArguments;
}

// Parse Site block
bool SiteBlock::parse(LineParser& parser, Dissolve* dissolve, SpeciesSite* site)
{
	Messenger::print("\nParsing %s block '%s'...\n", BlockKeywords::blockKeyword(BlockKeywords::SiteBlockKeyword), site->parent()->name());

	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);
		SiteBlock::SiteKeyword siteKeyword = SiteBlock::keyword(parser.argc(0));
		if ((siteKeyword != SiteBlock::nSiteKeywords) && ((parser.nArgs()-1) < SiteBlock::nArguments(siteKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", SiteBlock::keyword(siteKeyword));
			error = true;
			break;
		}
		switch (siteKeyword)
		{
			case (SiteBlock::EndSiteKeyword):
				Messenger::print("Found end of %s block.\n", BlockKeywords::blockKeyword(BlockKeywords::SiteBlockKeyword));
				blockDone = true;
				break;
			case (SiteBlock::OriginKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					if (!site->addOriginAtom(parser.argi(n) - 1))
					{
						Messenger::error("Failed to add origin atom for site '%s'.\n", site->name());
						error = true;
						break;
					}
				}
				break;
			case (SiteBlock::OriginMassWeightedKeyword):
				site->setOriginMassWeighted(parser.argb(1));
				break;
			case (SiteBlock::nSiteKeywords):
				Messenger::error("Unrecognised %s block keyword '%s' found.\n", BlockKeywords::blockKeyword(BlockKeywords::SiteBlockKeyword), parser.argc(0));
				BlockKeywords::printValidKeywords(BlockKeywords::SiteBlockKeyword);
				error = true;
				break;
			case (SiteBlock::XAxisKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					if (!site->addXAxisAtom(parser.argi(n) - 1))
					{
						Messenger::error("Failed to add x-axis atom for site '%s'.\n", site->name());
						error = true;
						break;
					}
				}
				break;
			case (SiteBlock::YAxisKeyword):
				for (int n=1; n<parser.nArgs(); ++n)
				{
					if (!site->addYAxisAtom(parser.argi(n) - 1))
					{
						Messenger::error("Failed to add y-axis atom for site '%s'.\n", site->name());
						error = true;
						break;
					}
				}
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::blockKeyword(BlockKeywords::SiteBlockKeyword), SiteBlock::keyword(siteKeyword));
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
