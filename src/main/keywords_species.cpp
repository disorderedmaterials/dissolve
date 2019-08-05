/*
	*** Keyword Parsing - Species Block
	*** src/main/keywords_species.cpp
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
#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/isotopes.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Return enum option info for SpeciesKeyword
EnumOptions<SpeciesBlock::SpeciesKeyword> SpeciesBlock::keywords()
{
	static EnumOptionsList SpeciesKeywords = EnumOptionsList() <<
		EnumOption(SpeciesBlock::AngleKeyword,	 		"Angle",		4,6) <<
		EnumOption(SpeciesBlock::AtomKeyword, 			"Atom",			6,7) <<
		EnumOption(SpeciesBlock::AutoAddGrainsKeyword,		"AutoAddGrains") <<
		EnumOption(SpeciesBlock::BondKeyword,			"Bond",			3,5) <<
		EnumOption(SpeciesBlock::BondTypeKeyword,		"BondType",		3) <<
		EnumOption(SpeciesBlock::ChargeKeyword,			"Charge",		2) <<
		EnumOption(SpeciesBlock::EndSpeciesKeyword,		"EndSpecies") <<
		EnumOption(SpeciesBlock::GrainKeyword,			"Grain",		1) <<
		EnumOption(SpeciesBlock::IsotopologueKeyword,		"Isotopologue",		EnumOption::OneOrMoreArguments) <<
		EnumOption(SpeciesBlock::SiteKeyword,			"Site",			1) <<
		EnumOption(SpeciesBlock::TorsionKeyword, 		"Torsion",		5,9);

	static EnumOptions<SpeciesBlock::SpeciesKeyword> options("SpeciesKeyword", SpeciesKeywords);

	return options;
}

// Parse Species block
bool SpeciesBlock::parse(LineParser& parser, Dissolve* dissolve, Species* species)
{
	Messenger::print("\nParsing %s '%s'\n", BlockKeywords::keywords().keyword(BlockKeywords::SpeciesBlockKeyword), species->name());

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
	SpeciesBond::BondType bt;
	Isotope* tope;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		if (parser.getArgsDelim() != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!keywords().isValid(parser.argc(0))) return keywords().errorAndPrintValid(parser.argc(0));
		SpeciesKeyword kwd = keywords().enumeration(parser.argc(0));
		if (!keywords().validNArgs(kwd, parser.nArgs()-1)) return false;

		// All OK, so process the keyword
		switch (kwd)
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
					Messenger::error("Unrecognised element symbol '%s' found in %s keyword.\n", parser.argc(2), SpeciesBlock::keywords().keyword(SpeciesBlock::AtomKeyword));
					el = NULL;
					error = true;
					break;
				}
				i = species->addAtom(el, parser.arg3d(3));
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

				// Check that the AtomType was successfully assigned, and raise an error if not
				if (!i->atomType())
				{
					error = true;
					break;
				}
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
			case (SpeciesBlock::BondTypeKeyword):
				// Find the specified bond
				b = species->hasBond(parser.argi(1)-1, parser.argi(2)-1);
				if (!b)
				{
					Messenger::error("Tried to set the bond type of bond between atoms %i and %i, but this bond does not exist.\n", parser.argi(1), parser.argi(2));
					error = true;
					break;
				}

				// Get the bond type
				bt = SpeciesBond::bondType(parser.argc(3));
				if (bt == SpeciesBond::nBondTypes)
				{
					Messenger::error("Bond function type '%s' requires %i parameters\n", SpeciesBond::bondFunction(bf), SpeciesBond::nFunctionParameters(bf));
					error = true;
					break;
				}
				b->setBondType(bt);
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
				iso = species->addIsotopologue(species->uniqueIsotopologueName(parser.argc(1)), dissolve->atomTypes());
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
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::keywords().keyword(BlockKeywords::SpeciesBlockKeyword), keywords().keyword(kwd));
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
		Messenger::error("Unterminated %s block found.\n", BlockKeywords::keywords().keyword(BlockKeywords::SpeciesBlockKeyword));
		error = true;
	}

	return (!error);
}
