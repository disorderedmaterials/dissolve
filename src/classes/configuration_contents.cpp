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

// Add Molecule to Configuration based on the supplied Species
Molecule* Configuration::addMolecule(Species* sp)
{
	// Create the new Molecule object
	Molecule* newMolecule = molecules_.add();

	// Add Atoms from Species to the Molecule
	SpeciesAtom* spi = sp->atoms();
	for (int n=0; n<sp->nAtoms(); ++n, spi = spi->next)
	{
		// Create new Atom
		Atom* i = addAtom(newMolecule);

		// Copy information from SpeciesAtom
		i->setElement(spi->element());
		i->setCharge(spi->charge());
		i->setCoordinates(spi->r());

		// Update our typeIndex (non-isotopic) and set local and master type indices
		AtomTypeData* atd = usedAtomTypes_.add(spi->atomType(), 1);
		i->setLocalTypeIndex(atd->listIndex());
		i->setMasterTypeIndex(spi->atomType()->index());
	}

	// Add Grains from Species into the Molecule
	SpeciesGrain* spg = sp->grains();
	for (int n = 0; n<sp->nGrains(); ++n, spg = spg->next)
	{
		// Create new Grain
		Grain* g = addGrain(newMolecule);

		// Add Atoms to the Grain
		for (int m=0; m<spg->nAtoms(); ++m)
		{
			g->addAtom(newMolecule->atom(spg->atom(m)->item->index()));
		}
	}

	// Add Bonds
	SpeciesBond* spb = sp->bonds();
	for (int n = 0; n<sp->nBonds(); ++n, spb = spb->next)
	{
		// Get Atom pointers involved in Bond
		Atom* i = newMolecule->atom(spb->indexI());
		Atom* j = newMolecule->atom(spb->indexJ());

		// Create new Bond
		Bond* b = addBond(newMolecule, i, j);
		b->setSpeciesBond(spb);
	}

	// Add Angles
	SpeciesAngle* spa = sp->angles();
	for (int n = 0; n<sp->nAngles(); ++n, spa = spa->next)
	{
		// Get Atom pointers involved in Angle
		Atom* i = newMolecule->atom(spa->indexI());
		Atom* j = newMolecule->atom(spa->indexJ());
		Atom* k = newMolecule->atom(spa->indexK());

		// Create new Angle
		Angle* a = addAngle(newMolecule, i, j, k);
		a->setSpeciesAngle(spa);
	}

	// Add Torsions
	SpeciesTorsion* spt = sp->torsions();
	for (int n = 0; n<sp->nTorsions(); ++n, spt = spt->next)
	{
		// Get Atom pointers involved in Torsion
		Atom* i = newMolecule->atom(spt->indexI());
		Atom* j = newMolecule->atom(spt->indexJ());
		Atom* k = newMolecule->atom(spt->indexK());
		Atom* l = newMolecule->atom(spt->indexL());

		// Create new Torsion
		Torsion* t = addTorsion(newMolecule, i, j, k, l);
		t->setSpeciesTorsion(spt);
	}
}

// Return number of Molecules in Configuration
int Configuration::nMolecules() const
{
	return molecules_.nItems();
}

// Return first Molecule
DynamicArray<Molecule>& Configuration::molecules()
{
	return molecules_;
}

// Return nth Molecule
Molecule* Configuration::molecule(int n)
{
	return molecules_[n];
}

// Add new Grain to Configuration, with Molecule parent specified
Grain* Configuration::addGrain(Molecule* molecule)
{
	// Create the new Grain object
	Grain* newGrain = grains_.add();

	// Add it to the specified Molecule, which also sets the Molecule parent of the Grain
	molecule->addGrain(newGrain);

	return newGrain;
}

// Return number of grains
int Configuration::nGrains() const
{
	return grains_.nItems();
}

// Return grain array
DynamicArray<Grain>& Configuration::grains()
{
	return grains_;
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

// Add new Atom to Configuration, with Molecule and Grain parents specified
Atom* Configuration::addAtom(Molecule* molecule, Grain* grain)
{
	// Create new Atom object
	Atom* newAtom = atoms_.add();
	molecule->addAtom(newAtom);
	newAtom->setGrain(grain);

	return newAtom;
}

// Return number of Atoms in Configuration
int Configuration::nAtoms() const
{
	return atoms_.nItems();
}

// Return Atom array
DynamicArray<Atom>& Configuration::atoms()
{
	return atoms_;
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

// Add new Bond to Configuration, with Molecule parent specified
Bond* Configuration::addBond(Molecule* molecule, Atom* i, Atom* j)
{
	// Create new Bond object
	Bond* newBond = bonds_.add();
	molecule->addBond(newBond);
	newBond->setAtoms(i, j);

	// Update local bound lists in Atoms
	i->addBond(newBond);
	j->addBond(newBond);

	return newBond;
}

// Return number of Bonds in Configuration
int Configuration::nBonds() const
{
	return bonds_.nItems();
}

// Return Bond array
DynamicArray<Bond>& Configuration::bonds()
{
	return bonds_;
}

// Return nth Bond
Bond* Configuration::bond(int n)
{
	return bonds_[n];
}

// Add new Angle to Configuration, with Molecule parent specified
Angle* Configuration::addAngle(Molecule* molecule, Atom* i, Atom* j, Atom* k)
{
	// Create new Angle object
	Angle* newAngle = angles_.add();
	molecule->addAngle(newAngle);
	newAngle->setAtoms(i, j, k);

	// Update local bound lists in Atoms
	i->addAngle(newAngle);
	j->addAngle(newAngle);
	k->addAngle(newAngle);

	return newAngle;
}

// Return number of Angles in Configuration
int Configuration::nAngles() const
{
	return angles_.nItems();
}

// Return Angle array
DynamicArray<Angle>& Configuration::angles()
{
	return angles_;
}

// Return nth Angle
Angle* Configuration::angle(int n)
{
	return angles_[n];
}

// Add new Torsion to Configuration, with Molecule parent specified
Torsion* Configuration::addTorsion(Molecule* molecule, Atom* i, Atom* j, Atom* k, Atom* l)
{
	// Create new Torsion object
	Torsion* newTorsion = torsions_.add();
	molecule->addTorsion(newTorsion);
	newTorsion->setAtoms(i, j, k, l);

	// Update local bound lists in Atoms
	i->addTorsion(newTorsion, 0.5);
	j->addTorsion(newTorsion, 0.5);
	k->addTorsion(newTorsion, 0.5);
	l->addTorsion(newTorsion, 0.5);

	return newTorsion;
}

// Return number of Torsions in Configuration
int Configuration::nTorsions() const
{
	return torsions_.nItems();
}

// Return Torsion array
DynamicArray<Torsion>& Configuration::torsions()
{
	return torsions_;
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

