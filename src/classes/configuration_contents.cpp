/*
	*** Configuration - Contents
	*** src/classes/configuration_contents.cpp
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

#include "classes/configuration.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/grain.h"
#include "classes/species.h"
#include "base/processpool.h"
#include "modules/import/import.h"

// Clear contents of Configuration, leaving core definitions intact
void Configuration::empty()
{
	bonds_.clear();
	angles_.clear();
	torsions_.clear();
	molecules_.clear();
	grains_.clear();
	atoms_.clear();
	usedAtomTypes_.clear();
	if (box_ != NULL) delete box_;
	box_ = new CubicBox(1.0);
	cells_.clear();

	// Set the populations of SpeciesInfo to zero, but leave the list intact
	ListIterator<SpeciesInfo> speciesInfoIterator(usedSpecies_);
	while (SpeciesInfo* spInfo = speciesInfoIterator.iterate()) spInfo->zeroPopulation();

	++contentsVersion_;
}

// Initialise content arrays
void Configuration::initialiseArrays(int nMolecules, int nGrains)
{
	// Clear current contents
	empty();

	molecules_.initialise(nMolecules);
	grains_.initialise(nGrains);
}

// Return specified used type
AtomType* Configuration::usedAtomType(int index)
{
	return usedAtomTypes_.atomType(index);
}

// Return specified used type data
AtomTypeData* Configuration::usedAtomTypeData(int index)
{
	return usedAtomTypes_[index];
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
int Configuration::nUsedAtomTypes() const
{
	return usedAtomTypes_.nItems();
}

// Add Species to list of those used by the Configuration, setting/adding the population specified
SpeciesInfo* Configuration::addUsedSpecies(Species* sp, int population)
{
	// Check if we have an existing info for this Species
	SpeciesInfo* spInfo = usedSpeciesInfo(sp);
	if (!spInfo)
	{
		spInfo = usedSpecies_.add();
		spInfo->setSpecies(sp);
	}

	// Increase the population
	spInfo->addPopulation(population);

	return spInfo;
}

// Return SpeciesInfo for specified Species
SpeciesInfo* Configuration::usedSpeciesInfo(Species* sp)
{
	for (SpeciesInfo* spInfo = usedSpecies_.first(); spInfo != NULL; spInfo = spInfo->next) if (spInfo->species() == sp) return spInfo;

	return NULL;
}

// Return list of SpeciesInfo for the Configuration
List<SpeciesInfo>& Configuration::usedSpecies()
{
	return usedSpecies_;
}

// Return if the specified Species is present in the usedSpecies list
bool Configuration::hasUsedSpecies(Species* sp)
{
	for (SpeciesInfo* spInfo = usedSpecies_.first(); spInfo != NULL; spInfo = spInfo->next) if (spInfo->species() == sp) return true;

	return false;
}

// Return the atomic density of the Configuration
double Configuration::atomicDensity() const
{
	return nAtoms() / box_->volume();
}

// Return version of current contents
int Configuration::contentsVersion() const
{
	return contentsVersion_;
}

// Increment version of current contents
void Configuration::incrementContentsVersion()
{
	++contentsVersion_;
}

// Add Molecule to Configuration based on the supplied Species
Molecule* Configuration::addMolecule(Species* sp)
{
	// Create the new Molecule object and set its Species pointer
	Molecule* newMolecule = molecules_.add();
	newMolecule->setSpecies(sp);

	// Update the relevant SpeciesInfo population
	addUsedSpecies(sp, 1);

	// Add Atoms from Species to the Molecule
	SpeciesAtom* spi = sp->firstAtom();
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
			g->addAtom(newMolecule->atom(spg->atom(m)->item()->index()));
		}
	}

	// Add Bonds
	SpeciesBond* spb = sp->bonds().first();
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
	SpeciesAngle* spa = sp->angles().first();
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
	SpeciesTorsion* spt = sp->torsions().first();
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

	return newMolecule;
}

// Return number of Molecules in Configuration
int Configuration::nMolecules() const
{
	return molecules_.nItems();
}

// Return array of Molecules
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
	if ((n < 0) || (n >= grains_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Grain index %i passed to Configuration::grain() is out of range (nGrains = %i).\n", n, grains_.nItems());
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

// Add new Atom with full data
Atom* Configuration::addAtom(Molecule* molecule, Grain* grain, AtomType* atomType, Vec3<double> r, double charge)
{
	// Create new Atom object
	Atom* newAtom = atoms_.add();
	molecule->addAtom(newAtom);
	newAtom->setGrain(grain);

	// Set element (from AtomType), coordinates and charge
	newAtom->setElement(atomType->element());
	newAtom->setCoordinates(r);
	newAtom->setCharge(charge);

	// Update our typeIndex (non-isotopic) and set local and master type indices
	AtomTypeData* atd = usedAtomTypes_.add(atomType, 1);
	newAtom->setLocalTypeIndex(atd->listIndex());
	newAtom->setMasterTypeIndex(atomType->index());

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

// Return Atom array (const)
const DynamicArray<Atom>& Configuration::constAtoms() const
{
	return atoms_;
}

// Return nth atom
Atom* Configuration::atom(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Atom index %i passed to Configuration::atom() is out of range (nAtoms = %i).\n", n, atoms_.nItems());
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

// Add new Bond to Configuration, with Molecule parent specified, from Atom indices
Bond* Configuration::addBond(Molecule* molecule, int i, int j)
{
	return addBond(molecule, atoms_[i], atoms_[j]);
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

// Return Bond array (const)
const DynamicArray<Bond>& Configuration::constBonds() const
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

// Add new Angle to Configuration, with Molecule parent specified, from Atom indices
Angle* Configuration::addAngle(Molecule* molecule, int i, int j, int k)
{
	return addAngle(molecule, atoms_[i], atoms_[j], atoms_[k]);
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

// Add new Torsion to Configuration, with Molecule parent specified, from Atom indices
Torsion* Configuration::addTorsion(Molecule* molecule, int i, int j, int k, int l)
{
	return addTorsion(molecule, atoms_[i], atoms_[j], atoms_[k], atoms_[l]);
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
