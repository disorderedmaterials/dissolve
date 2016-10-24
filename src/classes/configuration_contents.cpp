/*
	*** Configuration - Species Contents Functions
	*** src/classes/configuration_contents.cpp
	Copyright T. Youngs 2012-2016

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
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/grain.h"
#include "classes/species.h"
#include "base/comms.h"

/*
 * \brief Add space for a Molecule of the Species provided
 * Add an empty Molecule to the Configuration. The Molecule will contain array space for referencing the number of Atoms and Grains
 * defined in the supplied Species. No actual Atom or Grain information is created in this function (this is done by Molecule::instantiate()).
 */
void Configuration::addMolecule(Species* sp)
{
#ifdef CHECKS
	// Check for NULL pointer
	if (sp == NULL)
	{
		msg.error("NULL_POINTER - NULL Species pointer given to Configuration::addMolecule().\n");
		return;
	}
#endif
	Molecule* mol = molecules_.add();
	mol->initialise(sp, molecules_.nItems()-1);
	nGrains_ += mol->nGrains();
	nAtoms_ += mol->nAtoms();
}

// Return number of Molecules
int Configuration::nMolecules() const
{
	return molecules_.nItems();
}

// Return first Molecule
Molecule* Configuration::molecules()
{
	return molecules_.first();
}

// Return specified molecule
Molecule* Configuration::molecule(int id)
{
#ifdef CHECKS
	static Molecule dummy;
	if ((id < 0) || (id >= molecules_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Molecule index %i passed to Configuration::molecule() is out of range (nMolecules = %i).\n", id, molecules_.nItems());
		return &dummy;
	}
#endif	
	return molecules_[id];
}

// Return number of grains
int Configuration::nGrains() const
{
	return nGrains_;
}

// Return grain array
Grain* Configuration::grains()
{
	return grains_;
}

// Return nth grain
Grain& Configuration::grain(int n)
{
#ifdef CHECKS
	static Grain dummy;
	if ((n < 0) || (n >= nGrains_))
	{
		msg.print("OUT_OF_RANGE - Grain index %i passed to Configuration::grain() is out of range (nGrains_ = %i).\n", n, nGrains_);
		return dummy;
	}
#endif
	return grains_[n];
}

// Return number of atoms
int Configuration::nAtoms() const
{
	return nAtoms_;
}

// Return atom array
Atom* Configuration::atoms()
{
	return atoms_;
}

// Return nth atom
Atom* Configuration::atom(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index %i passed to Configuration::atom() is out of range (nAtoms_ = %i).\n", n, nAtoms_);
		return NULL;
	}
#endif
	return &atoms_[n];
}

// Setup Arrays
bool Configuration::setupArrays()
{
	if (molecules_.nItems() == 0)
	{
		msg.error("Configuration::setupArrays - No Molecules have been added to the Configuration - no arrays to be created.\n");
		return false;
	}
	if (nAtoms_== 0)
	{
		msg.error("Configuration::setupArrays - Molecules have been added to the Configuration, but no Atoms were counted.\n");
		return false;
	}
	if (nGrains_ == 0)
	{
		msg.error("Configuration::setupArrays - Molecules have been added to the Configuration, but no Grains were counted.\n");
		return false;
	}
	
	msg.print("--> Creating arrays for %i atoms and %i grains...\n", nAtoms_, nGrains_);
	try
	{
		atoms_ = new Atom[nAtoms_];
		grains_ = new Grain[nGrains_];
	}
	catch (bad_alloc& alloc)
	{
		msg.error("Configuration::setupArrays - Failed to allocate sufficient memory. Exception was : %s\n", alloc.what());
		return false;
	}

	// Setup grain indices
	for (int n=0; n<nGrains_; ++n) grains_[n].setIndex(n);

	return true;
}

/*
 * \brief Setup Molecules
 * \details Once molecules have been added to the Configuration with addMolecule(), and the master arrays of atoms and grains have
 * been allocated by setupArrays(), the coordinates of the individual atoms can be set. If atoms are present in the supplied Species, setupMolecules()
 * takes coordinates from there and copies them to the molecules defined in the configuration. Otherwise, a random centre of geometry and orientation is
 * generated for each molecule.
 */
bool Configuration::setupMolecules(Species& sourceCoordinates)
{
	// If there are no atoms in the sourceCoordinates species, assume that we are creating a random configuration.
	int atomCount = 0, grainCount = 0, localTypeIndex;
	Cell* c;
	Matrix3 transform;
	Vec3<double> r, cog, newCentre;
	bool randomising = sourceCoordinates.nAtoms() == 0;
	Species* sp;
	SpeciesAtom* sourceI = sourceCoordinates.atoms();

	// Initialise a shared random number pool - this will ensure all processes generate the same coordinates
	Comm.initialiseRandomBuffer(DUQComm::World);

	// Loop over defined molecules
	for (Molecule* mol = molecules_.first(); mol != NULL; mol = mol->next)
	{
		// Grab pointer to parent species
		sp = mol->species();

		// If we are randomising the configuration, get the centre of geometry for the species and generate a random centre and rotation matrix
		if (randomising)
		{
			// Calculate the centre of geometry of the molecule's source species
			cog = mol->species()->centreOfGeometry(box_);

			// Generate a new random centre of geometry for the molecule
			newCentre = box_->randomCoordinate();
			
			// Generate a random rotation matrix
			transform.createRotationXY(Comm.randomPlusMinusOne()*180.0, Comm.randomPlusMinusOne()*180.0);
		}

		// Atoms
		SpeciesAtom* i = mol->species()->atoms();
		for (int n = 0; n<sp->nAtoms(); ++n)
		{
			// Get / generate atom coordinate
			if (randomising) r = (transform * (i->r() - cog)) + newCentre;
			else
			{
				r = sourceI->r();
				sourceI = sourceI->next;
			}

			// Get cell location of atom, and find an empty atom slot in that cell
			c = cell(r);

			// Set the master atom reference
			atoms_[atomCount].setIndex(atomCount);
			atoms_[atomCount].setCoordinatesNasty(r);
			atoms_[atomCount].setCell(c);
			if (!c->addAtom(&atoms_[atomCount])) return false;

			// Update our typeIndex (non-isotopic) and set index of the atom
			localTypeIndex = usedAtomTypes_.add(i->atomType(), NULL, 1);
			atoms_[atomCount].setLocalTypeIndex(localTypeIndex);

			// Set atom pointer and character (charge, atomtype etc.) of atom in molecule from the source Species
			if (!mol->setupAtom(n, &atoms_[atomCount], i)) return false;

			++atomCount;
			i = i->next;
		}

		// Grains
		for (int n = 0; n<mol->nGrains(); ++n)
		{
			if (grainCount >= nGrains_)
			{
				msg.error("Mismatch between size of grain array in configuration, and number of grains needed by molecules.\n");
				return false;
			}
			if (!mol->setupGrain(n, &grains_[grainCount], sp->grain(n))) return false;

			++grainCount;
		}

	}

	return true;
}

// Return specified type
AtomType* Configuration::type(int index)
{
	return usedAtomTypes_.atomType(index);
}

// Return first AtomTypeData for this configuration
AtomTypeData* Configuration::usedAtomTypes()
{
	return usedAtomTypes_.first();
}

// Set global AtomType indices in Atoms from the map provided
bool Configuration::setGlobalAtomTypeIndices(const AtomTypeIndex& masterIndex)
{
	// Loop over AtomTypes in index, then over Atoms in Configuration
	int nAssigned = 0, globalIndex = 0;
	for (AtomTypeData* atd = masterIndex.first(); atd != NULL; atd = atd->next, ++globalIndex)
	{
		// Grab AtomType pointer
		AtomType* at = atd->atomType();

		// Loop over Molecules (need the original Species pointer to look up original AtomType pointer)
		int localIndex = 0;
		for (Molecule* mol = molecules_.first(); mol != NULL; mol = mol->next)
		{
			Species* sp = mol->species();
			for (int i=0; i<sp->nAtoms(); ++i, ++localIndex)
			{
				if (at == sp->atom(i)->atomType())
				{
					atoms_[localIndex].setGlobalTypeIndex(globalIndex);
					++nAssigned;
				}
			}
		}
	}

	// Did we successfully assign AtomTypes to all Atoms?
	if (nAssigned != nAtoms_)
	{
		msg.error("Failed to assign global AtomType indices to all Atoms in Configuration '%s'.\n", name());
		return false;
	}

	return true;
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
