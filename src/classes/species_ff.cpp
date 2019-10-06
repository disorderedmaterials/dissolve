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
	// Loop over atoms 'j'
	ListIterator<SpeciesAtom> jIterator(atoms_);
	while (SpeciesAtom* j = jIterator.iterate())
	{
		// Loop over bonds 'ij'
		for (RefListItem<SpeciesBond>* rij = j->bonds().first(); rij != NULL; rij = rij->next())
		{
			SpeciesBond* ij = rij->item();

			// Loop over bonds 'jk'
			for (RefListItem<SpeciesBond>* rjk = rij->next(); rjk != NULL; rjk = rjk->next())
			{
				SpeciesBond* jk = rjk->item();

				// If this angle doesn't already exist, add it now.
				SpeciesAngle* ijk = hasAngle(ij->partner(j), j, jk->partner(j));
				if (!ijk)
				{
					ijk = addAngle(ij->partner(j), j, jk->partner(j));
					ijk->setForm(SpeciesAngle::nAngleFunctions);
				}

				// Add torsions - loop over bonds 'kl'
				for (RefListItem<SpeciesBond>* rkl = jk->partner(j)->bonds().first(); rkl != NULL; rkl = rkl->next())
				{
					SpeciesBond* kl = rkl->item();

					// Exclude jk == kl
					if (jk == kl) continue;

					// If this torsion doesn't already exist, add it now.
					SpeciesTorsion* ijkl = hasTorsion(ij->partner(j), j, jk->partner(j), kl->partner(jk->partner(j)));
					if (!ijkl)
					{
						ijkl = addTorsion(ij->partner(j), j, jk->partner(j), kl->partner(jk->partner(j)));
						ijkl->setForm(SpeciesTorsion::nTorsionFunctions);
					}
				}
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

	// First, create (missing) higher-order intramolecular definitions
	completeIntramolecularTerms();

	// Apply the specified Forcefield
	if (!forcefield_->assignAtomTypes(this, coreData, false)) return false;
	if (!forcefield_->assignIntramolecular(this, true, true, true, true)) return false;

	return true;
}
