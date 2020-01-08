/*
	*** Species Definition - Intramolecular Terms
	*** src/classes/species_intra.cpp
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

#include "classes/species.h"
#include "classes/box.h"
#include "data/atomicradius.h"
#include "base/sysfunc.h"


/*
 * Private
 */

// Add missing higher order intramolecular terms from current bond connectivity, and prune any that are now invalid
void Species::updateIntramolecularTerms()
{
	SpeciesAtom* i, *j, *k, *l;
	SpeciesBond* ij, *kl;

	// Loop over bonds 'jk'
	DynamicArrayIterator<SpeciesBond> jkIterator(bonds_);
	while (SpeciesBond* jk = jkIterator.iterate())
	{
		// Get atoms 'j' and 'k'
		j = jk->i();
		k = jk->j();

		// Loop over bonds 'ij'
		for (int ijIndex = 0; ijIndex < j->nBonds(); ++ijIndex)
		{
			// Get bond 'ij' and check for 'ij' == 'jk'
			ij = j->bond(ijIndex);
			if (ij == jk) continue;

			// Get atom 'i'
			i = ij->partner(j);

			// Attempt to add angle term 'ijk' if 'i' > 'k'
			if (!hasAngle(i, j, k)) addAngle(i, j, k);

			// Loop over bonds 'kl'
			for (int klIndex = 0; klIndex < k->nBonds(); ++klIndex)
			{
				// Get bond 'kl' and check for 'kl' == 'jk'
				kl = k->bond(klIndex);
				if (kl == jk) continue;

				// Get atom 'l'
				l = kl->partner(k);

				// Attempt to add angle term 'jkl'
				if (!hasAngle(j, k, l)) addAngle(j, k, l);

				// If the torsion i-j-k-l doesn't already exist, add it now.
				if (!hasTorsion(i, j, k, l)) addTorsion(ij->partner(j), j, jk->partner(j), kl->partner(jk->partner(j)));
			}
		}
	}

	// Check existing angle terms for any that are invalid
	int anglePos = 0, origNAngles = angles_.nItems();
	for (int n=0; n<origNAngles; ++n)
	{
		// Grab the angle
		SpeciesAngle* angle = angles_[anglePos];

		// Check if this angle is still valid (i.e. i-j and j-k are owned by the Species and still bound)
		if ((!atoms_.contains(angle->i())) || (!atoms_.contains(angle->j())) || (!atoms_.contains(angle->k())))
		{
			angles_.removeWithReorder(anglePos);
			continue;
		}
		if ((!hasBond(angle->i(), angle->j())) || (!hasBond(angle->j(), angle->k())))
		{
			angles_.removeWithReorder(anglePos);
			continue;
		}

		// OK, so increase position and continue
		++anglePos;
	}

	int torsionPos = 0, origNTorsions = torsions_.nItems();
	for (int n=0; n<origNTorsions; ++n)
	{
		// Grab the angle
		SpeciesTorsion* torsion = torsions_[torsionPos];

		// Check if this torsion is still valid (i.e. i-j, j-k, and k-l are owned by the Species and still bound)
		if ((!atoms_.contains(torsion->i())) || (!atoms_.contains(torsion->j())) || (!atoms_.contains(torsion->k())) || (!atoms_.contains(torsion->l())))
		{
			torsions_.removeWithReorder(torsionPos);
			continue;
		}
		if ((!hasBond(torsion->i(), torsion->j())) || (!hasBond(torsion->j(), torsion->k())) || (!hasBond(torsion->k(), torsion->l())))
		{
			torsions_.removeWithReorder(torsionPos);
			continue;
		}

		// OK, so increase position and continue
		++torsionPos;
	}
}

/*
 * Public
 */

// Add new SpeciesBond definition (from SpeciesAtoms*)
SpeciesBond* Species::addBond(SpeciesAtom* i, SpeciesAtom* j)
{
	// Check ownership of these Atoms
	if (!atoms_.contains(i))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'i' is not owned by Species '%s' in Species::addBond().\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(j))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'j' is not owned by Species '%s' in Species::addBond().\n", name_.get());
		return NULL;
	}

	// Check for existence of Bond already
	if (hasBond(i, j))
	{
		Messenger::warn("Refused to add a new SpeciesBond between atoms %i and %i in Species '%s' since it already exists.\n", i->userIndex(), j->userIndex(), name_.get());
		return NULL;
	}
	
	// OK to add new Bond
	SpeciesBond* b = bonds_.add();
	b->setParent(this);
	b->setAtoms(i, j);
	i->addBond(b);
	j->addBond(b);

	// Update higher-order connectivity?
	if (autoUpdateIntramolecularTerms_) updateIntramolecularTerms();

	++version_;

	return b;
}

// Add new SpeciesBond definition
SpeciesBond* Species::addBond(int i, int j)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addBond() is out of range (%i) for Species '%s'.\n", i, name_.get());
		return NULL;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addBond() is out of range (%i) for Species '%s'.\n", j, name_.get());
		return NULL;
	}

	return addBond(atoms_[i], atoms_[j]);
}

// Remove bond between specified SpeciesAtoms*
bool Species::removeBond(SpeciesAtom* i, SpeciesAtom* j)
{
	// Find the bond
	DynamicArrayIterator<SpeciesBond> bondIterator(bonds_);
	while (SpeciesBond* b = bondIterator.iterate()) if (b->matches(i, j))
	{
		bonds_.removeWithReorder(bondIterator.currentIndex());
		++version_;

		// Update higher order intramolecular terms?
		if (autoUpdateIntramolecularTerms_) updateIntramolecularTerms();
	}

	return false;
}

// Reconnect existing SpeciesBond
bool Species::reconnectBond(SpeciesBond* bond, SpeciesAtom* i, SpeciesAtom* j)
{
	// Check ownership of the SpeciesBond
	if (!bonds_.contains(bond)) return Messenger::error("BAD_OWNERSHIP - Bond is not owned by Species '%s' in Species::reconnectBond().\n", name_.get());

	// Check ownership of these Atoms
	if (!atoms_.contains(i)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'i' is not owned by Species '%s' in Species::reconnectBond().\n", name_.get());
	if (!atoms_.contains(j)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'j' is not owned by Species '%s' in Species::reconnectBond().\n", name_.get());

	// If a Bond already exists between these Atoms, refuse to add it
	if (hasBond(i, j)) return Messenger::error("A bond between atoms %i and %i already exists in Species '%s', so refusing to reconnect a duplicate.\n", i->userIndex(), j->userIndex(), name_.get());

	// Disconnect the existing Bond
	// TODO Shouldn't this be the SpeciesAtoms specified in the bond, rather than the "new" i and j?
	i->removeBond(bond);
	j->removeBond(bond);

	// Add the new Bond
	bond->setAtoms(i, j);

	++version_;

	return true;
}

// Reconnect existing SpeciesBond
bool Species::reconnectBond(SpeciesBond* bond, int i, int j)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::reconnectBond() is out of range (%i) for Species '%s'.\n", i, name_.get());
		return false;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::reconnectBond() is out of range (%i) for Species '%s'.\n", j, name_.get());
		return false;
	}

	return reconnectBond(bond, atoms_[i], atoms_[j]);
}

// Return number of SpeciesBonds defined
int Species::nBonds() const
{
	return bonds_.nItems();
}

// Return array of SpeciesBond
DynamicArray<SpeciesBond>& Species::bonds()
{
	return bonds_;
}

// Return array of SpeciesBond (const)
const DynamicArray<SpeciesBond>& Species::constBonds() const
{
	return bonds_;
}

// Return whether SpeciesBond between specified SpeciesAtoms exists
bool Species::hasBond(SpeciesAtom* i, SpeciesAtom* j) const
{
	DynamicArrayConstIterator<SpeciesBond> bondIterator(bonds_);
	while (const SpeciesBond* b = bondIterator.iterate()) if (b->matches(i, j)) return true;

	return false;
}

// Return the SpeciesBond between the specified SpeciesAtoms
SpeciesBond* Species::bond(SpeciesAtom* i, SpeciesAtom* j)
{
	DynamicArrayIterator<SpeciesBond> bondIterator(bonds_);
	while (SpeciesBond* b = bondIterator.iterate()) if (b->matches(i, j)) return b;

	return NULL;
}

// Return the SpeciesBond between the specified SpeciesAtom indices
SpeciesBond* Species::bond(int i, int j)
{
	if ((i < 0) || (i >= nAtoms()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::hasBond() is out of range (%i) for Species '%s'\n", i, name_.get());
		return NULL;
	}
	if ((j < 0) || (j >= nAtoms()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::hasBond() is out of range (%i) for Species '%s'\n", j, name_.get());
		return NULL;
	}

	return bond(atoms_[i], atoms_[j]);
}

// Return the SpeciesBond between the specified SpeciesAtoms (const)
const SpeciesBond* Species::constBond(SpeciesAtom* i, SpeciesAtom* j) const
{
	DynamicArrayConstIterator<SpeciesBond> bondIterator(bonds_);
	while (const SpeciesBond* b = bondIterator.iterate()) if (b->matches(i, j)) return b;

	return NULL;
}

// Add missing bonds
void Species::addMissingBonds(double tolerance)
{
	Vec3<double> vij;
	double radiusI;
	SpeciesAtom** atoms = atoms_.array();
	for (int indexI = 0; indexI < nAtoms()-1; ++indexI)
	{
		// Get SpeciesAtom 'i' and its radius
		SpeciesAtom* i = atoms[indexI];
		radiusI = AtomicRadius::radius(i->element());
		for (int indexJ = indexI+1; indexJ < nAtoms(); ++indexJ)
		{
			// Get SpeciesAtom 'j'
			SpeciesAtom* j = atoms[indexJ];

			// If the two atoms are already bound, continue
			if (i->hasBond(j)) continue;

			// Calculate distance between atoms
			vij = j->r() - i->r();

			// Compare distance to sum of atomic radii (multiplied by tolerance factor)
			if (vij.magnitude() <= (radiusI + AtomicRadius::radius(j->element()))*tolerance) addBond(i, j);
		}
	}

	// May now require new higher-order terms
	if (autoUpdateIntramolecularTerms_) updateIntramolecularTerms();
}

// Add new SpeciesAngle definition (from supplied SpeciesAtom pointers)
SpeciesAngle* Species::addAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k)
{
	// Check ownership of these Atoms
	if (!atoms_.contains(i))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'i' is not owned by Species '%s' in Species::addAngle().\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(j))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'j' is not owned by Species '%s' in Species::addAngle().\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(k))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'k' is not owned by Species '%s' in Species::addAngle().\n", name_.get());
		return NULL;
	}

	// Check for existence of Angle already
	if (hasAngle(i, j, k))
	{
		Messenger::warn("Refused to add a new Angle between atoms %i, %i and %i in Species '%s' since it already exists.\n", i->userIndex(), j->userIndex(), k->userIndex(), name_.get());
		return NULL;
	}

	// OK to add new angle
	SpeciesAngle* a = angles_.add();
	a->setParent(this);
	a->setAtoms(i, j, k);

	++version_;

	return a;
}

// Add new SpeciesAngle definition
SpeciesAngle* Species::addAngle(int i, int j, int k)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", i, name_.get());
		return NULL;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", j, name_.get());
		return NULL;
	}
	if ((k < 0) || (k >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'k' supplied to Species::addAngle() is out of range (%i) for Species '%s'\n", k, name_.get());
		return NULL;
	}

	return addAngle(atoms_[i], atoms_[j], atoms_[k]);
}

// Reconnect existing SpeciesAngle
bool Species::reconnectAngle(SpeciesAngle* angle, SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k)
{
	// Check ownership of the SpeciesAngle
	if (!angles_.contains(angle)) return Messenger::error("BAD_OWNERSHIP - SpeciesAngle is not owned by Species '%s' in Species::reconnectAngle().\n", name_.get());

	// Check ownership of these Atoms
	if (!atoms_.contains(i)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'i' is not owned by Species '%s' in Species::reconnectAngle().\n", name_.get());
	if (!atoms_.contains(j)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'j' is not owned by Species '%s' in Species::reconnectAngle().\n", name_.get());
	if (!atoms_.contains(k)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'k' is not owned by Species '%s' in Species::reconnectAngle().\n", name_.get());

	// If an angle already exists between these Atoms, refuse to add it
	if (hasAngle(i, j, k)) return Messenger::error("An angle between atoms %i-%i-%i already exists in Species '%s', so refusing to reconnect a duplicate.\n", i->userIndex(), j->userIndex(), k->userIndex(), name_.get());

	// Set the new angle atoms
	angle->setAtoms(i, j, k);

	++version_;

	return true;
}

// Reconnect existing SpeciesAngle
bool Species::reconnectAngle(SpeciesAngle* angle, int i, int j, int k)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::reconnectAngle() is out of range (%i) for Species '%s'\n", i, name_.get());
		return false;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::reconnectAngle() is out of range (%i) for Species '%s'\n", j, name_.get());
		return false;
	}
	if ((k < 0) || (k >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'k' supplied to Species::reconnectAngle() is out of range (%i) for Species '%s'\n", k, name_.get());
		return false;
	}

	return reconnectAngle(angle, atoms_[i], atoms_[j], atoms_[k]);
}

// Return number of SpeciesAngles defined
int Species::nAngles() const
{
	return angles_.nItems();
}

// Return array of SpeciesAngle
DynamicArray<SpeciesAngle>& Species::angles()
{
	return angles_;
}

// Return array of SpeciesAngle (const)
const DynamicArray<SpeciesAngle>& Species::constAngles() const
{
	return angles_;
}

// Return whether SpeciesAngle between SpeciesAtoms exists
bool Species::hasAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k) const
{
	DynamicArrayConstIterator<SpeciesAngle> angleIterator(angles_);
	while (const SpeciesAngle* a = angleIterator.iterate()) if (a->matches(i, j, k)) return true;

	return false;
}

// Add new SpeciesTorsion definition (from supplied SpeciesAtom pointers)
SpeciesTorsion* Species::addTorsion(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l)
{
	// Check ownership of these Atoms
	if (!atoms_.contains(i))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'i' is not owned by Species '%s' in Species::addTorsion.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(j))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'j' is not owned by Species '%s' in Species::addTorsion.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(k))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'k' is not owned by Species '%s' in Species::addTorsion.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(l))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'l' is not owned by Species '%s' in Species::addTorsion.\n", name_.get());
		return NULL;
	}

	// Check for existence of Torsion already
	if (hasTorsion(i, j, k, l))
	{
		Messenger::warn("Refused to add a new Torsion between atoms %i, %i, %i and %i in Species '%s' since it already exists.\n", i->userIndex(), j->userIndex(), k->userIndex(), l->userIndex(), name_.get());
		return NULL;
	}

	// OK to add new torsion
	SpeciesTorsion* t = torsions_.add();
	t->setParent(this);
	t->setAtoms(i, j, k, l);

	++version_;

	return t;
}

// Add new SpeciesTorsion definition
SpeciesTorsion* Species::addTorsion(int i, int j, int k, int l)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addTorsion() is out of range (%i) for Species '%s'\n", i, name_.get());
		return NULL;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addTorsion() is out of range (%i) for Species '%s'\n", j, name_.get());
		return NULL;
	}
	if ((k < 0) || (k >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'k' supplied to Species::addTorsion() is out of range (%i) for Species '%s'\n", k, name_.get());
		return NULL;
	}
	if ((l < 0) || (l >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'l' supplied to Species::addTorsion() is out of range (%i) for Species '%s'\n", l, name_.get());
		return NULL;
	}

	return addTorsion(atoms_[i], atoms_[j], atoms_[k], atoms_[l]);
}

// Reconnect existing SpeciesTorsion
bool Species::reconnectTorsion(SpeciesTorsion* torsion, SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l)
{
	// Check ownership of the SpeciesTorsion
	if (!torsions_.contains(torsion)) return Messenger::error("BAD_OWNERSHIP - SpeciesTorsion is not owned by Species '%s' in Species::reconnectTorsion().\n", name_.get());

	// Check ownership of these Atoms
	if (!atoms_.contains(i)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'i' is not owned by Species '%s' in Species::reconnectTorsion().\n", name_.get());
	if (!atoms_.contains(j)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'j' is not owned by Species '%s' in Species::reconnectTorsion().\n", name_.get());
	if (!atoms_.contains(k)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'k' is not owned by Species '%s' in Species::reconnectTorsion().\n", name_.get());
	if (!atoms_.contains(l)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'l' is not owned by Species '%s' in Species::reconnectTorsion().\n", name_.get());

	// If a torsion already exists between these Atoms, refuse to add it
	if (hasTorsion(i, j, k, l)) return Messenger::error("A torsion between atoms %i-%i-%i-%i already exists in Species '%s', so refusing to reconnect a duplicate.\n", i->userIndex(), j->userIndex(), k->userIndex(), l->userIndex(), name_.get());

	// Set the new angle atoms
	torsion->setAtoms(i, j, k, l);

	++version_;

	return true;
}

// Reconnect existing SpeciesTorsion
bool Species::reconnectTorsion(SpeciesTorsion* torsion, int i, int j, int k, int l)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::reconnectTorsion() is out of range (%i) for Species '%s'\n", i, name_.get());
		return false;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::reconnectTorsion() is out of range (%i) for Species '%s'\n", j, name_.get());
		return false;
	}
	if ((k < 0) || (k >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'k' supplied to Species::reconnectTorsion() is out of range (%i) for Species '%s'\n", k, name_.get());
		return false;
	}
	if ((l < 0) || (l >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'l' supplied to Species::reconnectTorsion() is out of range (%i) for Species '%s'\n", l, name_.get());
		return false;
	}

	return reconnectTorsion(torsion, atoms_[i], atoms_[j], atoms_[k], atoms_[l]);
}

// Return number of SpeciesTorsions defined
int Species::nTorsions() const
{
	return torsions_.nItems();
}

// Return array of SpeciesTorsions
DynamicArray<SpeciesTorsion>& Species::torsions()
{
	return torsions_;
}

// Return array of SpeciesTorsions (const)
const DynamicArray<SpeciesTorsion>& Species::constTorsions() const
{
	return torsions_;
}

// Return whether SpeciesTorsion between SpeciesAtoms exists
bool Species::hasTorsion(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l) const
{
	DynamicArrayConstIterator<SpeciesTorsion> torsionIterator(torsions_);
	while (const SpeciesTorsion* t = torsionIterator.iterate()) if (t->matches(i, j, k, l)) return true;

	return false;
}

// Add new SpeciesImproper definition (from SpeciesAtom*)
SpeciesImproper* Species::addImproper(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l)
{
	// Check ownership of these Atoms
	if (!atoms_.contains(i))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'i' is not owned by Species '%s' in Species::addImproper.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(j))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'j' is not owned by Species '%s' in Species::addImproper.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(k))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'k' is not owned by Species '%s' in Species::addImproper.\n", name_.get());
		return NULL;
	}
	if (!atoms_.contains(l))
	{
		Messenger::print("BAD_OWNERSHIP - SpeciesAtom 'l' is not owned by Species '%s' in Species::addImproper.\n", name_.get());
		return NULL;
	}

	// Check for existence of Improper already
	if (hasImproper(i, j, k, l))
	{
		Messenger::warn("Refused to add a new Improper between atoms %i, %i, %i and %i in Species '%s' since it already exists.\n", i->userIndex(), j->userIndex(), k->userIndex(), l->userIndex(), name_.get());
		return NULL;
	}

	// OK to add new improper
	SpeciesImproper* imp = impropers_.add();
	imp->setParent(this);
	imp->setAtoms(i, j, k, l);

	++version_;

	return imp;
}

// Add new SpeciesImproper definition
SpeciesImproper* Species::addImproper(int i, int j, int k, int l)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::addImproper() is out of range (%i) for Species '%s'\n", i, name_.get());
		return NULL;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::addImproper() is out of range (%i) for Species '%s'\n", j, name_.get());
		return NULL;
	}
	if ((k < 0) || (k >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'k' supplied to Species::addImproper() is out of range (%i) for Species '%s'\n", k, name_.get());
		return NULL;
	}
	if ((l < 0) || (l >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'l' supplied to Species::addImproper() is out of range (%i) for Species '%s'\n", l, name_.get());
		return NULL;
	}

	return addImproper(atoms_[i], atoms_[j], atoms_[k], atoms_[l]);
}


// Reconnect existing SpeciesImproper
bool Species::reconnectImproper(SpeciesImproper* improper, SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l)
{
	// Check ownership of the SpeciesImproper
	if (!impropers_.contains(improper)) return Messenger::error("BAD_OWNERSHIP - SpeciesImproper is not owned by Species '%s' in Species::reconnectImproper().\n", name_.get());

	// Check ownership of these Atoms
	if (!atoms_.contains(i)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'i' is not owned by Species '%s' in Species::reconnectImproper().\n", name_.get());
	if (!atoms_.contains(j)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'j' is not owned by Species '%s' in Species::reconnectImproper().\n", name_.get());
	if (!atoms_.contains(k)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'k' is not owned by Species '%s' in Species::reconnectImproper().\n", name_.get());
	if (!atoms_.contains(l)) return Messenger::error("BAD_OWNERSHIP - SpeciesAtom 'l' is not owned by Species '%s' in Species::reconnectImproper().\n", name_.get());

	// If a improper already exists between these Atoms, refuse to add it
	if (hasImproper(i, j, k, l)) return Messenger::error("A improper between atoms %i-%i-%i-%i already exists in Species '%s', so refusing to reconnect a duplicate.\n", i->userIndex(), j->userIndex(), k->userIndex(), l->userIndex(), name_.get());

	// Set the new angle atoms
	improper->setAtoms(i, j, k, l);

	++version_;

	return true;
}

// Reconnect existing SpeciesImproper
bool Species::reconnectImproper(SpeciesImproper* improper, int i, int j, int k, int l)
{
	if ((i < 0) || (i >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'i' supplied to Species::reconnectImproper() is out of range (%i) for Species '%s'\n", i, name_.get());
		return false;
	}
	if ((j < 0) || (j >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'j' supplied to Species::reconnectImproper() is out of range (%i) for Species '%s'\n", j, name_.get());
		return false;
	}
	if ((k < 0) || (k >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'k' supplied to Species::reconnectImproper() is out of range (%i) for Species '%s'\n", k, name_.get());
		return false;
	}
	if ((l < 0) || (l >= atoms_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Internal index 'l' supplied to Species::reconnectImproper() is out of range (%i) for Species '%s'\n", l, name_.get());
		return false;
	}

	return reconnectImproper(improper, atoms_[i], atoms_[j], atoms_[k], atoms_[l]);
}

// Return number of SpeciesImproper defined
int Species::nImpropers() const
{
	return impropers_.nItems();
}

// Return array of SpeciesImproper
DynamicArray<SpeciesImproper>& Species::impropers()
{
	return impropers_;
}

// Return array of SpeciesImproper (const)
const DynamicArray<SpeciesImproper>& Species::constImpropers() const
{
	return impropers_;
}

// Return whether SpeciesImproper between SpeciesAtoms exists
bool Species::hasImproper(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l) const
{
	DynamicArrayConstIterator<SpeciesImproper> improperIterator(impropers_);
	while (const SpeciesImproper* imp = improperIterator.iterate()) if (imp->matches(i, j, k, l)) return true;

	return false;
}

// Return the SpeciesImproper between the specified SpeciesAtoms (if it exists)
SpeciesImproper* Species::improper(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l)
{
	DynamicArrayIterator<SpeciesImproper> improperIterator(impropers_);
	while (SpeciesImproper* imp = improperIterator.iterate()) if (imp->matches(i, j, k, l)) return imp;

	return NULL;
}

// Return whether the attached atoms lists have been created
bool Species::attachedAtomListsGenerated() const
{
	return attachedAtomListsGenerated_;
}

// Generate attached SpeciesAtom lists for all intramolecular terms
void Species::generateAttachedAtomLists()
{
	// Bonds
	DynamicArrayIterator<SpeciesBond> bondIterator(bonds_);
	while (SpeciesBond* b = bondIterator.iterate()) 
	{
		// Select all Atoms attached to Atom 'i', excluding the Bond as a path
		clearAtomSelection();
		selectFromAtom(b->i(), b);

		// If the list now contains Atom j, the two atoms are present in a cycle of some sort, and we can only add the Atom 'i' itself
		// In that case we can also finish the list for Atom 'j', and continue the loop.
		if (selectedAtoms_.contains(b->j()))
		{
			Messenger::printVerbose("Bond between Atoms %i-%i is present in a cycle, so a minimal set of attached atoms will be used.\n", b->i()->userIndex(), b->j()->userIndex());
			b->setAttachedAtoms(0, b->i());
			b->setAttachedAtoms(1, b->j());
			b->setInCycle(true);
			continue;
		}
		else b->setAttachedAtoms(0, selectedAtoms_);

		// Select all Atoms attached to Atom 'i', excluding the Bond as a path
		clearAtomSelection();
		selectFromAtom(b->j(), b);
		b->setAttachedAtoms(1, selectedAtoms_);
	}

	// Angles - termini are 'i' and 'k'
	DynamicArrayIterator<SpeciesAngle> angleIterator(angles_);
	while (SpeciesAngle* a = angleIterator.iterate()) 
	{
		// Grab relevant Bonds (if they exist)
		SpeciesBond* ji = a->j()->hasBond(a->i());
		SpeciesBond* jk = a->j()->hasBond(a->k());

		// Select all Atoms attached to Atom 'i', excluding the Bond ji as a path
		clearAtomSelection();
		selectFromAtom(a->i(), ji, jk);

		// Remove Atom 'j' from the list if it's there
		selectedAtoms_.remove(a->j());

		// If the list now contains Atom k, the two atoms are present in a cycle of some sort, and we can only add the Atom 'i' itself
		// In that case we can also finish the list for Atom 'k', and continue the loop.
		if (selectedAtoms_.contains(a->k()))
		{
			Messenger::printVerbose("Angle between Atoms %i-%i-%i is present in a cycle, so a minimal set of attached atoms will be used.\n", a->i()->userIndex(), a->j()->userIndex(), a->k()->userIndex());
			a->setAttachedAtoms(0, a->i());
			a->setAttachedAtoms(1, a->k());
			a->setInCycle(true);
			continue;
		}
		else a->setAttachedAtoms(0, selectedAtoms_);

		// Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
		clearAtomSelection();
		selectFromAtom(a->k(), ji, jk);

		// Remove Atom 'j' from the list if it's there
		selectedAtoms_.remove(a->j());

		a->setAttachedAtoms(1, selectedAtoms_);
	}

	// Torsions - termini are 'j' and 'k'
	DynamicArrayIterator<SpeciesTorsion> torsionIterator(torsions_);
	while (SpeciesTorsion* t = torsionIterator.iterate()) 
	{
		// Grab relevant Bond (if it exists)
		SpeciesBond* jk = t->j()->hasBond(t->k());

		// Select all Atoms attached to Atom 'j', excluding the Bond ji as a path
		clearAtomSelection();
		selectFromAtom(t->j(), jk);

		// Remove Atom 'j' from the list
		selectedAtoms_.remove(t->j());

		// If the list now contains Atom k, the two atoms are present in a cycle of some sort, and we can only add the Atom 'i'
		if (selectedAtoms_.contains(t->k()))
		{
			Messenger::printVerbose("Torsion between Atoms %i-%i-%i-%i is present in a cycle, so a minimal set of attached atoms will be used.\n", t->i()->userIndex(), t->j()->userIndex(), t->k()->userIndex(), t->l()->userIndex());
			t->setAttachedAtoms(0, t->i());
			t->setAttachedAtoms(1, t->l());
			t->setInCycle(true);
			continue;
		}
		else t->setAttachedAtoms(0, selectedAtoms_);

		// Select all Atoms attached to Atom 'k', excluding the Bond jk as a path
		clearAtomSelection();
		selectFromAtom(t->k(), jk);

		// Remove Atom 'k' from the list
		selectedAtoms_.remove(t->k());

		t->setAttachedAtoms(1, selectedAtoms_);
	}

	attachedAtomListsGenerated_ = true;
}

// Detach master term links for all interaction types, copying parameters to local SpeciesIntra
void Species::detachFromMasterTerms()
{
	DynamicArrayIterator<SpeciesBond> bondIterator(bonds_);
	while (SpeciesBond* b = bondIterator.iterate()) b->detachFromMasterIntra();

	DynamicArrayIterator<SpeciesAngle> angleIterator(angles_);
	while (SpeciesAngle* a = angleIterator.iterate()) a->detachFromMasterIntra();

	DynamicArrayIterator<SpeciesTorsion> torsionIterator(torsions_);
	while (SpeciesTorsion* t = torsionIterator.iterate()) t->detachFromMasterIntra();
}
