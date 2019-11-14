/*
	*** Species Definition - Forcefield
	*** src/classes/species_ff.cpp
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
#include "data/ff.h"
#include "base/sysfunc.h"

/*
 * Private
 */

// Add missing higher order intramolecular terms from current bond connectivity
void Species::completeIntramolecularTerms()
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
			if ((i > k) && (!hasAngle(i, j, k))) addAngle(i, j, k);

			// Loop over bonds 'kl'
			for (int klIndex = 0; klIndex < k->nBonds(); ++klIndex)
			{
				// Get bond 'kl' and check for 'kl' == 'jk'
				kl = k->bond(klIndex);
				if (kl == jk) continue;

				// Get atom 'l'
				l = kl->partner(k);

				// Attempt to add angle term 'jkl' if 'j' > 'l'
				if ((j > l) && (!hasAngle(j, k, l))) addAngle(j, k, l);

				// If the torsion i-j-k-l doesn't already exist, add it now.
				if (!hasTorsion(i,  j, k, l)) addTorsion(ij->partner(j), j, jk->partner(j), kl->partner(jk->partner(j)));
			}
		}
	}
}

/*
 * Public
 */

// Set Forcefield to source terms from
void Species::setForcefield(Forcefield* ff)
{
	forcefield_ = ff;
}

// Return Forcefield to source terms from
Forcefield* Species::forcefield() const
{
	return forcefield_;
}

// Apply terms from source Forcefield
bool Species::applyForcefieldTerms(CoreData& coreData)
{
	if (!forcefield_) return Messenger::error("No forcefield set in Species '%s', so can't apply terms.\n", name());

	// Apply the specified Forcefield
	if (!forcefield_->assignAtomTypes(this, coreData, false)) return false;
	if (!forcefield_->assignIntramolecular(this, true, true, true, true, true)) return false;

	return true;
}
