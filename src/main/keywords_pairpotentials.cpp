/*
	*** Keyword Parsing - PairPotentials Block
	*** src/main/keywords_pairpotentials.cpp
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
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "base/lineparser.h"

// PairPotentials Block Keywords
KeywordData PairPotentialsBlockData[] = {
	{ "CoulombTruncation",		1,	"Truncation scheme to apply to Coulomb potential" },
	{ "Delta",			1,	"Delta (in r-squared) of tabulated potential" },
	{ "EndPairPotentials",		0,	"Signals the end of the PairPotentials block" },
	{ "Exchangeable",		1,	"Set AtomTypes that exchange with others" },
	{ "Generate",			3,	"Generate a pair potential of the specified form and with the supplied parameters" },
	{ "GenerateAll",		1,	"Generate all (remaining) potentials according to current AtomType parameters" },
	{ "IncludeCoulomb",		1,	"Include Coulomb term in tabulated pair potentials" },
	{ "Parameters",			4,	"Set the atomic charge and short-range interaction parameters for the named atomtype" },
	{ "Range",			1,	"Maximal range of the pair potential (in r)" },
	{ "ShortRangeTruncation",	1,	"Truncation scheme to apply to short-range potential" },
	{ "ShortRangeTruncationWidth",	1,	"Width of the truncation region applied to the short-range potential (Cosine truncation only)" }
};

// Convert text string to PairPotentialsKeyword
PairPotentialsBlock::PairPotentialsKeyword PairPotentialsBlock::keyword(const char* s)
{
	for (int n=0; n<PairPotentialsBlock::nPairPotentialsKeywords; ++n) if (DissolveSys::sameString(s,PairPotentialsBlockData[n].name)) return (PairPotentialsBlock::PairPotentialsKeyword) n;
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
bool PairPotentialsBlock::parse(LineParser& parser, Dissolve* dissolve)
{
	Messenger::print("\nParsing %s block...\n", InputBlocks::inputBlock(InputBlocks::PairPotentialsBlock));

	AtomType* at1, *at2;
	PairPotential* pot;
	PairPotential::ShortRangeType srType;
	PairPotential::CoulombTruncationScheme cTrunc;
	PairPotential::ShortRangeTruncationScheme srTrunc;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);
		PairPotentialsBlock::PairPotentialsKeyword ppKeyword = PairPotentialsBlock::keyword(parser.argc(0));
		if ((ppKeyword != PairPotentialsBlock::nPairPotentialsKeywords) && ((parser.nArgs()-1) < PairPotentialsBlock::nArguments(ppKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", PairPotentialsBlock::keyword(ppKeyword));
			error = true;
			break;
		}
		switch (ppKeyword)
		{
			case (PairPotentialsBlock::CoulombTruncationKeyword):
				cTrunc = PairPotential::coulombTruncationScheme(parser.argc(1));
				if (cTrunc == PairPotential::nCoulombTruncationSchemes)
				{
					Messenger::error("Unknown Coulomb truncation scheme '%s' used in PairPotentials block.\n", parser.argc(1));
					error = true;
					break;
				}
				PairPotential::setCoulombTruncationScheme(cTrunc);
				break;
			case (PairPotentialsBlock::DeltaKeyword):
				dissolve->setPairPotentialDelta(parser.argd(1));
				break;
			case (PairPotentialsBlock::EndPairPotentialsKeyword):
				Messenger::print("Found end of %s block.\n", InputBlocks::inputBlock(InputBlocks::PairPotentialsBlock));
				blockDone = true;
				break;
			case (PairPotentialsBlock::ExchangeableKeyword):
				// Loop over all provided arguments (which are atom type names) and set flags in those atom types
				for (int n=1; n<parser.nArgs(); ++n)
				{
					AtomType* atomType = dissolve->findAtomType(parser.argc(n));
					if (!atomType)
					{
						Messenger::error("Unrecognised AtomType '%s' given to Exchangeable keyword.\n", parser.argc(n));
						return false;
					}
					atomType->setExchangeable(true);
				}
				break;
			case (PairPotentialsBlock::GenerateKeyword):
				// Get short-range type
				srType = PairPotential::shortRangeType(parser.argc(1));
				if (srType == PairPotential::nShortRangeTypes)
				{
					Messenger::error("Unknown short-range interaction type '%s' used in PairPotentials block.\n", parser.argc(1));
					error = true;
					break;
				}

				// Grab AtomTypes...
				at1 = dissolve->findAtomType(parser.argc(2));
				at2 = dissolve->findAtomType(parser.argc(3));
				if ((!at1) || (!at2))
				{
					Messenger::error("Unknown AtomType '%s' referenced in PairPotentials block.\n", at1 ? parser.argc(3) : parser.argc(2));
					error = true;
					break;
				}
				pot = dissolve->pairPotential(at1, at2);
				if (pot) Messenger::warn("Overwriting previous PairPotential parameters for interaction between '%s' and '%s'...\n", parser.argc(2), parser.argc(3));
				else
				{
					Messenger::print("Adding PairPotential for interaction between '%s' and '%s'...\n", parser.argc(2), parser.argc(3));
					pot = dissolve->addPairPotential(at1, at2);
				}

				// Now set the short-range type
				pot->setShortRangeType(srType);

				// If more parameters were supplied, these will for the basis for the potential. Otherwise, generate from existing AtomType parameters.
				if (parser.nArgs() == 4)
				{
					Messenger::print("No parameters supplied to Generate keyword for pair potential between '%s' and '%s', so AtomType parameters will be used.", at1->name(), at2->name());
					if (!pot->setParameters(at1, at2)) error = true;
				}
				else
				{
					// Read in the parameters - qi, qj, p1, p2, ...
					if (parser.hasArg(4)) pot->setChargeI(parser.argd(4));
					if (parser.hasArg(5)) pot->setChargeJ(parser.argd(5));
					for (int n=6; n<parser.nArgs(); ++n) pot->setParameter(n-6, parser.argd(n));
					CharString s("q(I)=%f, q(J)=%f", pot->chargeI(), pot->chargeJ());
					for (int n=0; n<MAXSRPARAMETERS; ++n) s.strcatf(", p%i=%f", n+1, pot->parameter(n));
					Messenger::print("Pair potential between '%s' and '%s' will be generated from parameters : %s\n", at1->name(), at2->name(), s.get());
				}
				break;
			case (PairPotentialsBlock::GenerateAllKeyword):
				Messenger::print("Generating all missing pair potentials...\n");

				// Get short-range type
				srType = PairPotential::shortRangeType(parser.argc(1));
				if (srType == PairPotential::nShortRangeTypes)
				{
					Messenger::error("Unknown short-range interaction type '%s' used in PairPotentials block.\n", parser.argc(1));
					error = true;
					break;
				}

				dissolve->generateMissingPairPotentials(srType);
				break;
			case (PairPotentialsBlock::IncludeCoulombKeyword):
				dissolve->setPairPotentialsIncludeCoulomb(parser.argb(1));
				break;
			case (PairPotentialsBlock::ParametersKeyword):
				// Find AtomType...
				at1 = dissolve->findAtomType(parser.argc(1));
				if (!at1)
				{
					Messenger::error("Unknown AtomType '%s' referenced in PairPotentials block.\n", parser.argc(1));
					error = true;
					break;
				}

				// Set parameter values
				at1->parameters().setCharge(parser.argd(2));
				for (int n=3; n<parser.nArgs(); ++n) at1->parameters().setParameter(n-3, parser.argd(n));
				break;
			case (PairPotentialsBlock::RangeKeyword):
				dissolve->setPairPotentialRange(parser.argd(1));
				break;
			case (PairPotentialsBlock::ShortRangeTruncationKeyword):
				srTrunc = PairPotential::shortRangeTruncationScheme(parser.argc(1));
				if (srTrunc == PairPotential::nShortRangeTruncationSchemes)
				{
					Messenger::error("Unknown short-range truncation scheme '%s' used in PairPotentials block.\n", parser.argc(1));
					error = true;
					break;
				}
				PairPotential::setShortRangeTruncationScheme(srTrunc);
				break;
			case (PairPotentialsBlock::ShortRangeTruncationWidthKeyword):
				PairPotential::setShortRangeTruncationWidth(parser.argd(1));
				break;
			case (PairPotentialsBlock::nPairPotentialsKeywords):
				Messenger::error("Unrecognised %s block keyword '%s' found.\n", InputBlocks::inputBlock(InputBlocks::PairPotentialsBlock), parser.argc(0));
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
