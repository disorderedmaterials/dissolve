/*
	*** SPC/Fw Forcefield
	*** src/data/ff/spcfw.cpp
	Copyright T. Youngs 2019

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

#include "data/ff/spcfw.h"
#include "data/ffangleterm.h"
#include "data/ffatomtype.h"
#include "data/ffbondterm.h"
#include "classes/atomtype.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/sysfunc.h"

/*
 * Implements "Flexible simple point-charge water model with improved liquid-state properties"
 * Yujie Wu, Harald L. Tepper and Gregory A. Voth
 * Journal of Chemical Physics 124 024503 (2006)
 * http://dx.doi.org/10.1063/1.2136877
 * 
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of TGAY.
 * All energy values are in kcal.
 */

// Constructor / Destructor
Forcefield_SPCFw::Forcefield_SPCFw()
{
	static ForcefieldAtomType atomTypes[] =
	{
		// 	Z	El	FFID	Name		Description
		//						q	Epsilon	Sigma
		{ this, 1,	"H",	1,	"HW",		"Water hydrogen",
								0.41,	0.0,	0.0 },
		{ this, 8,	"O",	2,	"OW",		"Water oxygen",
								-0.82,	0.6503,	3.165492 }
	};

	static ForcefieldBondTerm bondTerms[] =
	{
		//	i	j	Type (Harmonic)			k		eq
		{ this,	"HW",	"OW",	SpeciesBond::HarmonicForm,	4431.53,	1.0 }
	};

	static ForcefieldAngleTerm angleTerms[] =
	{
		//	i	j	k	Type (Harmonic)			k		eq
		{ this,	"HW",	"OW",	"HW",	SpeciesAngle::HarmonicForm,	317.5656,	113.24 }
	};
}

Forcefield_SPCFw::~Forcefield_SPCFw()
{
}

/*
 * Definition
 */

// Return name of Forcefield
const char* Forcefield_SPCFw::name()
{
	return "SPC/Fw";
}

/*
 * Atom Type Data
 */

// Determine and return atom type for specified SpeciesAtom
ForcefieldAtomType* Forcefield_SPCFw::determineAtomType(SpeciesAtom* i) const
{
	switch (i->element()->Z())
	{
		// Hydrogen
		case (ELEMENT_H): 
				if (isBoundTo(i, &Elements::element(ELEMENT_O), 1, false)) return atomTypeByName("HW", i->element());
			break;
		// Oxygen
		case (ELEMENT_O):
				if (isBoundTo(i, &Elements::element(ELEMENT_H), 2, false)) return atomTypeByName("OW", i->element());
			break;
		// Default
		default:
			break;
	}

	return NULL;
}

/*
 * Term Assignment
 */


// Assign suitable atom types to the supplied Species
bool Forcefield_SPCFw::assignAtomTypes(Species* sp, CoreData& coreData, bool keepExisting) const
{
	// Loop over Species atoms
	for (SpeciesAtom* i = sp->atoms().first(); i != NULL; i = i->next())
	{
		// If keepExisting == true, don't reassign a type to this atom if one already exists
		if (keepExisting && i->atomType()) continue;

		ForcefieldAtomType* atomType = determineAtomType(i);
		if (!atomType) Messenger::print("No forcefield type available for Atom %i of Species (%s).\n", i->index()+1, i->element()->symbol());
		else
		{
			// Check if an AtomType of the same name already exists - if it does, just use that one
			AtomType* at = coreData.findAtomType(atomType->typeName());
			if (!at)
			{
				at = coreData.addAtomType(i->element());
				at->setName(atomType->typeName());

				// Copy parameters from the Forcefield's atom type
				at->parameters() = atomType->parameters();
			}

			i->setAtomType(at);
		}
	}

	return true;
}
