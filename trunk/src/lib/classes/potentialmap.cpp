/*
	*** PotentialMap Definition
	*** src/classes/potentialmap.cpp
	Copyright T. Youngs 2012-2013

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

/*!
 * \brief Constructor
 * \details Constructor for PotentialMap. 
 */
PotentialMap::PotentialMap()
{
	potentialMatrix_ = NULL;
	typeMap_ = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for PotentialMap. 
 */
PotentialMap::~PotentialMap()
{
	clear();
}

/*!
 * \brief Clear all data
 */
void PotentialMap::clear()
{
	if (potentialMatrix_ != NULL)
	{
		for (int n=0; n<nTypes_; ++n) delete[] potentialMatrix_[n];
		delete[] potentialMatrix_;
	}
	potentialMatrix_ = NULL;
	if (typeMap_ != NULL) delete[] typeMap_;
	typeMap_ = NULL;
}

/*
// Data
*/

/*!
 * \brief Initialise maps
 */
bool PotentialMap::initialise(const AtomTypeIndex& typeIndex, const List<PairPotential>& pairPotentials, int nAtoms, Molecule* firstMolecule, double pairPotentialRange)
{
	// Clear old data first
	clear();

	// Create Atom->AtomType index
	msg.print("--> Creating Atom->AtomType index for %i Atoms...\n", nAtoms);
	nAtoms_ = nAtoms;
	try
	{
		typeMap_ = new int[nAtoms_];
	}
	catch (bad_alloc& alloc)
	{
		msg.error("Failed to allocate sufficient memory. Exception was : %s\n", alloc.what());
		return FALSE;
	}
	int count = 0;
	int index;
	for (Molecule* mol = firstMolecule; mol != NULL; mol = mol->next)
	{
		for (Atom* i = mol->species()->atoms(); i != NULL; i = i->next)
		{
			// Search for AtomType/Isotope in index
			index = typeIndex.indexOf(i->atomType());
			if (index == -1)
			{
				printf("INTERNAL_ERROR - AtomType for Atom not found in supplied AtomTypeIndex.\n");
				return FALSE;
			}
			typeMap_[count] = index;
			++count;
		}
	}

	// Create PairPotential matrix
	nTypes_ = typeIndex.nItems();
	potentialMatrix_ = new PairPotential**[nTypes_];
	for (int n=0; n<nTypes_; ++n) potentialMatrix_[n] = new PairPotential*[nTypes_];

	// Loop over defined PairPotentials
	int indexI, indexJ;
	for (PairPotential* pot = pairPotentials.first(); pot != NULL; pot = pot->next)
	{
		indexI = typeIndex.indexOf(pot->atomTypeI());
		indexJ = typeIndex.indexOf(pot->atomTypeJ());
		if (indexI == -1)
		{
			printf("INTERNAL_ERROR - Couldn't find AtomType '%s' in typeIndex.\n", pot->atomTypeI()->name());
			return FALSE;
		}
		if (indexJ == -1)
		{
			printf("INTERNAL_ERROR - Couldn't find AtomType '%s' in typeIndex.\n", pot->atomTypeJ()->name());
			return FALSE;
		}
		
		// Store PairPotential pointer
		if (indexI == indexJ)
		{
			msg.print("--> Linking self-interaction PairPotential for '%s' (index %i,%i in matrix).\n", pot->atomTypeI()->name(), indexI, indexJ);
			potentialMatrix_[indexI][indexI] = pot;
		}
		else
		{
			msg.print("--> Linking PairPotential between '%s' and '%s' (indices %i,%i and %i,%i in matrix).\n", pot->atomTypeI()->name(), pot->atomTypeJ()->name(), indexI, indexJ, indexJ, indexI);
			potentialMatrix_[indexI][indexJ] = pot;
			potentialMatrix_[indexJ][indexI] = pot;
		}
	}

	// Store potential range and square
	range_ = pairPotentialRange;
	rangeSquared_ = range_*range_;
	
	return TRUE;
}

/*!
 * \brief Return type index of Atom index supplied
 */
int PotentialMap::type(int index)
{
#ifdef CHECKS
	if ((index < 0) || (index >= nAtoms_))
	{
		 msg.print("OUT_OF_RANGE - Index %i given to PotentialMap::type() is out of range (size of array is %i).\n", index, nAtoms_);
		 return 0;
	}
#endif
	return typeMap_[index];
}

/*!
 * \brief Return PairPotential range
 */
double PotentialMap::range() const
{
	return range_;
}

/*!
 * \brief Return PairPotential range squared
 */
double PotentialMap::rangeSquared() const
{
	return rangeSquared_;
}

/*
// Energy / Force
*/

/*!
 * \brief Return energy between Atom types at squared distance specified
 */
double PotentialMap::energy(int typeI, int typeJ, double distanceSquared) const
{
#ifdef CHECKS
	if ((typeI < 0) || (typeI >= nTypes_))
	{
		msg.print("OUT_OF_RANGE - Type index typeI (%i) passed to PotentialMap::energy() is out of range (nTypes_ = %i).\n", typeI, nTypes_);
		return 0.0;
	}
	if ((typeJ < 0) || (typeJ >= nTypes_))
	{
		msg.print("OUT_OF_RANGE - Type index typeJ (%i) passed to PotentialMap::energy() is out of range (nTypes_ = %i).\n", typeJ, nTypes_);
		return 0.0;
	}
#endif
	return potentialMatrix_[typeI][typeJ]->energyAtRSquared(distanceSquared);
}

/*!
 * \brief Return energy between Atoms at squared distance specified
 */
double PotentialMap::energy(const Atom* i, const Atom* j, double distanceSquared) const
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer i passed to PotentialMap::energy().\n");
		return 0.0;
	}
	if (j == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer j passed to PotentialMap::energy().\n");
		return 0.0;
	}
	if ((i->index() < 0) || (i->index() >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index (%i) for Atom 'i' is out of range in PotentialMap::energy().\n", i->index());
		return 0.0;
	}
	if ((j->index() < 0) || (j->index() >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index (%i) for Atom 'j' is out of range in PotentialMap::energy().\n", i->index());
		return 0.0;
	}
#endif
	return energy(typeMap_[i->index()], typeMap_[j->index()], distanceSquared);
}

/*!
 * \brief Return force between Atom types at squared distance specified
 */
double PotentialMap::force(int typeI, int typeJ, double distanceSquared) const
{
#ifdef CHECKS
	if ((typeI < 0) || (typeI >= nTypes_))
	{
		msg.print("OUT_OF_RANGE - Type index typeI (%i) passed to PotentialMap::force() is out of range (nTypes_ = %i).\n", typeI, nTypes_);
		return 0.0;
	}
	if ((typeJ < 0) || (typeJ >= nTypes_))
	{
		msg.print("OUT_OF_RANGE - Type index typeJ (%i) passed to PotentialMap::force() is out of range (nTypes_ = %i).\n", typeJ, nTypes_);
		return 0.0;
	}
#endif
	return potentialMatrix_[typeI][typeJ]->forceAtRSquared(distanceSquared);
}

/*!
 * \brief Return force between Atoms at squared distance specified
 */
double PotentialMap::force(const Atom* i, const Atom* j, double distanceSquared) const
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer i passed to PotentialMap::force().\n");
		return 0.0;
	}
	if (j == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer j passed to PotentialMap::force().\n");
		return 0.0;
	}
	if ((i->index() < 0) || (i->index() >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index (%i) for Atom 'i' is out of range in PotentialMap::force().\n", i->index());
		return 0.0;
	}
	if ((j->index() < 0) || (j->index() >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index (%i) for Atom 'j' is out of range in PotentialMap::force().\n", i->index());
		return 0.0;
	}
#endif
	return force(typeMap_[i->index()], typeMap_[j->index()], distanceSquared);
}
