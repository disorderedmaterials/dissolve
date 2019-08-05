/*
	*** Keyword Parsing - Site Block
	*** src/main/keywords_site.cpp
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
#include "classes/speciessite.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Return enum option info for SiteKeyword
EnumOptions<SiteBlock::SiteKeyword> SiteBlock::keywords()
{
	static EnumOptionsList SiteKeywords = EnumOptionsList() <<
		EnumOption(SiteBlock::EndSiteKeyword, 			"EndSite") <<
		EnumOption(SiteBlock::OriginKeyword, 			"Origin",		EnumOption::OneOrMoreArguments) <<
		EnumOption(SiteBlock::OriginMassWeightedKeyword,	"OriginMassWeighted",	1) <<
		EnumOption(SiteBlock::XAxisKeyword,			"XAxis",		EnumOption::OneOrMoreArguments) <<
		EnumOption(SiteBlock::YAxisKeyword, 			"YAxis",		EnumOption::OneOrMoreArguments);

	static EnumOptions<SiteBlock::SiteKeyword> options("SiteKeyword", SiteKeywords);

	return options;
}

// Parse Site block
bool SiteBlock::parse(LineParser& parser, Dissolve* dissolve, SpeciesSite* site)
{
	Messenger::print("\nParsing %s block '%s'...\n", BlockKeywords::keywords().keyword(BlockKeywords::SiteBlockKeyword), site->parent()->name());

	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		if (parser.getArgsDelim() != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!keywords().isValid(parser.argc(0))) return keywords().errorAndPrintValid(parser.argc(0));
		SiteKeyword kwd = keywords().enumeration(parser.argc(0));
		if (!keywords().validNArgs(kwd, parser.nArgs()-1)) return false;

		// All OK, so process the keyword
		switch (kwd)
		{
			case (SiteBlock::EndSiteKeyword):
				Messenger::print("Found end of %s block.\n", BlockKeywords::keywords().keyword(BlockKeywords::SiteBlockKeyword));
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
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::keywords().keyword(BlockKeywords::SiteBlockKeyword), keywords().keyword(kwd));
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
		Messenger::error("Unterminated %s block found.\n", BlockKeywords::keywords().keyword(BlockKeywords::SiteBlockKeyword));
		error = true;
	}

	return (!error);
}
