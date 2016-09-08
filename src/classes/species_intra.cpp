/*
	*** Species Definition - Intramolecular Terms
	*** src/classes/species_intra.cpp
	Copyright T. Youngs 2012-2014

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

#include "main/flags.h"
#include "classes/species.h"
#include "classes/box.h"

/*!
 * \brief Add new Bond definition (from Atoms*)
 * \details Add a new Bond definition to the Species, between the Atoms specified.
 */
SpeciesBond* Species::addBond(SpeciesAtom* i, SpeciesAtom* j)
{
	// Check ownership of these Atoms
	if (!atoms_.contains(i))
	{
		msg.print("BAD_OWNERSHIP - Atom 'i' is not owned by Species '%s' in Species::addBond.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(j))
	{
		msg.print("BAD_OWNERSHIP - Atom 'j' is not owned by Species '%s' in Species::addBond.\n", name_.get());
		return NULL;
	}

	// Check for existence of Bond already
	if (hasBond(i, j))
	{
		msg.print("Warning: Refused to add a new Bond between atoms %i and %i in Species '%s' since it already exists.\n", i->userIndex(), j->userIndex(), name_.get());
		return NULL;
	}
	
	// OK to add new Bond
	SpeciesBond* b = bonds_.add();
	b->setParent(this);
	b->setAtoms(i, j);
	i->addBond(b);
	j->addBond(b);

	Flags::wave(Flags::SpeciesChanged);
	
	return b;
}

/*!
 * \brief Add new Bond definition
 * \details Add a new Bond definition to the Species, between the Atoms with indices specified.
 */
SpeciesBond* Species::addBond(int i, int j)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addBond() is out of range (%i) for Species '%s'\n", i, name_.get());
		return false;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addBond() is out of range (%i) for Species '%s'\n", j, name_.get());
		return false;
	}

	return addBond(atoms_[i], atoms_[j]);
}

/*!
 * \brief Return number of bonds in list
 */
int Species::nBonds() const
{
	return bonds_.nItems();
}

/*!
 * \brief Return list of bonds
 */
SpeciesBond *Species::bonds() const
{
	return bonds_.first();
}

/*!
 * \brief Return nth bond
 */
SpeciesBond *Species::bond(int n)
{
	return bonds_[n];
}

/*!
 * \brief Return whether bond between atoms exists
 */
SpeciesBond* Species::hasBond(SpeciesAtom* i, SpeciesAtom* j) const
{
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next) if (b->matches(i, j)) return b;
	return NULL;
}

/*!
 * \brief Add new angle definition (from supplied atom pointers)
 * \details Add a new angle definition to the species, between the atoms specified.
 */
SpeciesAngle* Species::addAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k)
{
	// Check ownership of these Atoms
	if (!atoms_.contains(i))
	{
		msg.print("BAD_OWNERSHIP - Atom 'i' is not owned by Species '%s' in Species::addAngle.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(j))
	{
		msg.print("BAD_OWNERSHIP - Atom 'j' is not owned by Species '%s' in Species::addAngle.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(k))
	{
		msg.print("BAD_OWNERSHIP - Atom 'k' is not owned by Species '%s' in Species::addAngle.\n", name_.get());
		return NULL;
	}

	// Check for existence of Angle already
	if (hasAngle(i, j, k))
	{
		msg.print("Warning: Refused to add a new Angle between atoms %i, %i and %i in Species '%s' since it already exists.\n", i->userIndex()+1, j->userIndex()+1, k->userIndex()+1, name_.get());
		return NULL;
	}

	// OK to add new angle
	SpeciesAngle* a = angles_.add();
	a->setParent(this);
	a->setAtoms(i, j, k);
	
	Flags::wave(Flags::SpeciesChanged);
	
	return a;
}

/*!
 * \brief Add new angle definition
 * \details Add a new angle definition to the species, between the atoms with indices specified.
 */
SpeciesAngle* Species::addAngle(int i, int j, int k)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", i, name_.get());
		return false;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", j, name_.get());
		return false;
	}
	if ((k < 0) || (k >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Internal index 'k' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", k, name_.get());
		return false;
	}
	return addAngle(atoms_[i], atoms_[j], atoms_[k]);
}

/*!
 * \brief Return number of angles in list
 */
int Species::nAngles() const
{
	return angles_.nItems();
}

/*!
 * \brief Return list of angles
 */
SpeciesAngle *Species::angles() const
{
	return angles_.first();
}

/*!
 * \brief Return nth angle
 */
SpeciesAngle *Species::angle(int n)
{
	return angles_[n];
}

/*!
 * \brief Return whether angle between atoms exists
 */
bool Species::hasAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k) const
{
	for (SpeciesAngle* a = angles_.first(); a != NULL; a = a->next) if (a->matches(i, j, k)) return true;
	return false;
}

/*!
 * \brief Recalculate intramolecular terms between Atoms in the Species
 * \details Determines chemical Bonds within the Species, based on the distances between defined Atoms and a table of
 * representative atomic radii. Once bonds have been determined, angle terms are automatically generated.
 */
void Species::recalculateIntramolecular()
{
	SpeciesAtom* i, *j;
	SpeciesBond* b;
	SpeciesAngle* a;
	RefListItem<SpeciesBond,int>* ri, *rj;
	int n = 0;
	double dist, radius_i;

	// Clear existing bond and angle definitions
	for (i = atoms_.first(); i != NULL; i = i->next) i->clearBonds();
	bonds_.clear();
	angles_.clear();

	// First, determine which Atoms are within bonding distance
	for (i = atoms_.first(); i != atoms_.last(); i = i->next)
	{
		radius_i = PeriodicTable::element(i->element()).atomicRadius();
		for (j = i->next; j != NULL; j = j->next)
		{
			dist = (i->r()-j->r()).magnitude();
			if (dist <= (radius_i+PeriodicTable::element(j->element()).atomicRadius())*1.15)
			{
				// Create Bond and set initial equilibrium distance
				b = addBond(i, j);
				b->setEquilibrium(dist);
			}
		}
		++n;
	}
	msg.print("Found %i Bonds in Species '%s'.\n", bonds_.nItems(), name_.get());

	// Construct Angles list, looping over central Atom
	for (j = atoms_.first(); j != NULL; j = j->next)
	{
		// Double loop over Bonds
		printf("Angle is on Atom %i which has %i bonds\n", j->userIndex(), j->nBonds());;
		for (ri = j->bonds(); ri != NULL; ri = ri->next)
		{
			i = ri->item->partner(j);
			for (rj = ri->next; rj != NULL; rj = rj->next)
			{
				// Create Angle and set initial equilibrium angle
				a = addAngle(i, j, rj->item->partner(j));
				a->setEquilibrium(Box::angle(i->r(), j->r(), rj->item->partner(j)->r()));
			}
		}
	}
	
	Flags::wave(Flags::SpeciesChanged);
}

/*!
 * \brief Calculate local Atom index lists for interactions
 * \details For each Bond and Angle defined within the Species, all directly or indirectly attached Atoms at each terminus of the
 * interaction are selected and the indices added to the relevant local lists for the interaction. If, in the process, we find that
 * when selecting from Atom 'i' (of a Bond) we end up selecting Atom 'j', then the two Atoms are present in a cycle and we must explicitly
 * deal with such cases. But how!!! XXX TODO
 */
bool Species::calculateIndexLists()
{
	// Bonds
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next)
	{
		// Atom 'i'
		clearAtomSelection();
		selectFromAtom(b->i(), b);
		if (selectedAtoms_.contains(b->j()))
		{
// 			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
// 			return false;
			msg.print("Bond between Atoms %i and %i in Species '%s' exists in a cycle, so a minimal set of attached atoms will be used.\n", b->indexI()+1, b->indexJ()+1, name_.get());
			clearAtomSelection();
// 			for (RefListItem<Bond,int>* ri = b->i()->bonds(); ri != NULL; ri = ri->next) if (ri->item->partner(b->i()) != b->j()) selectedAtoms_.add(ri->item->partner(b->i()));
			selectedAtoms_.add(b->j());
		}
		b->setAttachedAtoms(0, selectedAtoms_);

		// Atom 'j'
		clearAtomSelection();
		selectFromAtom(b->j(), b);
		if (selectedAtoms_.contains(b->i()))
		{
// 			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
// 			return false;
			msg.print("Bond between Atoms %i and %i in Species '%s' exists in a cycle, so a minimal set of attached atoms will be used.\n", b->indexI()+1, b->indexJ()+1, name_.get());
			clearAtomSelection();
// 			for (RefListItem<Bond,int>* ri = b->j()->bonds(); ri != NULL; ri = ri->next) if (ri->item->partner(b->j()) != b->i()) selectedAtoms_.add(ri->item->partner(b->j()));
			selectedAtoms_.add(b->j());
		}
		b->setAttachedAtoms(1, selectedAtoms_);
	}

	// Angles
	for (SpeciesAngle* a = angles_.first(); a != NULL; a = a->next)
	{
		// Atom 'i'
		clearAtomSelection();
		SpeciesBond* b = hasBond(a->i(), a->j());
		if (b == NULL)
		{
			msg.error("Species '%s' contains an angle %i-%i-%i, but a bond %i-%i is not defined.\n", name_.get(), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->indexI()+1, a->indexJ()+1 );
			return false;
		}
		selectFromAtom(a->i(), b);
		if (selectedAtoms_.contains(a->j()))
		{
// 			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
// 			return false;
			msg.print("Angle between atoms %i-%i-%i in species '%s' exists in a cycle, so a minimal set of attached atoms will be used.\n", a->indexI()+1, a->indexJ()+1, a->indexK()+1, name_.get());
			clearAtomSelection();
			selectedAtoms_.add(a->i());
		}
		a->setAttachedAtoms(0, selectedAtoms_);

		// Atom 'k'
		clearAtomSelection();
		b = hasBond(a->j(), a->k());
		if (b == NULL)
		{
			msg.error("Species '%s' contains an angle %i-%i-%i, but a bond %i-%i is not defined.\n", name_.get(), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->indexJ()+1, a->indexK()+1 );
			return false;
		}
		selectFromAtom(a->k(), b);
		if (selectedAtoms_.contains(a->j()))
		{
// 			msg.error("Species '%s' contains one or more cycles, and these cannot be dealt with at present.\n", name_.get());
// 			return false;
			msg.print("Angle between atoms %i-%i-%i in species '%s' exists in a cycle, so a minimal set of attached atoms will be used.\n", a->indexI()+1, a->indexJ()+1, a->indexK()+1, name_.get());
			clearAtomSelection();
			selectedAtoms_.add(a->k());
		}
		a->setAttachedAtoms(1, selectedAtoms_);
	}
}

/*!
 * \brief Create scaling matrix
 */
void Species::createScalingMatrix()
{
	int n, m, rootIndex = atoms_[0]->index();

	scalingMatrix_.initialise(atoms_.nItems(), atoms_.nItems());

	// Unitise matrix (i.e. set all Atom pairs to interact fully)
	scalingMatrix_ = 1.0;
	
	// 'Torsion' interactions (set to 0.5)
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next)
	{
		// TODO
	}
	
	// Bond interactions (set to 0.0)
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next)
	{
		n = b->indexI() - rootIndex;
		m = b->indexJ() - rootIndex;
		scalingMatrix_.ref(n,m) = 0.0;
		scalingMatrix_.ref(m,n) = 0.0;
	}

	// Angle interactions (set to 0.0)
	for (SpeciesAngle* a = angles_.first(); a != NULL; a = a->next)
	{
		n = a->indexI() - rootIndex;
		m = a->indexK() - rootIndex;
		scalingMatrix_.ref(n,m) = 0.0;
		scalingMatrix_.ref(m,n) = 0.0;
	}
}

/*!
 * \brief Return scaling factor for supplied indices
 */
double Species::scaling(int indexI, int indexJ) const
{
#ifdef CHECKS
	if ((indexI < 0) || (indexI >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - After rooting, supplied Atom indexI is out of range (%i) (nAtoms = %i).\n", indexI, atoms_.nItems());
		return 0.0;
	}
	if ((indexJ < 0) || (indexJ >= atoms_.nItems()))
	{
		msg.print("OUT_OF_RANGE - After rooting, supplied Atom indexJ is out of range (%i) (nAtoms = %i).\n", indexJ, atoms_.nItems());
		return 0.0;
	}
#endif
	return scalingMatrix_.value(indexI, indexJ);
}

/*!
 * \brief Identify inter-Grain terms
 */
void Species::identifyInterGrainTerms()
{
	// Clear existing connections in grains
	for (SpeciesGrain* sg = grains_.first(); sg != NULL; sg = sg->next) sg->clearConnections();

	// Bonds
	for (SpeciesBond* b = bonds_.first(); b != NULL; b = b->next)
	{
#ifdef CHECKS
		if ((b->i() == NULL) || (b->j() == NULL))
		{
			msg.error("NULL_POINTER - One or both Atom pointers in a Bond are NULL (%p, %p)\n", b->i(), b->j());
			continue;
		}
		if ((b->i()->grain() == NULL) || (b->j()->grain() == NULL))
		{
			msg.error("NULL_POINTER - One or both Grain pointers in a Bond are NULL (%p, %p)\n", b->i()->grain(), b->j()->grain());
			continue;
		}
#endif

		// Is this an entirely INTRA-Grain Bond?
		if (b->i()->grain() == b->j()->grain())
		{
			b->i()->grain()->addInternalBond(b);
			continue;
		}

		// Bond is between two Grains, so add it to both and set the interGrain flag
		b->setInterGrain(true);
		b->i()->grain()->addBondConnection(b);
		b->j()->grain()->addBondConnection(b);
	}

	// Angles
	for (SpeciesAngle* a = angles_.first(); a != NULL; a = a->next)
	{
#ifdef CHECKS
		if ((a->i() == NULL) || (a->j() == NULL) || (a->k() == NULL))
		{
			msg.error("NULL_POINTER - One or more Atom pointers in an Angle are NULL (%p, %p, %p)\n", a->i(), a->j(), a->k());
			continue;
		}
		if ((a->i()->grain() == NULL) || (a->j()->grain() == NULL) || (a->k()->grain() == NULL))
		{
			msg.error("NULL_POINTER - One or more Grain pointers in an Angle are NULL (%p, %p, %p)\n", a->i()->grain(), a->j()->grain(), a->k()->grain());
			continue;
		}
#endif

		// Is this an entirely INTRA-Grain Angle?
		if ((a->i()->grain() == a->j()->grain()) && (a->j()->grain() == a->k()->grain()))
		{
			a->i()->grain()->addInternalAngle(a);
			continue;
		}
		
		// Angle is between at least two Grains, so add it to the relevant parties.
		// Always add to central Atom...
		a->j()->grain()->addAngleConnection(a);
		
		// If Atom 'i' is in a different Grain, add it to that...
		if (a->j()->grain() != a->i()->grain()) a->i()->grain()->addAngleConnection(a);
		
		// If Atom 'k' is in *another* different Grain, add it to that too
		
		// Only add to third Atom if its different again...
		if ((a->k()->grain() != a->i()->grain()) && (a->k()->grain() != a->j()->grain())) a->k()->grain()->addAngleConnection(a);
		
		// Finally, if 'i' and 'k' are in different Grains, set the intraGrain flag to intramolecular correction energies are calculated correctly
		if (a->i()->grain() != a->k()->grain()) a->setInterGrain(true);
	}
}
