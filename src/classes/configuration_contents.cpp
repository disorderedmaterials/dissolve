/*
	*** Configuration - Contents
	*** src/classes/configuration_contents.cpp
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

#include "classes/configuration.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/grain.h"
#include "classes/species.h"
#include "base/processpool.h"
#include "modules/import/import.h"

// Return number of Molecules in Configuration
int Configuration::nMolecules() const
{
	return molecules_.nItems();
}

// Return first Molecule
Molecule** Configuration::molecules()
{
	return molecules_.array();
}

// Return nth Molecule
Molecule* Configuration::molecule(int n)
{
	return molecules_[n];
}

// Return number of grains
int Configuration::nGrains() const
{
	return grains_.nItems();
}

// Return grain array
Grain** Configuration::grains()
{
	return grains_.array();
}

// Return nth grain
Grain* Configuration::grain(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= nGrains_))
	{
		Messenger::print("OUT_OF_RANGE - Grain index %i passed to Configuration::grain() is out of range (nGrains_ = %i).\n", n, nGrains_);
		return NULL;
	}
#endif
	return grains_[n];
}

// Return number of Atoms in Configuration
int Configuration::nAtoms() const
{
	return atoms_.nItems();
}

// Return Atom array
Atom** Configuration::atoms()
{
	return atoms_.array();
}

// Return nth atom
Atom* Configuration::atom(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= nAtoms_))
	{
		Messenger::print("OUT_OF_RANGE - Atom index %i passed to Configuration::atom() is out of range (nAtoms_ = %i).\n", n, nAtoms_);
		return NULL;
	}
#endif
	return atoms_[n];
}

// Return number of Bonds in Configuration
int Configuration::nBonds() const
{
	return bonds_.nItems();
}

// Return Bond array
Bond** Configuration::bonds()
{
	return bonds_.array();
}

// Return nth Bond
Bond* Configuration::bond(int n)
{
	return bonds_[n];
}

// Return number of Angles in Configuration
int Configuration::nAngles() const
{
	return angles_.nItems();
}

// Return Angle array
Angle** Configuration::angles()
{
	return angles_.array();
}

// Return nth Angle
Angle* Configuration::angle(int n)
{
	return angles_[n];
}

// Return number of Torsions in Configuration
int Configuration::nTorsions() const
{
	return torsions_.nItems();
}

// Return Torsion array
Torsion** Configuration::torsions()
{
	return torsions_.array();
}

// Return nth Torsion
Torsion* Configuration::torsion(int n)
{
	return torsions_[n];
}

// Return specified type
AtomType* Configuration::type(int index)
{
	return usedAtomTypes_.atomType(index);
}

// Return first AtomTypeData for this Configuration
AtomTypeData* Configuration::usedAtomTypes()
{
	return usedAtomTypes_.first();
}

// Return AtomTypeList for this Configuration
const AtomTypeList& Configuration::usedAtomTypesList() const
{
	return usedAtomTypes_;
}

// Return number of atom types used in this Configuration
int Configuration::nUsedAtomTypes()
{
	return usedAtomTypes_.nItems();
}

// Return current coordinate index
int Configuration::coordinateIndex()
{
	return coordinateIndex_;
}

// Increment current coordinate index
void Configuration::incrementCoordinateIndex()
{
	++coordinateIndex_;
}

// Load coordinates from file
bool Configuration::loadCoordinates(LineParser& parser, const char* format)
{
	// Load coordinates into temporary array
	Array< Vec3<double> > r;
	if (!ImportModule::readCoordinates(format, parser, r)) return false;

	// Temporary array now contains some number of atoms - does it match the number in the configuration's molecules?
	if (atoms_.nItems() != r.nItems())
	{
		Messenger::error("Number of atoms read from initial coordinates file (%i) does not match that in Configuration (%i).\n", r.nItems(), atoms_.nItems());
		return false;
	}

	// All good, so copy atom coordinates over into our array
	for (int n=0; n<atoms_.nItems(); ++n) atoms_[n]->setCoordinates(r[n]);

	return true;
}

