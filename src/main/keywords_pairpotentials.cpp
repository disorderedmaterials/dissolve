/*
	*** Keyword Parsing - PairPotentials Block
	*** src/main/keywords_pairpotentials.cpp
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
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "base/lineparser.h"

// Return enum option info for PairPotentialsKeyword
EnumOptions<PairPotentialsBlock::PairPotentialsKeyword> PairPotentialsBlock::keywords()
{
	static EnumOptionsList PairPotentialsKeywords = EnumOptionsList() <<
		EnumOption(PairPotentialsBlock::CoulombTruncationKeyword, 		"CoulombTruncation",		1) <<
		EnumOption(PairPotentialsBlock::DeltaKeyword, 				"Delta",			1) <<
		EnumOption(PairPotentialsBlock::EndPairPotentialsKeyword,		"EndPairPotentials") <<
		EnumOption(PairPotentialsBlock::GenerateKeyword, 			"Generate",			3,9) <<
		EnumOption(PairPotentialsBlock::GenerateAllKeyword, 			"GenerateAll",			1) <<
		EnumOption(PairPotentialsBlock::IncludeCoulombKeyword, 			"IncludeCoulomb",		1) <<
		EnumOption(PairPotentialsBlock::ParametersKeyword, 			"Parameters",			4,6) <<
		EnumOption(PairPotentialsBlock::RangeKeyword, 				"Range",			1) <<
		EnumOption(PairPotentialsBlock::ShortRangeTruncationKeyword, 		"ShortRangeTruncation",		1) <<
		EnumOption(PairPotentialsBlock::ShortRangeTruncationWidthKeyword, 	"ShortRangeTruncationWidth",	1);

	static EnumOptions<PairPotentialsBlock::PairPotentialsKeyword> options("PairPotentialsKeyword", PairPotentialsKeywords);

	return options;
}

// Parse PairPotentials block
bool PairPotentialsBlock::parse(LineParser& parser, Dissolve* dissolve)
{
	Messenger::print("\nParsing %s block...\n", BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword));

	AtomType* at1, *at2;
	PairPotential* pot;
	PairPotential::ShortRangeType srType;
	PairPotential::CoulombTruncationScheme cTrunc;
	PairPotential::ShortRangeTruncationScheme srTrunc;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		if (parser.getArgsDelim() != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!keywords().isValid(parser.argc(0))) return keywords().errorAndPrintValid(parser.argc(0));
		PairPotentialsKeyword kwd = keywords().enumeration(parser.argc(0));
		if (!keywords().validNArgs(kwd, parser.nArgs()-1)) return false;

		// All OK, so process the keyword
		switch (kwd)
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
				Messenger::print("Found end of %s block.\n", BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword));
				blockDone = true;
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
					Messenger::printVerbose("Adding PairPotential for interaction between '%s' and '%s'...\n", parser.argc(2), parser.argc(3));
					pot = dissolve->addPairPotential(at1, at2);
				}

				// Now set the short-range type
				pot->setShortRangeType(srType);

				// If more parameters were supplied, these will for the basis for the potential. Otherwise, generate from existing AtomType parameters.
				if (parser.nArgs() == 4)
				{
					Messenger::printVerbose("No parameters supplied to Generate keyword for pair potential between '%s' and '%s', so AtomType parameters will be used.", at1->name(), at2->name());
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
					Messenger::printVerbose("Pair potential between '%s' and '%s' will be generated from parameters : %s\n", at1->name(), at2->name(), s.get());
				}
				break;
			case (PairPotentialsBlock::GenerateAllKeyword):
				Messenger::printVerbose("Generating all missing pair potentials...\n");

				// Get short-range type
				srType = PairPotential::shortRangeType(parser.argc(1));
				if (srType == PairPotential::nShortRangeTypes)
				{
					Messenger::error("Unknown short-range interaction type '%s' used in PairPotentials block.\n", parser.argc(1));
					error = true;
					break;
				}

				if (!dissolve->generateMissingPairPotentials(srType)) error = true;
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
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword), keywords().keyword(kwd));
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
		Messenger::error("Unterminated %s block found.\n", BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword));
		error = true;
	}

	return (!error);
}
