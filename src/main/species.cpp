/*
	*** Dissolve - Species Functions
	*** src/main/species.cpp
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

#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "base/sysfunc.h"
#include <string.h>

// Add a new Species to the list
Species* Dissolve::addSpecies()
{
	Species* newSpecies = coreData_.addSpecies();

	setUp_ = false;

	return newSpecies;
}

// Remove the specified Species from the list
void Dissolve::removeSpecies(Species* sp)
{
	// Remove Species
	coreData_.species().remove(sp);

	setUp_ = false;
}

// Return number of defined Species
int Dissolve::nSpecies() const
{
	return coreData_.nSpecies();
}

// Return Species list
List<Species>& Dissolve::species()
{
	return coreData_.species();
}

// Return nth Species in the list
Species* Dissolve::species(int n)
{
	return coreData_.species(n);
}

// Search for Species by name
Species* Dissolve::findSpecies(const char* name) const
{
	return coreData_.findSpecies(name);
}

// Update Species (or all) Isotopologues (or specified)
void Dissolve::updateIsotopologues(Species* species, Isotopologue* iso)
{
	if (iso) iso->update(coreData_.atomTypes());
	else if (species) species->updateIsotopologues(coreData_.atomTypes());
	else for (species = coreData_.species().first(); species != NULL; species = species->next) species->updateIsotopologues(coreData_.atomTypes());
}

// Remove Isotopologue from Species
void Dissolve::removeSpeciesIsotopologue(Species* species, Isotopologue* iso)
{
	// NULL pointer check
	if (species == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Species passed to Dissolve::removeSpeciesIsotopologue.\n");
		return;
	}
	if (iso == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Isotopologue passed to Dissolve::removeSpeciesIsotopologue.\n");
		return;
	}
	
	species->removeIsotopologue(iso);

	setUp_ = false;
}

// Copy AtomType, creating a new one if necessary
void Dissolve::copyAtomType(SpeciesAtom* sourceAtom, SpeciesAtom* destAtom)
{
	// Check for no AtomType being set
	if (!sourceAtom->atomType())
	{
		destAtom->setAtomType(NULL);
		return;
	}

	// Search for the existing atom's AtomType by name, and create it if it doesn't exist
	AtomType* at = findAtomType(sourceAtom->atomType()->name());
	if (!at)
	{
		at = addAtomType(sourceAtom->element());
		at->setName(sourceAtom->atomType()->name());
		at->parameters() = sourceAtom->atomType()->parameters();
	}

	destAtom->setAtomType(at);
}

// Copy intramolecular interaction parameters, adding MasterIntra if necessary
void Dissolve::copySpeciesIntra(SpeciesIntra* sourceIntra, SpeciesIntra* destIntra)
{
	// Remove any existing master parameters link from the destination object
	if (destIntra->masterParameters()) destIntra->detachFromMasterIntra();

	// If sourceIntra referneces a MasterIntra, check for its presence in the supplied Dissolve reference, and create it if necessary
	if (sourceIntra->masterParameters())
	{
		// Search for MasterIntra by the same name in our main Dissolve instance
		MasterIntra* master = NULL;
		if (sourceIntra->type() == SpeciesIntra::IntramolecularBond)
		{
			master = hasMasterBond(sourceIntra->masterParameters()->name());
			if (!master)
			{
				master = addMasterBond(sourceIntra->masterParameters()->name());
				master->setParameters(sourceIntra->parametersAsArray());
			}
		}
		else if (sourceIntra->type() == SpeciesIntra::IntramolecularAngle)
		{
			master = hasMasterAngle(sourceIntra->masterParameters()->name());
			if (!master)
			{
				master = addMasterAngle(sourceIntra->masterParameters()->name());
				master->setParameters(sourceIntra->parametersAsArray());
			}
		}
		else if (sourceIntra->type() == SpeciesIntra::IntramolecularTorsion)
		{
			master = hasMasterTorsion(sourceIntra->masterParameters()->name());
			if (!master)
			{
				master = addMasterTorsion(sourceIntra->masterParameters()->name());
				master->setParameters(sourceIntra->parametersAsArray());
			}
		}

		master->setForm(sourceIntra->masterParameters()->form());

		// Set the master pointer in the interaction
		destIntra->setMasterParameters(master);
	}
	else
	{
		// Just copy over form / parameters
		destIntra->setForm(sourceIntra->form());
		destIntra->setParameters(sourceIntra->parametersAsArray());
	}
}

// Copy Species from supplied instance
Species* Dissolve::copySpecies(const Species* species)
{
	// Create our new Species
	Species* newSpecies = addSpecies();
	newSpecies->setName(coreData_.uniqueSpeciesName(species->name()));

	// Duplicate atoms
	ListIterator<SpeciesAtom> atomIterator(species->atoms());
	while (SpeciesAtom* i = atomIterator.iterate())
	{
		// Create the Atom in our new Species
		SpeciesAtom* newAtom = newSpecies->addAtom(i->element(), i->r());

		// Search for the existing atom's AtomType by name, and create it if it doesn't exist
		copyAtomType(i, newAtom);
	}

	// Duplicate bonds
	ListIterator<SpeciesBond> bondIterator(species->bonds());
	while (SpeciesBond* b = bondIterator.iterate())
	{
		// Create the bond in the new Species
		SpeciesBond* newBond = newSpecies->addBond(b->indexI(), b->indexJ());

		// Copy interaction parameters, including MasterIntra if necessary
		copySpeciesIntra(b, newBond);
	}

	// Duplicate angles
	ListIterator<SpeciesAngle> angleIterator(species->angles());
	while (SpeciesAngle* a = angleIterator.iterate())
	{
		// Create the angle in the new Species
		SpeciesAngle* newAngle = newSpecies->addAngle(a->indexI(), a->indexJ(), a->indexK());

		// Copy interaction parameters, including MasterIntra if necessary
		copySpeciesIntra(a, newAngle);
	}

	// Duplicate torsions
	ListIterator<SpeciesTorsion> torsionIterator(species->torsions());
	while (SpeciesTorsion* t = torsionIterator.iterate())
	{
		// Create the torsion in the new Species
		SpeciesTorsion* newTorsion = newSpecies->addTorsion(t->indexI(), t->indexJ(), t->indexK(), t->indexL());

		// Copy interaction parameters, including MasterIntra if necessary
		copySpeciesIntra(t, newTorsion);
	}

	// Finalise the new Species
	newSpecies->updateUsedAtomTypes();
	newSpecies->addNaturalIsotopologue(atomTypes());
	newSpecies->updateGrains();
	newSpecies->centreAtOrigin();
	newSpecies->orderAtomsWithinGrains();

	return newSpecies;
}
