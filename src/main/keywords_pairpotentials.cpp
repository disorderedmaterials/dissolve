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
PairPotentialsBlock::PairPotentialsKeyword PairPotentialsBlock::keyword(const char* s)
{
	for (int n=0; n<PairPotentialsBlock::nPairPotentialsKeywords; ++n) if (DUQSys::sameString(s,PairPotentialsBlockData[n].name)) return (PairPotentialsBlock::PairPotentialsKeyword) n;
	return PairPotentialsBlock::nPairPotentialsKeywords;
}

// Convert PairPotentialsKeyword to text string
const char* PairPotentialsBlock::keyword(PairPotentialsBlock::PairPotentialsKeyword id)
{
	return PairPotentialsBlockData[id].name;
}

// Return minimum number of expected arguments
int PairPotentialsBlock::nArguments(PairPotentialsBlock::PairPotentialsKeyword id)
{
	return PairPotentialsBlockData[id].nArguments;
}
// Parse PairPotentials block
bool PairPotentialsBlock::parse(LineParser& parser, DUQ* duq)
{
	Messenger::print("\nParsing %s block...\n", InputBlocks::inputBlock(InputBlocks::PairPotentialsBlock));

	AtomType* at, *at2;
	PairPotential* pot;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank(duq->worldPool()))
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(duq->worldPool(), LineParser::SkipBlanks+LineParser::UseQuotes);
		PairPotentialsBlock::PairPotentialsKeyword ppKeyword = PairPotentialsBlock::keyword(parser.argc(0));
		if ((ppKeyword != PairPotentialsBlock::nPairPotentialsKeywords) && ((parser.nArgs()-1) < PairPotentialsBlock::nArguments(ppKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", PairPotentialsBlock::keyword(ppKeyword));
			error = true;
			break;
		}
		switch (ppKeyword)
		{
			case (PairPotentialsBlock::CoulombKeyword):
			case (PairPotentialsBlock::DispersionKeyword):
			case (PairPotentialsBlock::FullKeyword):
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
				if (ppKeyword == PairPotentialsBlock::CoulombKeyword)
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
				else if (ppKeyword == PairPotentialsBlock::DispersionKeyword)
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
			case (PairPotentialsBlock::DeltaKeyword):
				duq->setPairPotentialDelta(parser.argd(1));
				break;
			case (PairPotentialsBlock::EndPairPotentialsKeyword):
				Messenger::print("Found end of %s block.\n", InputBlocks::inputBlock(InputBlocks::PairPotentialsBlock));
				blockDone = true;
				break;
			case (PairPotentialsBlock::RangeKeyword):
				duq->setPairPotentialRange(parser.argd(1));
				break;
			case (PairPotentialsBlock::TruncationWidthKeyword):
				duq->setPairPotentialTruncationWidth(parser.argd(1));
				break;
			case (PairPotentialsBlock::nPairPotentialsKeywords):
				Messenger::error("Unrecognised %s block keyword found - '%s'\n", InputBlocks::inputBlock(InputBlocks::PairPotentialsBlock), parser.argc(0));
				InputBlocks::printValidKeywords(InputBlocks::PairPotentialsBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", InputBlocks::inputBlock(InputBlocks::PairPotentialsBlock), PairPotentialsBlock::keyword(ppKeyword));
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
