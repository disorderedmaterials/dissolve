/*
	*** SpeciesBond Definition
	*** src/classes/speciesbond.cpp
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

#include "classes/speciesbond.h"
#include "classes/speciesatom.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/enumhelpers.h"

// Constructor
SpeciesBond::SpeciesBond() : SpeciesIntra(), ListItem<SpeciesBond>()
{
	parent_ = NULL;
	i_ = NULL;
	j_ = NULL;
	form_ = SpeciesBond::nBondFunctions;
}

// Destructor
SpeciesBond::~SpeciesBond()
{
}

/*
 * SpeciesAtom Information
 */

// Set SpeciesAtoms involved in SpeciesBond
void SpeciesBond::setAtoms(SpeciesAtom* i, SpeciesAtom* j)
{
	i_ = i;
	j_ = j;
#ifdef CHECKS
	if (i_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom i in SpeciesBond::set().\n");
	if (j_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom j in SpeciesBond::set().\n");
#endif
}

// Return first SpeciesAtom involved in SpeciesBond
SpeciesAtom* SpeciesBond::i() const
{
	return i_;
}

// Return second SpeciesAtom involved in SpeciesBond
SpeciesAtom* SpeciesBond::j() const
{
	return j_;
}

// Return the 'other' SpeciesAtom in the SpeciesBond
SpeciesAtom* SpeciesBond::partner(SpeciesAtom* i) const
{
	return (i == i_ ? j_ : i_);
}

// Return index (in parent Species) of first SpeciesAtom
int SpeciesBond::indexI() const
{
#ifdef CHECKS
	if (i_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'i' found in SpeciesBond::indexI(). Returning 0...\n");
		return 0;
	}
#endif
	return i_->index();
}

// Return index (in parent Species) of second SpeciesAtom
int SpeciesBond::indexJ() const
{
#ifdef CHECKS
	if (j_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'j' found in SpeciesBond::indexJ(). Returning 0...\n");
		return 0;
	}
#endif
	return j_->index();
}

// Return whether SpeciesAtoms in Angle match those specified
bool SpeciesBond::matches(SpeciesAtom* i, SpeciesAtom* j) const
{
	if ((i_ == i) && (j_ == j)) return true;
	if ((i_ == j) && (j_ == i)) return true;
	return false;
}

/*
 * Interaction Parameters
 */

// Bond function keywords
const char* BondFunctionKeywords[] = { "Harmonic" };
int BondFunctionNParameters[] = { 2 };

// Convert string to functional form
SpeciesBond::BondFunction SpeciesBond::bondFunction(const char* s)
{
	for (int n=0; n<SpeciesBond::nBondFunctions; ++n) if (DUQSys::sameString(s, BondFunctionKeywords[n])) return (SpeciesBond::BondFunction) n;
	return SpeciesBond::nBondFunctions;
}

// Return functional form text
const char* SpeciesBond::bondFunction(SpeciesBond::BondFunction func)
{
	return BondFunctionKeywords[func];
}

// Return number of parameters required for functional form
int SpeciesBond::nFunctionParameters(SpeciesBond::BondFunction func)
{
	return BondFunctionNParameters[func];
}

// Return energy for specified distance
double SpeciesBond::energy(double distance) const
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	if (form() == SpeciesBond::HarmonicForm)
	{
		/*
		 * Parameters:
		 * 0 : force constant
		 * 1 : equilibrium distance
		 */
		double delta = distance - params[1];
		return 0.5*params[0]*delta*delta;
	}

	Messenger::error("Functional form of SpeciesBond term not set, so can't calculate energy.\n");
	return 0.0;
}

// Return force multiplier for specified distance
double SpeciesBond::force(double distance) const
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	if (form() == SpeciesBond::HarmonicForm)
	{
		/*
		 * Parameters:
		 * 0 : force constant
		 * 1 : equilibrium distance
		 */
		return -params[0]*(distance-params[1]);
	}

	Messenger::error("Functional form of SpeciesBond term not set, so can't calculate force.\n");
	return 0.0;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool SpeciesBond::broadcast(ProcessPool& procPool, const List<SpeciesAtom>& atoms)
{
#ifdef PARALLEL
	int buffer[2];

	// Put atom indices into buffer and send
	if (procPool.isMaster())
	{
		buffer[0] = indexI();
		buffer[1] = indexJ();
	}
	if (!procPool.broadcast(buffer, 2)) return false;
	
	// Slaves now take SpeciesAtom pointers from supplied List
	if (procPool.isSlave())
	{
		setAtoms(atoms.item(buffer[0]), atoms.item(buffer[1]));
		if (i_ != NULL) i_->addBond(this);
		if (j_ != NULL) j_->addBond(this);
	}
	
	// Send parameter info
	if (!procPool.broadcast(parameters_, MAXINTRAPARAMS)) return false;
	if (!procPool.broadcast(form_)) return false;
#endif
	return true;
}
