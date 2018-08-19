/*
	*** Keyword Parsing - Species Block
	*** src/main/keywords_species.cpp
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
#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/isotopes.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Species Block Keywords
KeywordData SpeciesBlockData[] = {
	{ "Angle",			4,	"Define an angle interaction within the Species" },
	{ "Atom",			6,	"Define an Atom in the Species" },
	{ "AutoAddGrains",		0,	"Automatically add Grains to cover all atoms in the Species" },
	{ "Bond",			3,	"Define a bond interaction within the Species" },
	{ "Charge",			2,	"Specify the atomic charge for an individual atom" },
	{ "EndSpecies",			0,	"Ends the current Species definition" },
	{ "Grain",			1,	"Define a Grain within the Species " },
	{ "Isotopologue",		1,	"Add an isotopologue to the Species" },
	{ "Site",			1,	"Define an analysis site for the Species" },
	{ "Torsion",			5,	"Define a torsion interaction within the Species" }
};

// Convert text string to SpeciesKeyword
SpeciesBlock::SpeciesKeyword SpeciesBlock::keyword(const char* s)
{
	for (int n=0; n<nSpeciesKeywords; ++n) if (DissolveSys::sameString(s,SpeciesBlockData[n].name)) return (SpeciesBlock::SpeciesKeyword) n;
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
bool SpeciesBlock::parse(LineParser& parser, Dissolve* dissolve, Species* species)
{
	Messenger::print("\nParsing %s '%s'\n", MainInputKeywords::mainInputKeyword(MainInputKeywords::SpeciesBlockKeyword), species->name());

	Element* el;
	CharString arg1, arg2;
	AtomType* at;
	Isotopologue* iso;
	SpeciesAngle* a;
	SpeciesAtom* i;
	SpeciesBond* b;
	SpeciesTorsion* t;
	SpeciesGrain* sg;
	SpeciesSite* site;
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
				// Check the functional form specified - if it starts with '@' it is a reference to master parameters
				if (parser.argc(4)[0] == '@')
				{
					// Search through master Angle parameters to see if this name exists
					MasterIntra* master = dissolve->hasMasterAngle(parser.argc(4));
					if (!master)
					{
						Messenger::error("No master Angle parameters named '%s' exist.\n", &parser.argc(4)[1]);
						error = true;
						break;
					}

					// Create a new angle definition
					a = species->addAngle(parser.argi(1)-1, parser.argi(2)-1, parser.argi(3)-1);
					if (!a)
					{
						error = true;
						break;
					}
					a->setMasterParameters(master);
				}
				else
				{
					af = SpeciesAngle::angleFunction(parser.argc(4));
					if (af == SpeciesAngle::nAngleFunctions)
					{
						Messenger::error("Functional form of Angle (%s) not recognised.\n", parser.argc(4));
						error = true;
						break;
					}

					// Create a new angle definition
					a = species->addAngle(parser.argi(1)-1, parser.argi(2)-1, parser.argi(3)-1);
					if (!a)
					{
						error = true;
						break;
					}
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

				// Perform any final setup on the Angle
				a->setUp();
				break;
			case (SpeciesBlock::AtomKeyword):
				el = Elements::elementPointer(parser.argc(2));
				if (el->Z() == 0)
				{
					Messenger::error("Unrecognised element symbol '%s' found in %s keyword.\n", parser.argc(2), SpeciesBlock::keyword(SpeciesBlock::AtomKeyword));
					el = NULL;
					error = true;
					break;
				}
				i = species->addAtom(el, parser.argd(3), parser.argd(4), parser.argd(5));
				if (parser.hasArg(7)) i->setCharge(parser.argd(7));

				// Locate the AtomType assigned to the Atom
				at = dissolve->findAtomType(parser.argc(6));
				if (!at)
				{
					Messenger::print("Creating AtomType '%s'...\n", parser.argc(6));
					at = dissolve->addAtomType(el);
					at->setName(parser.argc(6));
				}

				// Finally, set AtomType for the Atom
				i->setAtomType(at);
				break;
			case (SpeciesBlock::AutoAddGrainsKeyword):
				species->autoAddGrains();
				break;
			case (SpeciesBlock::BondKeyword):
				// Check the functional form specified - if it starts with '@' it is a reference to master parameters
				if (parser.argc(3)[0] == '@')
				{
					// Search through master Bond parameters to see if this name exists
					MasterIntra* master = dissolve->hasMasterBond(parser.argc(3));
					if (!master)
					{
						Messenger::error("No master Bond parameters named '%s' exist.\n", &parser.argc(3)[1]);
						error = true;
						break;
					}

					// Create a new bond definition
					b = species->addBond(parser.argi(1)-1, parser.argi(2)-1);
					if (!b)
					{
						error = true;
						break;
					}
					b->setMasterParameters(master);
				}
				else
				{
					// Check the functional form specified
					bf = SpeciesBond::bondFunction(parser.argc(3));
					if (bf == SpeciesBond::nBondFunctions)
					{
						Messenger::error("Functional form of Bond (%s) not recognised.\n", parser.argc(3));
						error = true;
						break;
					}

					// Create a new bond definition
					b = species->addBond(parser.argi(1)-1, parser.argi(2)-1);
					if (!b)
					{
						error = true;
						break;
					}
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

				// Perform any final setup on the Bond
				b->setUp();
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
				species->updateUsedAtomTypes();
				species->addNaturalIsotopologue();
				species->updateGrains();
				species->centreAtOrigin();
				species->orderAtomsWithinGrains();
				Messenger::print("Found end of Species '%s'.\n", species->name());
				blockDone = true;
				break;
			case (SpeciesBlock::GrainKeyword):
				sg = species->addGrain();
				sg->setName(species->uniqueGrainName(parser.argc(1)));
				Messenger::print("Added grain definition '%s' to Species '%s'\n", sg->name(), species->name());
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
				Messenger::print("Added Isotopologue '%s' to Species '%s'\n", iso->name(), species->name());
				// Each parser argument is a string of the form ATOMTYPE=ISO
				for (int n=2; n<parser.nArgs(); ++n)
				{
					// Split argument into parts before and after '='
					arg1 = DissolveSys::beforeChar(parser.argc(n),'=');
					arg2 = DissolveSys::afterChar(parser.argc(n),'=');
					
					at = dissolve->findAtomType(arg1.get());
					if (at == NULL)
					{
						Messenger::error("Failed to find AtomType '%s', referred to in Isotopologue '%s', Species '%s'\n", arg1.get(), iso->name(), species->name());
						error = true;
						break;
					}

					// Is the supplied isotope valid for the AtomType's element?
					el = at->element();
					tope = Isotopes::isotope(el, arg2.asInteger());
					if (tope == NULL)
					{
						Messenger::error("No such Isotope (%i) for element %s (AtomType '%s') in Isotopologue '%s', Species '%s'\n", arg2.asInteger(), el->symbol(), at->name(), iso->name(), species->name());
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
			case (SpeciesBlock::SiteKeyword):
				// First argument is the name of the site to create - make sure it doesn't exist already
				site = species->findSite(parser.argc(1));
				if (site)
				{
					Messenger::error("The site '%s' already exists on Species '%s', and cannot be redefined.\n", parser.argc(1), species->name());
					error = true;
					break;
				}

				site = species->addSite(parser.argc(1));
				if (!SiteBlock::parse(parser, dissolve, site)) error = true;
				break;
			case (SpeciesBlock::TorsionKeyword):
				// Check the functional form specified - if it starts with '@' it is a reference to master parameters
				if (parser.argc(5)[0] == '@')
				{
					// Search through master Torsion parameters to see if this name exists
					MasterIntra* master = dissolve->hasMasterTorsion(parser.argc(5));
					if (!master)
					{
						Messenger::error("No master Torsion parameters named '%s' exist.\n", &parser.argc(5)[1]);
						error = true;
						break;
					}

					// Create a new torsion definition
					t = species->addTorsion(parser.argi(1)-1, parser.argi(2)-1, parser.argi(3)-1, parser.argi(4)-1);
					if (!t)
					{
						error = true;
						break;
					}
					t->setMasterParameters(master);
				}
				else
				{
					// Check the functional form specified
					tf = SpeciesTorsion::torsionFunction(parser.argc(5));
					if (tf == SpeciesTorsion::nTorsionFunctions)
					{
						Messenger::error("Functional form of Torsion (%s) not recognised.\n", parser.argc(5));
						error = true;
						break;
					}

					// Create a new torsion definition
					t = species->addTorsion(parser.argi(1)-1, parser.argi(2)-1, parser.argi(3)-1, parser.argi(4)-1);
					if (!t)
					{
						error = true;
						break;
					}
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

				// Perform any final setup on the Torsion
				t->setUp();
				break;
			case (SpeciesBlock::nSpeciesKeywords):
				Messenger::error("Unrecognised %s block keyword '%s' found.\n", MainInputKeywords::mainInputKeyword(MainInputKeywords::SpeciesBlockKeyword), parser.argc(0));
				MainInputKeywords::printValidBlockKeywords(MainInputKeywords::SpeciesBlockKeyword);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", MainInputKeywords::mainInputKeyword(MainInputKeywords::SpeciesBlockKeyword), SpeciesBlock::keyword(spKeyword));
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
