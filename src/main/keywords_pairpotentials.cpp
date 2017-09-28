/*
	*** Keyword Parsing - PairPotentials Block
	*** src/main/keywords_pairpotentials.cpp
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
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "base/lineparser.h"

// PairPotentials Block Keywords
KeywordData PairPotentialsBlockData[] = {
	{ "CoulombTruncation",		1,	"Truncation scheme to apply to Coulomb potential" },
	{ "Delta",			1,	"Delta (in r-squared) of tabulated potential" },
	{ "EndPairPotentials",		0,	"Signals the end of the PairPotentials block" },
	{ "Generate",			3,	"Generate a pair potential of the specified form, according to internal parameters (or those supplied with 'Parameters')" },
	{ "GenerateAll",		1,	"Generate all (remaining) potentials according to internal atomtype parameters (or those supplied with 'Parameters')" },
	{ "IncludeCoulomb",		1,	"Include Coulomb term in tabulated pair potentials" },
	{ "Parameters",			4,	"Set the atomic charge and short-range interaction parameters for the named atomtype" },
	{ "Range",			1,	"Maximal range of the pair potential (in r)" },
	{ "TruncationWidth",		1,	"Width of the truncation region applied to the short-range potential" }
};

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

	AtomType* at1, *at2;
	PairPotential* pot;
	PairPotential::ShortRangeType srType;
	PairPotential::TruncationScheme cTrunc = PairPotential::ShiftedTruncation;
	Parameters* params;
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
				cTrunc = PairPotential::truncationScheme(parser.argc(1));
				if (cTrunc == PairPotential::nTruncationSchemes)
				{
					Messenger::error("Unknown short-range interaction type '%s' used in PairPotentials block.\n", parser.argc(1));
					error = true;
					break;
				}
				PairPotential::setCoulombTruncationScheme(cTrunc);
				break;
			case (PairPotentialsBlock::DeltaKeyword):
				duq->setPairPotentialDelta(parser.argd(1));
				break;
			case (PairPotentialsBlock::EndPairPotentialsKeyword):
				Messenger::print("Found end of %s block.\n", InputBlocks::inputBlock(InputBlocks::PairPotentialsBlock));
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
				at1 = duq->findAtomType(parser.argc(2));
				at2 = duq->findAtomType(parser.argc(3));
				if ((!at1) || (!at2))
				{
					Messenger::error("Unknown AtomType '%s' referenced in PairPotentials block.\n", at1 ? parser.argc(3) : parser.argc(2));
					error = true;
					break;
				}
				pot = duq->hasPairPotential(at1, at2);
				if (pot) Messenger::warn("Overwriting previous PairPotential parameters for interaction between '%s' and '%s'...\n", parser.argc(2), parser.argc(3));
				else
				{
					Messenger::print("Adding PairPotential for interaction between '%s' and '%s'...\n", parser.argc(2), parser.argc(3));
					pot = duq->addPairPotential(at1, at2);
				}

				// Check Parameters for AtomTypes
				if ((!at1->parameters()) || (!at2->parameters()))
				{
					Messenger::error("Parameters for AtomType '%s' don't yet exist, so can't setup a PairPotential.\n", at1->parameters() ? parser.argc(3) : parser.argc(2));
					error = true;
					break;
				}

				pot->setShortRangeType(srType);
				if (!pot->setParameters(at1, at2)) error = true;
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

				// Loop over all atomtype pairs and generate any missing potentials
				for (at1 = duq->atomTypes(); at1 != NULL; at1 = at1->next)
				{
					for (at2 = at1; at2 != NULL; at2 = at2->next)
					{
						pot = duq->hasPairPotential(at1, at2);
						if (pot)
						{
							Messenger::print("PairPotential already exists for interaction between '%s' and '%s'...\n", at1->name(), at2->name());
							continue;
						}
						else
						{
							Messenger::print("Adding PairPotential for interaction between '%s' and '%s'...\n", at1->name(), at2->name());
							pot = duq->addPairPotential(at1, at2);
						}

						// Check Parameters for AtomTypes
						if ((!at1->parameters()) || (!at2->parameters()))
						{
							Messenger::error("Parameters for AtomType '%s' don't exist, so can't setup a PairPotential.\n", at1->parameters() ? at2->name() : at1->name());
							error = true;
							break;
						}

						pot->setShortRangeType(srType);
						pot->setParameters(at1, at2);
					}
					if (error) break;
				}
				break;
			case (PairPotentialsBlock::IncludeCoulombKeyword):
				duq->setPairPotentialsIncludeCoulomb(parser.argb(1));
				break;
			case (PairPotentialsBlock::ParametersKeyword):
				// Find AtomType...
				at1 = duq->findAtomType(parser.argc(1));
				if (!at1)
				{
					Messenger::error("Unknown AtomType '%s' referenced in PairPotentials block.\n", parser.argc(1));
					error = true;
					break;
				}

				// Do we need to create some new parameters for the AtomType?
				if (!at1->parameters())
				{
					params = PeriodicTable::element(at1->element()).addParameters();
					params->setName(at1->name());
					at1->setParameters(params);
				}

				// Get Parameters pointer and set values
				params = at1->parameters();
				params->setCharge(parser.argd(2));
				for (int n=3; n<parser.nArgs(); ++n) params->setParameter(n-3, parser.argd(n));
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
