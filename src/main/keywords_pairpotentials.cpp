/*
	*** Keyword Parsing - PairPotentials Block
	*** src/main/keywords_pairpotentials.cpp
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
#include "base/sysfunc.h"

// PairPotentials Block Keywords
KeywordData PairPotentialsBlockData[] = {
	{ "Coulomb",			4,	"" },
	{ "Delta",			1,	"" },
	{ "Dispersion",			4,	"" },
	{ "EndPairPotentials",		0,	"" },
	{ "Full",			6,	"" },
	{ "Range",			1,	"" },
	{ "TruncationWidth",		1,	"" }
};

// PairPotentials Block NArguments
int PairPotentialsBlockNArguments[] = { 4, 1, 4, 0, 6, 1, 1 };

// Convert text string to PairPotentialsKeyword
Keywords::PairPotentialsKeyword Keywords::pairPotentialsKeyword(const char* s)
{
	for (int n=0; n<Keywords::nPairPotentialsKeywords; ++n) if (DUQSys::sameString(s,PairPotentialsBlockData[n].name)) return (Keywords::PairPotentialsKeyword) n;
	return Keywords::nPairPotentialsKeywords;
}

// Convert PairPotentialsKeyword to text string
const char* Keywords::pairPotentialsKeyword(Keywords::PairPotentialsKeyword id)
{
	return PairPotentialsBlockData[id].name;
}

// Return minimum number of expected arguments
int Keywords::pairPotentialsBlockNArguments(Keywords::PairPotentialsKeyword id)
{
	return PairPotentialsBlockData[id].nArguments;
}
// Parse PairPotentials block
bool Keywords::parsePairPotentialsBlock(LineParser& parser, DUQ* duq)
{
	Messenger::print("Parsing %s block\n", Keywords::inputBlock(Keywords::PairPotentialsBlock));

	AtomType* at, *at2;
	PairPotential* pot;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
		Keywords::PairPotentialsKeyword ppKeyword = Keywords::pairPotentialsKeyword(parser.argc(0));
		if ((ppKeyword != Keywords::nPairPotentialsKeywords) && ((parser.nArgs()-1) < Keywords::pairPotentialsBlockNArguments(ppKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", Keywords::pairPotentialsKeyword(ppKeyword));
			error = true;
			break;
		}
		switch (ppKeyword)
		{
			case (Keywords::CoulombKeyword):
			case (Keywords::DispersionKeyword):
			case (Keywords::FullKeyword):
				// Grab AtomTypes first...
				at = duq->findAtomType(parser.argc(1));
				at2 = duq->findAtomType(parser.argc(2));
				if ((!at) || (!at2))
				{
					Messenger::error("Eror: AtomType '%s' used in PairPotential has not been defined.\n", at ? parser.argc(2) : parser.argc(1));
					error = true;
					break;
				}
				pot = duq->addPairPotential();
				pot->setParameters(at, at2);
				// Pair potential now contains values from the Parameters associated with the AtomTypes, so we will overwrite them...
				if (ppKeyword == Keywords::CoulombKeyword)
				{
					if (!parser.hasArg(4))
					{
						Messenger::error("Not enough charge parameters supplied in PairPotential definition.\n");
						error = true;
						break;
					}
					pot->setType(PairPotential::CoulombType);
					pot->setChargeI(parser.argd(3));
					pot->setChargeJ(parser.argd(4));
				}
				else if (ppKeyword == Keywords::DispersionKeyword)
				{
					if (!parser.hasArg(4))
					{
						Messenger::error("Not enough LJ parameters supplied in PairPotential definition.\n");
						error = true;
						break;
					}
					pot->setType(PairPotential::DispersionType);
					pot->setSigmaIJ(parser.argd(3));
					pot->setEpsilonIJ(parser.argd(4));
				}
				else
				{
					if (!parser.hasArg(6))
					{
						Messenger::error("Not enough LJ/charge parameters supplied in PairPotential definition.\n");
						error = true;
						break;
					}
					pot->setType(PairPotential::FullType);
					pot->setSigmaIJ(parser.argd(3));
					pot->setEpsilonIJ(parser.argd(4));
					pot->setChargeI(parser.argd(5));
					pot->setChargeJ(parser.argd(6));
				}
				break;
			case (Keywords::DeltaKeyword):
				duq->setPairPotentialDelta(parser.argd(1));
				break;
			case (Keywords::EndPairPotentialsKeyword):
				Messenger::print("Found end of %s block.\n", Keywords::inputBlock(Keywords::PairPotentialsBlock));
				blockDone = true;
				break;
			case (Keywords::RangeKeyword):
				duq->setPairPotentialRange(parser.argd(1));
				break;
			case (Keywords::TruncationWidthKeyword):
				duq->setPairPotentialTruncationWidth(parser.argd(1));
				break;
			case (Keywords::nPairPotentialsKeywords):
				Messenger::error("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(Keywords::PairPotentialsBlock), parser.argc(0));
				Keywords::printValidKeywords(Keywords::PairPotentialsBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(Keywords::PairPotentialsBlock), Keywords::pairPotentialsKeyword(ppKeyword));
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
