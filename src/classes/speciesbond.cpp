/*
	*** SpeciesBond Definition
	*** src/classes/speciesbond.cpp
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

#include "classes/speciesbond.h"
#include "classes/speciesatom.h"
#include "base/ptable.h"
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

// Return first SpeciesAtom involved in interaction
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

// Return index (in parent Species) of nth SpeciesAtom in interaction
int SpeciesBond::index(int n) const
{
	if (n == 0) return indexI();
	else if (n == 1) return indexJ();

	Messenger::error("SpeciesAtom index %i is out of range in SpeciesBond::index(int). Returning 0...\n");
	return 0;
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
const char* BondFunctionKeywords[] = { "Harmonic", "EPSR", "SoftHarmonic" };
int BondFunctionNParameters[] = { 2, 2, 3 };

// Convert string to functional form
SpeciesBond::BondFunction SpeciesBond::bondFunction(const char* s)
{
	for (int n=0; n<SpeciesBond::nBondFunctions; ++n) if (DissolveSys::sameString(s, BondFunctionKeywords[n])) return (SpeciesBond::BondFunction) n;
	return SpeciesBond::nBondFunctions;
}

// Return functional form text
const char* SpeciesBond::bondFunction(SpeciesBond::BondFunction func)
{
	return BondFunctionKeywords[func];
}

// Return functional form array
const char** SpeciesBond::bondFunctions()
{
	return BondFunctionKeywords;
}

// Return number of parameters required for functional form
int SpeciesBond::nFunctionParameters(SpeciesBond::BondFunction func)
{
	return BondFunctionNParameters[func];
}

// Set up any necessary parameters
void SpeciesBond::setUp()
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	/*
	 * Depending on the form, we may have other dependent parameters to set up
	 * These are always stored in the *local* SpeciesIntra array, rather than those in any associated master parameters
	 * This way, we can reference both general master parameters as well as others which depend on the atoms involved, for instance
	 */
	if (form() == SpeciesBond::EPSRForm)
	{
		// Work out omega-squared(ab) from mass of natural isotopes
		double massI = periodicTable.element(i_->element()).isotope(0)->atomicWeight();
		double massJ = periodicTable.element(j_->element()).isotope(0)->atomicWeight();
		parameters_[2] = params[1] / sqrt((massI + massJ) / (massI * massJ));
	}
	else if (form() == SpeciesBond::SoftHarmonicForm)
	{
		// Work out omega-squared(ab) from mass of natural isotopes
		double massI = periodicTable.element(i_->element()).isotope(0)->atomicWeight();
		double massJ = periodicTable.element(j_->element()).isotope(0)->atomicWeight();
		parameters_[3] = params[1] / sqrt((massI + massJ) / (massI * massJ));
	}
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
	else if (form() == SpeciesBond::EPSRForm)
	{
		/*
		 * Basically a harmonic oscillator metered by the mass of the atoms (encapsulated in the omegaSquared parameter
		 * 
		 * Parameters:
		 * 0 : general force constant C / 2.0
		 * 1 : equilibrium distance
		 * 2 : omega squared (LOCAL parameter)
		 */
		double delta = distance - params[1];
		return params[0] * (delta*delta) / parameters_[2];
	}
	else if (form() == SpeciesBond::SoftHarmonicForm)
	{
		/*
		 * Around the equilibrium position (up to +/- range), it is a harmonic oscillator metered by the mass of the atoms
		 * a la EPSR, but outside of this range we apply an additional harmonic potential a factor of 100 greater
		 * 
		 * Parameters:
		 * 0 : general force constant C / 2.0
		 * 1 : equilibrium distance
		 * 2 : range
		 * 3 : omega squared (LOCAL parameter)
		 */
		double delta = fabs(distance - params[1]);
		if (delta < params[2]) return params[0] * (delta*delta) / parameters_[3];
		else return params[0] * (delta*delta) / parameters_[3] + 3000.0 * ((delta - params[2])*(delta - params[2]));
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
	else if (form() == SpeciesBond::EPSRForm)
	{
		/*
		 * Basically a harmonic oscillator metered by the mass of the atoms (encapsulated in the omegaSquared parameter
		 * 
		 * Parameters:
		 * 0 : general force constant C / 2.0
		 * 1 : equilibrium distance
		 * 2 : omega squared (LOCAL parameter)
		 */
		return -2.0 * params[0] * (distance - params[1]);
	}
	else if (form() == SpeciesBond::SoftHarmonicForm)
	{
		/*
		 * Around the equilibrium position (up to +/- range), it is a harmonic oscillator metered by the mass of the atoms
		 * a la EPSR, but outside of this range we apply an additional harmonic potential a factor of 100 greater
		 * 
		 * Parameters:
		 * 0 : general force constant C / 2.0
		 * 1 : equilibrium distance
		 * 2 : range
		 * 3 : omega squared (LOCAL parameter)
		 */
		double delta = distance - params[1];
		if (fabs(delta) < params[2]) return -params[0] * delta / parameters_[2];
		else if (delta < 0) return -2.0 * ((params[0] * delta / parameters_[3]) + 3000.0 * params[0] * (delta + params[2]));
		else return -2.0 * ((params[0] * delta / parameters_[3]) + 3000.0 * params[0] * (delta - params[2]));
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
