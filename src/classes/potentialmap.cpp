/*
	*** PotentialMap Definition
	*** src/classes/potentialmap.cpp
	Copyright T. Youngs 2012-2018

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

#include "classes/potentialmap.h"
#include "classes/pairpotential.h"
#include "classes/species.h"
#include "classes/molecule.h"
#include "classes/atomtype.h"
#include "base/messenger.h"
#include "base/parameters.h"
#include "math/constants.h"
#include <math.h>
#include <new>
using namespace std;

// Constructor
PotentialMap::PotentialMap()
{
}

// Destructor
PotentialMap::~PotentialMap()
{
}

// Clear all data
void PotentialMap::clear()
{
	potentialMatrix_.clear();
}

/*
 * Source Parameters
 */

// Initialise maps
bool PotentialMap::initialise(const List<AtomType>& masterAtomTypes, const List<PairPotential>& pairPotentials, double pairPotentialRange)
{
	// Clear old data first
	clear();

	// Create PairPotential matrix
	nTypes_ = masterAtomTypes.nItems();
	potentialMatrix_.initialise(nTypes_, nTypes_);

	// Loop over defined PairPotentials
	int indexI, indexJ;
	for (PairPotential* pot = pairPotentials.first(); pot != NULL; pot = pot->next)
	{
		indexI = pot->atomTypeI()->index();
		indexJ = pot->atomTypeJ()->index();
		if (indexI == -1)
		{
			printf("INTERNAL_ERROR - Couldn't find AtomType '%s' in typeIndex.\n", pot->atomTypeI()->name());
			return false;
		}
		if (indexJ == -1)
		{
			printf("INTERNAL_ERROR - Couldn't find AtomType '%s' in typeIndex.\n", pot->atomTypeJ()->name());
			return false;
		}
		
		// Store PairPotential pointer
		if (indexI == indexJ)
		{
			Messenger::print("Linking self-interaction PairPotential for '%s' (index %i,%i in matrix).\n", pot->atomTypeI()->name(), indexI, indexJ);
			potentialMatrix_.ref(indexI,indexI) = pot;
		}
		else
		{
			Messenger::print("Linking PairPotential between '%s' and '%s' (indices %i,%i and %i,%i in matrix).\n", pot->atomTypeI()->name(), pot->atomTypeJ()->name(), indexI, indexJ, indexJ, indexI);
			potentialMatrix_.ref(indexI, indexJ) = pot;
			potentialMatrix_.ref(indexJ, indexI) = pot;
		}
	}

	// Store potential range
	range_ = pairPotentialRange;
	
	return true;
}

// Return PairPotential range
double PotentialMap::range() const
{
	return range_;
}

/*
 * Energy / Force
 */

// Return energy between Atom types at squared distance specified
double PotentialMap::energy(const Atom* i, const Atom* j, double r) const
{
#ifdef CHECKS
	if ((i->masterTypeIndex() < 0) || (i->masterTypeIndex() >= nTypes_))
	{
		Messenger::print("OUT_OF_RANGE - Type index of atom i (%i) passed to PotentialMap::energy() is out of range (nTypes_ = %i).\n", i->masterTypeIndex(), nTypes_);
		return 0.0;
	}
	if ((j->masterTypeIndex() < 0) || (j->masterTypeIndex() >= nTypes_))
	{
		Messenger::print("OUT_OF_RANGE - Type index of atom j (%i) passed to PotentialMap::energy() is out of range (nTypes_ = %i).\n", j->masterTypeIndex(), nTypes_);
		return 0.0;
	}
	if (r < 0.0)
	{
		Messenger::print("OUT_OF_RANGE - Distance passed to PotentialMap::energy() is negative (%f).\n", r);
		return 0.0;
	}
#endif
	// Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the interpolated potential
	PairPotential* pp = potentialMatrix_.value(i->masterTypeIndex(), j->masterTypeIndex());
	if (pp->includeCoulomb()) return pp->energy(r);
	else return (pp->energy(r) + pp->analyticCoulombEnergy(i->charge() * j->charge(), r));
}

// Return analytic energy between Atom types at squared distance specified
double PotentialMap::analyticEnergy(const Atom* i, const Atom* j, double r) const
{
#ifdef CHECKS
	if ((i->masterTypeIndex() < 0) || (i->masterTypeIndex() >= nTypes_))
	{
		Messenger::print("OUT_OF_RANGE - Type index of atom i (%i) passed to PotentialMap::analyticEnergy() is out of range (nTypes_ = %i).\n", i->masterTypeIndex(), nTypes_);
		return 0.0;
	}
	if ((j->masterTypeIndex() < 0) || (j->masterTypeIndex() >= nTypes_))
	{
		Messenger::print("OUT_OF_RANGE - Type index of atom j (%i) passed to PotentialMap::analyticEnergy() is out of range (nTypes_ = %i).\n", j->masterTypeIndex(), nTypes_);
		return 0.0;
	}
	if (r < 0.0)
	{
		Messenger::print("OUT_OF_RANGE - Distance passed to PotentialMap::analyticEnergy() is negative (%f).\n", r);
		return 0.0;
	}
#endif
	return potentialMatrix_.value(i->masterTypeIndex(), j->masterTypeIndex())->analyticEnergy(r);
}

// Return force between Atom types at squared distance specified
double PotentialMap::force(const Atom* i, const Atom* j, double r) const
{
#ifdef CHECKS
	if ((i->masterTypeIndex() < 0) || (i->masterTypeIndex() >= nTypes_))
	{
		Messenger::print("OUT_OF_RANGE - Type index of atom i (%i) passed to PotentialMap::force() is out of range (nTypes_ = %i).\n", i->masterTypeIndex(), nTypes_);
		return 0.0;
	}
	if ((j->masterTypeIndex() < 0) || (j->masterTypeIndex() >= nTypes_))
	{
		Messenger::print("OUT_OF_RANGE - Type index of atom j (%i) passed to PotentialMap::force() is out of range (nTypes_ = %i).\n", j->masterTypeIndex(), nTypes_);
		return 0.0;
	}
	if (r < 0.0)
	{
		Messenger::print("OUT_OF_RANGE - Distance passed to PotentialMap::force() is negative (%f).\n", r);
		return 0.0;
	}
#endif
	// Check to see whether Coulomb terms should be calculated from atomic charges, rather than them being included in the interpolated potential
	PairPotential* pp = potentialMatrix_.value(i->masterTypeIndex(), j->masterTypeIndex());
	if (pp->includeCoulomb()) return pp->force(r);
	else return (pp->force(r) + pp->analyticCoulombForce(i->charge() * j->charge(), r));
}

// Return analytic force between Atom types at squared distance specified
double PotentialMap::analyticForce(const Atom* i, const Atom* j, double r) const
{
#ifdef CHECKS
	if ((i->masterTypeIndex() < 0) || (i->masterTypeIndex() >= nTypes_))
	{
		Messenger::print("OUT_OF_RANGE - Type index of atom i (%i) passed to PotentialMap::analyticForce() is out of range (nTypes_ = %i).\n", i->masterTypeIndex(), nTypes_);
		return 0.0;
	}
	if ((j->masterTypeIndex() < 0) || (j->masterTypeIndex() >= nTypes_))
	{
		Messenger::print("OUT_OF_RANGE - Type index of atom j (%i) passed to PotentialMap::analyticForce() is out of range (nTypes_ = %i).\n", j->masterTypeIndex(), nTypes_);
		return 0.0;
	}
	if (r < 0.0)
	{
		Messenger::print("OUT_OF_RANGE - Distance passed to PotentialMap::analyticForce() is negative (%f).\n", r);
		return 0.0;
	}
#endif
	return potentialMatrix_.value(i->masterTypeIndex(), j->masterTypeIndex())->analyticForce(r);
}
