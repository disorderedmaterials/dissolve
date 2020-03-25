/*
	*** Species Input/Output
	*** src/classes/species_io.cpp
	Copyright T. Youngs 2012-2020

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

#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/coredata.h"
#include "data/fflibrary.h"
#include "data/elements.h"
#include "data/isotopes.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Load Species from file
bool Species::load(const char* filename)
{
	// Grab extension from filename
	CharString ext = DissolveSys::lowerCase(DissolveSys::afterLastChar(filename, '.'));

	if (ext == "xyz") return loadFromXYZ(filename);
	else Messenger::print("Can't load - unknown extension for file '%s'.\n", filename);

	return false;
}

// Load Species information from XYZ file
bool Species::loadFromXYZ(const char* filename)
{
	Messenger::print("Loading XYZ data from file '%s'\n", filename);
	
	// Open the specified file...
	LineParser parser;
	parser.openInput(filename);
	if (!parser.isFileGoodForReading())
	{
		Messenger::error("Couldn't open XYZ file '%s'.\n", filename);
		return false;
	}

	// Clear any existing data
	clear();

	// Simple format - first line is number of atoms, next line is title, then follow atoms/coordinates, one atom per line
	parser.getArgsDelim(LineParser::Defaults);
	int nAtoms = parser.argi(0);
	parser.readNextLine(LineParser::Defaults);
	name_ = parser.line();
	int success;
	Element* el;
	for (int n=0; n<nAtoms; ++n)
	{
		success = parser.getArgsDelim(LineParser::Defaults);
		if (success != 0)
		{
			parser.closeFiles();
			Messenger::error("Couldn't read Atom %i from file '%s'\n", n+1, filename);
			return false;
		}
		el = Elements::elementPointer(parser.argc(0));
		SpeciesAtom* i = addAtom(el, parser.arg3d(1));
		if (parser.hasArg(4)) i->setCharge(parser.argd(4));
	}

	Messenger::print("Succesfully loaded XYZ data from file '%s'.\n", filename);
	parser.closeFiles();
	return true;
}

/*
 * Read / Write
 */

// Return enum option info for SpeciesKeyword
EnumOptions<Species::SpeciesKeyword> Species::keywords()
{
	static EnumOptionsList SpeciesKeywords = EnumOptionsList() <<
		EnumOption(Species::AngleKeyword,	 	"Angle",		3,6) <<
		EnumOption(Species::AtomKeyword, 		"Atom",			6,7) <<
		EnumOption(Species::BondKeyword,		"Bond",			2,5) <<
		EnumOption(Species::BondTypeKeyword,		"BondType",		3) <<
		EnumOption(Species::ChargeKeyword,		"Charge",		2) <<
		EnumOption(Species::CoordinateSetsKeyword,	"CoordinateSets",	2,99) <<
		EnumOption(Species::EndSpeciesKeyword,		"EndSpecies") <<
		EnumOption(Species::ForcefieldKeyword,		"Forcefield",		1) <<
		EnumOption(Species::ImproperKeyword, 		"Improper",		5,9) <<
		EnumOption(Species::IsotopologueKeyword,	"Isotopologue",		EnumOption::OneOrMoreArguments) <<
		EnumOption(Species::SiteKeyword,		"Site",			1) <<
		EnumOption(Species::TorsionKeyword, 		"Torsion",		4,9);

	static EnumOptions<Species::SpeciesKeyword> options("SpeciesKeyword", SpeciesKeywords);

	return options;
}

// Read Species definition from specified LineParser
bool Species::read(LineParser& parser, CoreData& coreData)
{
	Messenger::print("\nParsing Species '%s'\n", name());

	Element* el;
	CharString arg1, arg2;
	AtomType* at;
	Isotopologue* iso;
	SpeciesAngle* a;
	SpeciesAtom* i;
	SpeciesBond* b;
	SpeciesImproper* imp;
	SpeciesTorsion* t;
	SpeciesSite* site;
	SpeciesBond::BondFunction bf;
	SpeciesAngle::AngleFunction af;
	SpeciesImproper::ImproperFunction impf;
	SpeciesTorsion::TorsionFunction tf;
	SpeciesBond::BondType bt;
	Isotope* tope;
	bool blockDone = false, error = false;

	// Turn off intramolecular term autogeneration while we're reading
	autoUpdateIntramolecularTerms_ = false;

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
			case (Species::AngleKeyword):
				// Create a new angle definition between the specified atoms
				a = addAngle(parser.argi(1)-1, parser.argi(2)-1, parser.argi(3)-1);
				if (!a)
				{
					error = true;
					break;
				}

				/*
				 * If only the indices were given, create an angle without a specified functional form (a Forcefield is presumably going to be specified).
				 * Otherwise, check the functional form specified - if it starts with '@' it is a reference to master parameters
				 */
				if (parser.nArgs() == 4) a->setForm(SpeciesAngle::NoForm);
				else if (parser.argc(4)[0] == '@')
				{
					// Search through master Angle parameters to see if this name exists
					MasterIntra* master = coreData.hasMasterAngle(parser.argc(4));
					if (!master)
					{
						Messenger::error("No master Angle parameters named '%s' exist.\n", &parser.argc(4)[1]);
						error = true;
						break;
					}

					a->setMasterParameters(master);
				}
				else
				{
					if (!SpeciesAngle::angleFunctions().isValid(parser.argc(4)))
					{
						Messenger::error("Functional form of Angle (%s) not recognised.\n", parser.argc(4));
						error = true;
						break;
					}
					af = SpeciesAngle::angleFunctions().enumeration(parser.argc(4));

					a->setForm(af);
					for (int n=0; n<SpeciesAngle::angleFunctions().minArgs(af); ++n)
					{
						if (!parser.hasArg(n+5))
						{
							Messenger::error("Angle function type '%s' requires %i parameters\n", SpeciesAngle::angleFunctions().keyword(af), SpeciesAngle::angleFunctions().minArgs(af));
							error = true;
							break;
						}
						a->setParameter(n, parser.argd(n+5));
					}
				}

				// Perform any final setup on the Angle
				a->setUp();
				break;
			case (Species::AtomKeyword):
				el = Elements::elementPointer(parser.argc(2));
				if (el->Z() == 0)
				{
					Messenger::error("Unrecognised element symbol '%s' found in %s keyword.\n", parser.argc(2), Species::keywords().keyword(Species::AtomKeyword));
					el = NULL;
					error = true;
					break;
				}
				i = addAtom(el, parser.arg3d(3));
				if (parser.hasArg(7)) i->setCharge(parser.argd(7));

				// Locate the AtomType assigned to the Atom
				if (DissolveSys::sameString("None", parser.argc(6))) at = NULL;
				else
				{
					at = coreData.findAtomType(parser.argc(6));
					if (!at)
					{
						Messenger::printVerbose("Creating AtomType '%s'...\n", parser.argc(6));
						at = coreData.addAtomType(el);
						at->setName(parser.argc(6));
					}
				}

				// Finally, set AtomType for the Atom
				i->setAtomType(at);
				break;
			case (Species::BondKeyword):
				// Create a new bond definition between the specified atoms
				b = addBond(parser.argi(1)-1, parser.argi(2)-1);
				if (!b)
				{
					error = true;
					break;
				}

				/*
				 * If only the indices were given, create a bond without a specified functional form (a Forcefield is presumably going to be specified).
				 * Otherwise, check the functional form specified - if it starts with '@' it is a reference to master parameters
				 */
				if (parser.nArgs() == 3) b->setForm(SpeciesBond::NoForm);
				else if (parser.argc(3)[0] == '@')
				{
					// Search through master Bond parameters to see if this name exists
					MasterIntra* master = coreData.hasMasterBond(parser.argc(3));
					if (!master)
					{
						Messenger::error("No master Bond parameters named '%s' exist.\n", &parser.argc(3)[1]);
						error = true;
						break;
					}

					b->setMasterParameters(master);
				}
				else
				{
					// Check the functional form specified
					if (!SpeciesBond::bondFunctions().isValid(parser.argc(3)))
					{
						Messenger::error("Functional form of Bond (%s) not recognised.\n", parser.argc(3));
						error = true;
						break;
					}
					bf = SpeciesBond::bondFunctions().enumeration(parser.argc(3));

					b->setForm(bf);
					for (int n=0; n<SpeciesBond::bondFunctions().minArgs(bf); ++n)
					{
						if (!parser.hasArg(4+n))
						{
							Messenger::error("Bond function type '%s' requires %i parameters\n", SpeciesBond::bondFunctions().keyword(bf), SpeciesBond::bondFunctions().minArgs(bf));
							error = true;
							break;
						}
						b->setParameter(n, parser.argd(n+4));
					}
				}

				// Perform any final setup on the Bond
				b->setUp();
				break;
			case (Species::BondTypeKeyword):
				// Find the specified bond
				b = bond(parser.argi(1)-1, parser.argi(2)-1);
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
					Messenger::error("Unrecognised bond type '%s'.\n", parser.argc(3));
					error = true;
					break;
				}
				b->setBondType(bt);
				break;
			case (Species::ChargeKeyword):
				i = atom(parser.argi(1) - 1);
				if (i) i->setCharge(parser.argd(2));
				else
				{
					Messenger::error("Specified Atom index (%i) for Charge keyword is out of range.\n", parser.argi(1));
					error = true;
				}
				break;
			case (Species::CoordinateSetsKeyword):
				if (!coordinateSetInputCoordinates_.read(parser, 1, CharString("End%s", keywords().keyword(Species::CoordinateSetsKeyword)), coreData))
				{
					Messenger::error("Failed to set coordinate sets file / format.\n");
					error = true;
				}
				else
				{
					// Open the specified file
					LineParser coordinateSetParser(parser.processPool());
					if ((!coordinateSetParser.openInput(coordinateSetInputCoordinates_.filename())) || (!coordinateSetParser.isFileGoodForReading()))
					{
						Messenger::error("Couldn't open coordinate sets file '%s'.\n", coordinateSetInputCoordinates_.filename());
						error = true;
						break;
					}

					while (!coordinateSetParser.eofOrBlank())
					{
						CoordinateSet* coordSet = addCoordinateSet();
						if (!coordinateSetInputCoordinates_.importData(coordinateSetParser, coordSet->coordinates()))
						{
							Messenger::error("Failed to read coordinate set %i from file.\n", nCoordinateSets());
							error = true;
							coordinateSets_.remove(coordSet);
							break;
						}
					}

					Messenger::print("%i coordinate sets read in for Species '%s'.\n", nCoordinateSets(), name());
				}
				break;
			case (Species::EndSpeciesKeyword):
				if (forcefield_) applyForcefieldTerms(coreData);
				Messenger::print("Found end of Species '%s'.\n", name());
				blockDone = true;
				break;
			case (Species::ForcefieldKeyword):
				forcefield_ = ForcefieldLibrary::forcefield(parser.argc(1));
				break;
			case (Species::ImproperKeyword):
				// Check the functional form specified - if it starts with '@' it is a reference to master parameters
				if (parser.argc(5)[0] == '@')
				{
					// Search through master Improper parameters to see if this name exists
					MasterIntra* master = coreData.hasMasterImproper(parser.argc(5));
					if (!master)
					{
						Messenger::error("No master Improper parameters named '%s' exist.\n", &parser.argc(5)[1]);
						error = true;
						break;
					}

					// Create a new improper definition
					imp = addImproper(parser.argi(1)-1, parser.argi(2)-1, parser.argi(3)-1, parser.argi(4)-1);
					if (!imp)
					{
						error = true;
						break;
					}
					t->setMasterParameters(master);
				}
				else
				{
					// Check the functional form specified
					if (!SpeciesImproper::improperFunctions().isValid(parser.argc(5)))
					{
						Messenger::error("Functional form of Improper (%s) not recognised.\n", parser.argc(5));
						error = true;
						break;
					}
					impf = SpeciesImproper::improperFunctions().enumeration(parser.argc(5));

					// Create a new improper definition
					imp = addImproper(parser.argi(1)-1, parser.argi(2)-1, parser.argi(3)-1, parser.argi(4)-1);
					if (!imp)
					{
						error = true;
						break;
					}
					t->setForm(tf);
					for (int n=0; n<SpeciesImproper::improperFunctions().minArgs(impf); ++n)
					{
						if (!parser.hasArg(n+6))
						{
							Messenger::error("Improper function type '%s' requires %i parameters\n", SpeciesImproper::improperFunctions().keyword(impf), SpeciesImproper::improperFunctions().minArgs(impf));
							error = true;
							break;
						}
						t->setParameter(n, parser.argd(n+6));
					}
				}

				// Perform any final setup on the Improper
				t->setUp();
				break;
			case (Species::IsotopologueKeyword):
				iso = addIsotopologue(uniqueIsotopologueName(parser.argc(1)));
				Messenger::printVerbose("Added Isotopologue '%s' to Species '%s'\n", iso->name(), name());
				// Each parser argument is a string of the form ATOMTYPE=ISO
				for (int n=2; n<parser.nArgs(); ++n)
				{
					// Split argument into parts before and after '='
					arg1 = DissolveSys::beforeChar(parser.argc(n),'=');
					arg2 = DissolveSys::afterChar(parser.argc(n),'=');

					at = coreData.findAtomType(arg1.get());
					if (at == NULL)
					{
						Messenger::error("Failed to find AtomType '%s', referred to in Isotopologue '%s', Species '%s'\n", arg1.get(), iso->name(), name());
						error = true;
						break;
					}

					// Is the supplied isotope valid for the AtomType's element?
					el = at->element();
					tope = Isotopes::isotope(el, arg2.asInteger());
					if (tope == NULL)
					{
						Messenger::error("No such Isotope (%i) for element %s (AtomType '%s') in Isotopologue '%s', Species '%s'\n", arg2.asInteger(), el->symbol(), at->name(), iso->name(), name());
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
			case (Species::SiteKeyword):
				// First argument is the name of the site to create - make sure it doesn't exist already
				site = findSite(parser.argc(1));
				if (site)
				{
					Messenger::error("The site '%s' already exists on Species '%s', and cannot be redefined.\n", parser.argc(1), name());
					error = true;
					break;
				}

				site = addSite(parser.argc(1));
				if (!site->read(parser)) error = true;
				break;
			case (Species::TorsionKeyword):
				// Create a new angle definition between the specified atoms
				t = addTorsion(parser.argi(1)-1, parser.argi(2)-1, parser.argi(3)-1, parser.argi(4)-1);
				if (!t)
				{
					error = true;
					break;
				}

				/*
				 * If only the indices were given, create an angle without a specified functional form (a Forcefield is presumably going to be specified).
				 * Otherwise, check the functional form specified - if it starts with '@' it is a reference to master parameters
				 */
				if (parser.nArgs() == 5) t->setForm(SpeciesTorsion::NoForm);
				else if (parser.argc(5)[0] == '@')
				{
					// Search through master Torsion parameters to see if this name exists
					MasterIntra* master = coreData.hasMasterTorsion(parser.argc(5));
					if (!master)
					{
						Messenger::error("No master Torsion parameters named '%s' exist.\n", &parser.argc(5)[1]);
						error = true;
						break;
					}

					t->setMasterParameters(master);
				}
				else
				{
					// Check the functional form specified
					if (!SpeciesTorsion::torsionFunctions().isValid(parser.argc(5)))
					{
						Messenger::error("Functional form of Torsion (%s) not recognised.\n", parser.argc(5));
						error = true;
						break;
					}
					tf = SpeciesTorsion::torsionFunctions().enumeration(parser.argc(5));

					t->setForm(tf);
					for (int n=0; n<SpeciesTorsion::torsionFunctions().minArgs(tf); ++n)
					{
						if (!parser.hasArg(n+6))
						{
							Messenger::error("Torsion function type '%s' requires %i parameters\n", SpeciesTorsion::torsionFunctions().keyword(tf), SpeciesTorsion::torsionFunctions().minArgs(tf));
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
				printf("DEV_OOPS - Species block keyword '%s' not accounted for.\n", keywords().keyword(kwd));
				error = true;
				break;
		}

		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	// Turn intramolecular term autogeneration back on
	autoUpdateIntramolecularTerms_ = true;

	// If there's no error and the blockDone flag isn't set, return an error
	if (!error && !blockDone)
	{
		Messenger::error("Unterminated Species block found.\n");
		error = true;
	}

	return (!error);
}

// Write Species definition to specified LineParser
bool Species::write(LineParser& parser, const char* prefix)
{
	if (!parser.writeLineF("%sSpecies '%s'\n", prefix, name())) return false;

	// Create new prefix
	CharString newPrefix("%s  ", prefix);

	// Atoms
	parser.writeLineF("%s# Atoms\n", newPrefix.get());
	int count = 0;
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next())
	{
		if (!parser.writeLineF("%s%s  %3i  %3s  %12.6e  %12.6e  %12.6e  '%s'  %12.6e\n", newPrefix.get(), keywords().keyword(Species::AtomKeyword), ++count, i->element()->symbol(), i->r().x, i->r().y, i->r().z, i->atomType() == NULL ? "None" : i->atomType()->name(), i->charge())) return false;
	}

	// Bonds
	RefList<const SpeciesBond> bondTypes[SpeciesBond::nBondTypes];
	if (nBonds() > 0)
	{
		if (!parser.writeLineF("\n%s# Bonds\n", newPrefix.get())) return false;
		DynamicArrayConstIterator<SpeciesBond> bondIterator(bonds());
		while (const SpeciesBond* b = bondIterator.iterate())
		{
			if (b->form() == SpeciesBond::NoForm)
			{
				if (!parser.writeLineF("%s%s  %3i  %3i\n", newPrefix.get(), keywords().keyword(Species::BondKeyword), b->indexI()+1, b->indexJ()+1)) return false;
			}
			else if (b->masterParameters())
			{
				if (!parser.writeLineF("%s%s  %3i  %3i  @%s\n", newPrefix.get(), keywords().keyword(Species::BondKeyword), b->indexI()+1, b->indexJ()+1, b->masterParameters()->name())) return false;
			}
			else
			{
				CharString s("%s%s  %3i  %3i  %s", newPrefix.get(), keywords().keyword(Species::BondKeyword), b->indexI()+1, b->indexJ()+1, SpeciesBond::bondFunctions().keywordFromInt(b->form()));
				for (int n=0; n<SpeciesBond::bondFunctions().minArgs( (SpeciesBond::BondFunction) b->form()); ++n) s.strcatf("  %8.3f", b->parameter(n));
				if (!parser.writeLineF("%s\n", s.get())) return false;
			}

			// Add the bond to the reflist corresponding to its indicated bond type (unless it is a SingleBond, which we will ignore as this is the default)
			if (b->bondType() != SpeciesBond::SingleBond) bondTypes[b->bondType()].append(b);
		}

		// Any bond type information to write?
		bool bondTypeHeaderWritten = false;
		for (int bt=1; bt<SpeciesBond::nBondTypes; ++bt) if (bondTypes[bt].nItems() > 0)
		{
			// Write header if it hasn't been written already
			if (!bondTypeHeaderWritten)
			{
				if (!parser.writeLineF("\n%s# Bond Types\n", newPrefix.get())) return false;
				bondTypeHeaderWritten = true;
			}
			for (const SpeciesBond* bond : bondTypes[bt]) if (!parser.writeLineF("%s%s  %3i  %3i  %s\n", newPrefix.get(), keywords().keyword(Species::BondTypeKeyword), bond->indexI()+1, bond->indexJ()+1, SpeciesBond::bondType((SpeciesBond::BondType) bt))) return false;
		}
	}

	// Angles
	if (nAngles() > 0)
	{
		if (!parser.writeLineF("\n%s# Angles\n", newPrefix.get())) return false;
		DynamicArrayConstIterator<SpeciesAngle> angleIterator(angles());
		while (const SpeciesAngle* a = angleIterator.iterate())
		{
			if (a->form() == SpeciesAngle::NoForm)
			{
				if (!parser.writeLineF("%s%s  %3i  %3i  %3i\n", newPrefix.get(), keywords().keyword(Species::AngleKeyword), a->indexI()+1, a->indexJ()+1, a->indexK()+1)) return false;
			}
			else if (a->masterParameters())
			{
				if (!parser.writeLineF("%s%s  %3i  %3i  %3i  @%s\n", newPrefix.get(), keywords().keyword(Species::AngleKeyword), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->masterParameters()->name())) return false;
			}
			else
			{
				CharString s("%s%s  %3i  %3i  %3i  %s", newPrefix.get(), keywords().keyword(Species::AngleKeyword), a->indexI()+1, a->indexJ()+1, a->indexK()+1, SpeciesAngle::angleFunctions().keywordFromInt(a->form()));
				for (int n=0; n<SpeciesAngle::angleFunctions().minArgs((SpeciesAngle::AngleFunction) a->form()); ++n) s.strcatf("  %8.3f", a->parameter(n));
				if (!parser.writeLineF("%s\n", s.get())) return false;
			}
		}
	}

	// Torsions
	if (nTorsions() > 0)
	{
		if (!parser.writeLineF("\n%s# Torsions\n", newPrefix.get())) return false;
		DynamicArrayConstIterator<SpeciesTorsion> torsionIterator(torsions());
		while (const SpeciesTorsion* t = torsionIterator.iterate())
		{
			if (t->form() == SpeciesTorsion::NoForm)
			{
				if (!parser.writeLineF("%s%s  %3i  %3i  %3i  %3i\n", newPrefix.get(), keywords().keyword(Species::TorsionKeyword), t->indexI()+1, t->indexJ()+1, t->indexK()+1, t->indexL()+1)) return false;
			}
			else if (t->masterParameters())
			{
				if (!parser.writeLineF("%s%s  %3i  %3i  %3i  %3i  @%s\n", newPrefix.get(), keywords().keyword(Species::TorsionKeyword), t->indexI()+1, t->indexJ()+1, t->indexK()+1, t->indexL()+1, t->masterParameters()->name())) return false;
			}
			else
			{
				CharString s("%s%s  %3i  %3i  %3i  %3i  %s", newPrefix.get(), keywords().keyword(Species::TorsionKeyword), t->indexI()+1, t->indexJ()+1, t->indexK()+1, t->indexL()+1, SpeciesTorsion::torsionFunctions().keywordFromInt(t->form()));
				for (int n=0; n<SpeciesTorsion::torsionFunctions().minArgs( (SpeciesTorsion::TorsionFunction) t->form()); ++n) s.strcatf("  %8.3f", t->parameter(n));
				if (!parser.writeLineF("%s\n", s.get())) return false;
			}
		}
	}

	// Impropers
	if (nImpropers() > 0)
	{
		if (!parser.writeLineF("\n%s# Impropers\n", newPrefix.get())) return false;
		DynamicArrayConstIterator<SpeciesImproper> improperIterator(impropers());
		while (const SpeciesImproper* imp = improperIterator.iterate())
		{
			if (imp->masterParameters())
			{
				if (!parser.writeLineF("%s%s  %3i  %3i  %3i  %3i  @%s\n", newPrefix.get(), keywords().keyword(Species::ImproperKeyword), imp->indexI()+1, imp->indexJ()+1, imp->indexK()+1, imp->indexL()+1, imp->masterParameters()->name())) return false;
			}
			else
			{
				CharString s("%s%s  %3i  %3i  %3i  %3i  %s", newPrefix.get(), keywords().keyword(Species::ImproperKeyword), imp->indexI()+1, imp->indexJ()+1, imp->indexK()+1, imp->indexL()+1, SpeciesImproper::improperFunctions().keywordFromInt(imp->form()));
				for (int n=0; n<SpeciesImproper::improperFunctions().minArgs( (SpeciesImproper::ImproperFunction) imp->form()); ++n) s.strcatf("  %8.3f", imp->parameter(n));
				if (!parser.writeLineF("%s\n", s.get())) return false;
			}
		}
	}

	// Forcefield
	if (forcefield_)
	{
		if (!parser.writeLineF("\n%s# Forcefield\n", newPrefix.get())) return false;
		if (!parser.writeLineF("%s%s  '%s'\n", newPrefix.get(), keywords().keyword(Species::ForcefieldKeyword), forcefield_->name())) return false;
	}

	// Isotopologues
	if (nIsotopologues() > 0)
	{
		if (!parser.writeLineF("\n%s# Isotopologues\n", newPrefix.get())) return false;

		for (Isotopologue* iso = isotopologues_.first(); iso != NULL; iso = iso->next())
		{
			if (!parser.writeLineF("%s%s  '%s'", newPrefix.get(), keywords().keyword(Species::IsotopologueKeyword), iso->name())) return false;
			RefDataListIterator<AtomType,Isotope*> isotopeIterator(iso->isotopes());
			while (AtomType* atomType = isotopeIterator.iterate())
			{
				// No need to write anything that's the natural isotope...
				if (isotopeIterator.currentData()->A() == 0) continue;

				if (!parser.writeLineF("  %s=%i", atomType->name(), isotopeIterator.currentData()->A())) return false;
			}
			if (!parser.writeLineF("\n")) return false;
		}
	}

	// Sites
	if (nSites() > 0)
	{
		if (!parser.writeLineF("\n%s# Sites\n", newPrefix.get())) return false;

		ListIterator<SpeciesSite> siteIterator(sites());
		while (SpeciesSite* site = siteIterator.iterate()) if (!site->write(parser, newPrefix)) return false;
	}

	// Input Coordinates
	if (coordinateSetInputCoordinates_.hasValidFileAndFormat())
	{
		if (!coordinateSetInputCoordinates_.writeFilenameAndFormat(parser, CharString("\n%s%s  ", newPrefix.get(), keywords().keyword(Species::CoordinateSetsKeyword)))) return false;
		if (!coordinateSetInputCoordinates_.writeBlock(parser, newPrefix.get())) return false;
		if (!parser.writeLineF("%sEnd%s\n", newPrefix.get(), keywords().keyword(Species::CoordinateSetsKeyword))) return false;
	}

	// Done with this species
	if (!parser.writeLineF("%s%s\n", prefix, keywords().keyword(Species::EndSpeciesKeyword))) return false;

	return true;
}
