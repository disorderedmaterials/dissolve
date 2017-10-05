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
	{ "Angle",			4,	"Define an angle interaction within the Species" },
	{ "Atom",			6,	"" },
	{ "AutoAddGrains",		0,	"" },
	{ "Bond",			3,	"Define a bond interaction within the Species" },
	{ "Charge",			2,	"" },
	{ "EndSpecies",			0,	"" },
	{ "Grain",			1,	"" },
	{ "Isotopologue",		1,	"" },
	{ "MasterBond",			2,	"Define master Bond parameters that can be referred to" },
	{ "MasterAngle",		3,	"Define master Angle parameters that can be referred to" },
	{ "MasterTorsion",		4,	"Define master Torsion parameters that can be referred to" },
	{ "Torsion",			5,	"Define a torsion interaction within the Species" }
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
	SpeciesTorsion* t;
	SpeciesGrain* sg;
	SpeciesBond::BondFunction bf;
	SpeciesAngle::AngleFunction af;
	SpeciesTorsion::TorsionFunction tf;
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
				// Check the functional form specified
				af = SpeciesAngle::angleFunction(parser.argc(1));
				if (af == SpeciesAngle::nAngleFunctions)
				{
					Messenger::error("Functional form of angle (%s) not recognised.\n", parser.argc(1));
					error = true;
					break;
				}
				// Create a new angle definition
				a = species->addAngle(parser.argi(2)-1, parser.argi(3)-1, parser.argi(4)-1);
				if (a)
				{
					a->setForm(af);
					for (int n=0; n<SpeciesAngle::nFunctionParameters(af); ++n)
					{
						if (!parser.hasArg(n+5))
						{
							Messenger::error("Angle function type '%s' requires %i parameters\n", SpeciesAngle::angleFunction(af), SpeciesAngle::nFunctionParameters(af));
							error = true;
							break;
						}
						a->setParameter(n, parser.argd(n+5));
					}
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
						Messenger::print("Initial parameters set for AtomType '%s' : charge=%f, param1=%f, param2=%f, param3=%f, param4=%f.\n", parser.argc(6), params->charge(), params->parameter(0), params->parameter(1), params->parameter(2), params->parameter(3));
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
				// Check the functional form specified
				bf = SpeciesBond::bondFunction(parser.argc(1));
				if (bf == SpeciesBond::nBondFunctions)
				{
					Messenger::error("Functional form of bond (%s) not recognised.\n", parser.argc(1));
					error = true;
					break;
				}
				// Create a new bond definition
				b = species->addBond(parser.argi(2)-1, parser.argi(3)-1);
				if (b)
				{
					b->setForm(bf);
					for (int n=0; n<SpeciesBond::nFunctionParameters(bf); ++n)
					{
						if (!parser.hasArg(4+n))
						{
							Messenger::error("Bond function type '%s' requires %i parameters\n", SpeciesBond::bondFunction(bf), SpeciesBond::nFunctionParameters(bf));
							error = true;
							break;
						}
						b->setParameter(n, parser.argd(n+4));
					}
				}
				else error = true;
				break;
			case (SpeciesBlock::ChargeKeyword):
				i = species->atom(parser.argi(1) - 1);
				if (i) i->setCharge(parser.argd(2));
				else
				{
					Messenger::error("Specified Atom index (%i) for Charge keyword is out of range.\n", parser.argi(1));
					error = true;
				}
				break;
			case (SpeciesBlock::EndSpeciesKeyword):
				species->updateGrains();
				species->centreAtOrigin();
				species->orderAtomsWithinGrains();
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
						Messenger::error("Failed to find Atom with index %i in Species '%s'\n", parser.argi(n), species->name());
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
			case (SpeciesBlock::MasterAngleKeyword):
				// Check the functional form specified
				af = SpeciesAngle::angleFunction(parser.argc(2));
				if (af == SpeciesAngle::nAngleFunctions)
				{
					Messenger::error("Functional form of angle (%s) not recognised.\n", parser.argc(1));
					error = true;
					break;
				}
				// Create a new master angle definition
				a = species->addMasterAngle(parser.argc(1));
				if (a)
				{
					a->setName(parser.argc(1));
					a->setForm(af);
					for (int n=0; n<SpeciesAngle::nFunctionParameters(af); ++n)
					{
						if (!parser.hasArg(n+3))
						{
							Messenger::error("Angle function type '%s' requires %i parameters\n", SpeciesAngle::angleFunction(af), SpeciesAngle::nFunctionParameters(af));
							error = true;
							break;
						}
						a->setParameter(n, parser.argd(n+3));
					}
				}
				else error = true;
				break;
			case (SpeciesBlock::MasterBondKeyword):
				// Check the functional form specified
				bf = SpeciesBond::bondFunction(parser.argc(2));
				if (bf == SpeciesBond::nBondFunctions)
				{
					Messenger::error("Functional form of bond (%s) not recognised.\n", parser.argc(1));
					error = true;
					break;
				}
				// Create a new master bond definition
				b = species->addMasterBond(parser.argc(1));
				if (b)
				{
					b->setName(parser.argc(1));
					b->setForm(bf);
					for (int n=0; n<SpeciesBond::nFunctionParameters(bf); ++n)
					{
						if (!parser.hasArg(n+3))
						{
							Messenger::error("Bond function type '%s' requires %i parameters\n", SpeciesBond::bondFunction(bf), SpeciesBond::nFunctionParameters(bf));
							error = true;
							break;
						}
						b->setParameter(n, parser.argd(n+3));
					}
				}
				else error = true;
				break;
			case (SpeciesBlock::MasterTorsionKeyword):
				// Check the functional form specified
				tf = SpeciesTorsion::torsionFunction(parser.argc(2));
				if (tf == SpeciesTorsion::nTorsionFunctions)
				{
					Messenger::error("Functional form of torsion (%s) not recognised.\n", parser.argc(1));
					error = true;
					break;
				}
				// Create a new master torsion definition
				t = species->addMasterTorsion(parser.argc(1));
				if (t)
				{
					t->setName(parser.argc(1));
					t->setForm(tf);
					for (int n=0; n<SpeciesTorsion::nFunctionParameters(tf); ++n)
					{
						if (!parser.hasArg(n+3))
						{
							Messenger::error("Torsion function type '%s' requires %i parameters\n", SpeciesTorsion::torsionFunction(tf), SpeciesTorsion::nFunctionParameters(tf));
							error = true;
							break;
						}
						t->setParameter(n, parser.argd(n+3));
					}
				}
				else error = true;
				break;
			case (SpeciesBlock::TorsionKeyword):
				// Check the functional form specified
				tf = SpeciesTorsion::torsionFunction(parser.argc(1));
				if (tf == SpeciesTorsion::nTorsionFunctions)
				{
					Messenger::error("Functional form of torsion (%s) not recognised.\n", parser.argc(1));
					error = true;
					break;
				}
				// Create a new torsion definition
				t = species->addTorsion(parser.argi(2)-1, parser.argi(3)-1, parser.argi(4)-1, parser.argi(5)-1);
				if (t)
				{
					t->setForm(tf);
					for (int n=0; n<SpeciesTorsion::nFunctionParameters(tf); ++n)
					{
						if (!parser.hasArg(n+6))
						{
							Messenger::error("Torsion function type '%s' requires %i parameters\n", SpeciesTorsion::torsionFunction(tf), SpeciesTorsion::nFunctionParameters(tf));
							error = true;
							break;
						}
						t->setParameter(n, parser.argd(n+6));
					}
				}
				else error = true;
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
