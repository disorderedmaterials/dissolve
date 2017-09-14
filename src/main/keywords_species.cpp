/*
	*** Keyword Parsing - Species Block
	*** src/main/keywords_species.cpp
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
#include "classes/atomtype.h"
#include "classes/species.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Species Block Keywords
KeywordData SpeciesBlockData[] = {
	{ "Angle",			5,	"" },
	{ "Atom",			6,	"" },
	{ "AutoAddGrains",		0,	"" },
	{ "Bond",			4,	"" },
	{ "Charge",			2,	"" },
	{ "EndSpecies",			0,	"" },
	{ "Grain",			1,	"" },
	{ "Isotopologue",		1,	"" }
};

// Convert text string to SpeciesKeyword
SpeciesBlock::SpeciesKeyword SpeciesBlock::keyword(const char* s)
{
	for (int n=0; n<nSpeciesKeywords; ++n) if (DUQSys::sameString(s,SpeciesBlockData[n].name)) return (SpeciesBlock::SpeciesKeyword) n;
	return nSpeciesKeywords;
}

// Convert SpeciesKeyword to text string
const char* SpeciesBlock::keyword(SpeciesBlock::SpeciesKeyword id)
{
	return SpeciesBlockData[id].name;
}

// Return minimum number of expected arguments
int SpeciesBlock::nArguments(SpeciesBlock::SpeciesKeyword id)
{
	return SpeciesBlockData[id].nArguments;
}

// Parse Species block
bool SpeciesBlock::parse(LineParser& parser, DUQ* duq, Species* species)
{
	Messenger::print("\nParsing %s '%s'\n", InputBlocks::inputBlock(InputBlocks::SpeciesBlock), species->name());

	int el;
	CharString arg1, arg2;
	AtomType* at;
	Isotopologue* iso;
	Parameters* params;
	SpeciesAngle* a;
	SpeciesAtom* i;
	SpeciesBond* b;
	SpeciesGrain* sg;
	Isotope* tope;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);
		SpeciesBlock::SpeciesKeyword spKeyword = SpeciesBlock::keyword(parser.argc(0));
		if ((spKeyword != SpeciesBlock::nSpeciesKeywords) && ((parser.nArgs()-1) < SpeciesBlock::nArguments(spKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", SpeciesBlock::keyword(spKeyword));
			error = true;
			break;
		}
		switch (spKeyword)
		{
			case (SpeciesBlock::AngleKeyword):
				a = species->addAngle(parser.argi(1)-1, parser.argi(2)-1, parser.argi(3)-1);
				if (a)
				{
					a->setEquilibrium(parser.argd(4));
					a->setForceConstant(parser.argd(5));
				}
				else error = true;
				break;
			case (SpeciesBlock::AtomKeyword):
				el = PeriodicTable::find(parser.argc(2));
				if (el == -1)
				{
					Messenger::error("Unrecognised element symbol '%s' found in %s keyword.\n", parser.argc(2), SpeciesBlock::keyword(SpeciesBlock::AtomKeyword));
					el = 0;
					error = true;
					break;
				}
				i = species->addAtom(el, parser.argd(3), parser.argd(4), parser.argd(5));
				if (parser.hasArg(7)) i->setCharge(parser.argd(7));

				// Locate the AtomType assigned to the Atom
				at = duq->findAtomType(parser.argc(6));
				if (!at)
				{
					Messenger::print("Creating AtomType '%s'...\n", parser.argc(6));
					at = duq->addAtomType(el);
					at->setName(parser.argc(6));

					// Check to see if some named potential parameters exist, corrsponding to the AtomType name
					params = PeriodicTable::element(el).findParameters(parser.argc(6));
					if (params)
					{
						at->setParameters(params);
						Messenger::print("Initial parameters set for AtomType '%s' : sigma=%f, epsilon=%f, charge=%f.\n", parser.argc(6), params->sigma(), params->epsilon(), params->charge());
					}
					else Messenger::print("No Parameters called '%s' exist to associate to AtomType - they must be added in a PairPotentials block.\n", parser.argc(6));
				}

				// Finally, set AtomType for the Atom
				i->setAtomType(at);
				break;
			case (SpeciesBlock::AutoAddGrainsKeyword):
				species->autoAddGrains();
				break;
			case (SpeciesBlock::BondKeyword):
				b = species->addBond(parser.argi(1)-1, parser.argi(2)-1);
				if (b)
				{
					b->setEquilibrium(parser.argd(3));
					b->setForceConstant(parser.argd(4));
				}
				else error = true;
				break;
			case (SpeciesBlock::ChargeKeyword):
				i = species->atom(parser.argi(1) - 1);
				if (i) i->setCharge(parser.argd(2));
				else
				{
					Messenger::error("Specified atom index (%i) for Charge keyword is out of range.\n", parser.argi(1));
					error = true;
				}
				break;
			case (SpeciesBlock::EndSpeciesKeyword):
				species->updateGrains();
				species->centreAtOrigin();
				species->orderAtomsWithinGrains();
				species->calculateIndexLists();
				species->createScalingMatrix();
				Messenger::print("Found end of Species '%s'.\n", species->name());
				blockDone = true;
				break;
			case (SpeciesBlock::GrainKeyword):
				sg = species->addGrain();
				sg->setName(species->uniqueGrainName(parser.argc(1)));
				Messenger::print("--> Added grain definition '%s' to Species '%s'\n", sg->name(), species->name());
				for (int n=2; n<parser.nArgs(); ++n)
				{
					i = species->atom(parser.argi(n)-1);
					if (i == NULL)
					{
						Messenger::error("Failed to find atom with index %i in Species '%s'\n", parser.argi(n), species->name());
						error = true;
					}
					else species->addAtomToGrain(i, sg);
				}
				break;
			case (SpeciesBlock::IsotopologueKeyword):
				iso = species->addIsotopologue(species->uniqueIsotopologueName(parser.argc(1)));
				duq->updateIsotopologues(species, iso);
				Messenger::print("--> Added Isotopologue '%s' to Species '%s'\n", iso->name(), species->name());
				// Each parser argument is a string of the form ATOMTYPE=ISO
				for (int n=2; n<parser.nArgs(); ++n)
				{
					// Split argument into parts before and after '='
					arg1 = DUQSys::beforeChar(parser.argc(n),'=');
					arg2 = DUQSys::afterChar(parser.argc(n),'=');
					
					at = duq->findAtomType(arg1.get());
					if (at == NULL)
					{
						Messenger::error("Failed to find AtomType '%s', referred to in Isotopologue '%s', Species '%s'\n", arg1.get(), iso->name(), species->name());
						error = true;
						break;
					}

					// Is supplied isotope valid for the AtomType's element?
					el = at->element();
					tope = PeriodicTable::element(el).hasIsotope(arg2.asInteger());
					if (tope == NULL)
					{
						Messenger::error("No such Isotope (%i) for element %s (AtomType '%s') in Isotopologue '%s', Species '%s'\n", arg2.asInteger(), PeriodicTable::element(el).symbol(), at->name(), iso->name(), species->name());
						error = true;
						break;
					}
					
					// Assign isotope to AtomType
					if (!iso->setAtomTypeIsotope(at, tope))
					{
						error = true;
						break;
					}
				}
				break;
			case (SpeciesBlock::nSpeciesKeywords):
				Messenger::error("Unrecognised %s block keyword found - '%s'\n", InputBlocks::inputBlock(InputBlocks::SpeciesBlock), parser.argc(0));
				InputBlocks::printValidKeywords(InputBlocks::SpeciesBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", InputBlocks::inputBlock(InputBlocks::SpeciesBlock), SpeciesBlock::keyword(spKeyword));
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
