/*
	*** SpeciesAngle Definition
	*** src/classes/speciesangle.cpp
	Copyright T. Youngs 2012-2018

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

#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/enumhelpers.h"

// Constructor
SpeciesAngle::SpeciesAngle() : SpeciesIntra(), ListItem<SpeciesAngle>()
{
	parent_ = NULL;
	i_ = NULL;
	j_ = NULL;
	k_ = NULL;
	form_ = SpeciesAngle::nAngleFunctions;
}

// Destructor
SpeciesAngle::~SpeciesAngle()
{
}

/*
 * Atom Information
 */

// Set SpeciesAtoms involved in interaction
void SpeciesAngle::setAtoms(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k)
{
	i_ = i;
	j_ = j;
	k_ = k;
#ifdef CHECKS
	if (i_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* i in SpeciesAngle::set().\n");
	if (j_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* j in SpeciesAngle::set().\n");
	if (k_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* k in SpeciesAngle::set().\n");
#endif
}

// Return first SpeciesAtom
SpeciesAtom* SpeciesAngle::i() const
{
	return i_;
}

// Return second (central) SpeciesAtom
SpeciesAtom* SpeciesAngle::j() const
{
	return j_;
}

// Return third SpeciesAtom
SpeciesAtom* SpeciesAngle::k() const
{
	return k_;
}

// Return index (in parent Species) of first SpeciesAtom
int SpeciesAngle::indexI() const
{
#ifdef CHECKS
	if (i_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'i' found in SpeciesAngle::indexI(). Returning 0...\n");
		return 0;
	}
#endif
	return i_->index();
}

// Return index (in parent Species) of second (central) SpeciesAtom
int SpeciesAngle::indexJ() const
{
#ifdef CHECKS
	if (j_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'j' found in SpeciesAngle::indexJ(). Returning 0...\n");
		return 0;
	}
#endif
	return j_->index();
}

// Return index (in parent Species) of third SpeciesAtom
int SpeciesAngle::indexK() const
{
#ifdef CHECKS
	if (k_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'k' found in SpeciesAngle::indexK(). Returning 0...\n");
		return 0;
	}
#endif
	return k_->index();
}

// Return index (in parent Species) of nth SpeciesAtom in interaction
int SpeciesAngle::index(int n) const
{
	if (n == 0) return indexI();
	else if (n == 1) return indexJ();
	else if (n == 2) return indexK();

	Messenger::error("SpeciesAtom index %i is out of range in SpeciesAngle::index(int). Returning 0...\n");
	return 0;
}

// Return whether Atoms in Angle match those specified
bool SpeciesAngle::matches(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k) const
{
	if (j_ != j) return false;
	if ((i_ == i) && (k_ == k)) return true;
	if ((i_ == k) && (k_ == i)) return true;
	return false;
}

/*
 * Interaction Parameters
 */

// Angle function keywords
const char* AngleFunctionKeywords[] = { "Harmonic" };
int AngleFunctionNParameters[] = { 2 };

// Convert string to functional form
SpeciesAngle::AngleFunction SpeciesAngle::angleFunction(const char* s)
{
	for (int n=0; n<SpeciesAngle::nAngleFunctions; ++n) if (DUQSys::sameString(s, AngleFunctionKeywords[n])) return (SpeciesAngle::AngleFunction) n;
	return SpeciesAngle::nAngleFunctions;
}

// Return functional form text
const char* SpeciesAngle::angleFunction(SpeciesAngle::AngleFunction func)
{
	return AngleFunctionKeywords[func];
}

// Return functional form text
const char** SpeciesAngle::angleFunctions()
{
	return AngleFunctionKeywords;
}

// Return number of parameters required for functional form
int SpeciesAngle::nFunctionParameters(AngleFunction func)
{
	return AngleFunctionNParameters[func];
}

// Return energy for specified angle
double SpeciesAngle::energy(double angleInDegrees) const
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	if (form() == SpeciesAngle::HarmonicForm)
	{
		/*
		 * Parameters:
		 * 0 : force constant
		 * 1 : equilibrium angle (degrees)
		 */
		double delta = (angleInDegrees - params[1]) / DEGRAD;
		return 0.5*params[0]*delta*delta;
	}

	Messenger::error("Functional form of SpeciesAngle term not set, so can't calculate energy.\n");
	return 0.0;
}

// Return force multiplier for specified angle
double SpeciesAngle::force(double angleInDegrees) const
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	if (form() == SpeciesAngle::HarmonicForm)
	{
		/*
		 * Parameters:
		 * 0 : force constant
		 * 1 : equilibrium angle (degrees)
		 */
		// Set initial derivative of angle w.r.t. cos(angle)
		double dU_dtheta = -1.0 / sin(angleInDegrees/DEGRAD);

		// Chain rule - multiply by derivative of energy w.r.t. angle (harmonic form)
		dU_dtheta *= -params[0]*((angleInDegrees-params[1])/DEGRAD);

		return dU_dtheta;
	}

	Messenger::error("Functional form of SpeciesAngle term not set, so can't calculate force.\n");
	return 0.0;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool SpeciesAngle::broadcast(ProcessPool& procPool, const List<SpeciesAtom>& atoms)
{
#ifdef PARALLEL
	int buffer[3];

	// Put atom indices into buffer and send
	if (procPool.isMaster())
	{
		buffer[0] = indexI();
		buffer[1] = indexJ();
		buffer[2] = indexK();
	}
	if (!procPool.broadcast(buffer, 3)) return false;
	
	// Slaves now take Atom pointers from supplied List
	if (procPool.isSlave())
	{
		i_ = atoms.item(buffer[0]);
		j_ = atoms.item(buffer[1]);
		k_ = atoms.item(buffer[2]);
	}
	
	// Send parameter info
	if (!procPool.broadcast(parameters_, MAXINTRAPARAMS)) return false;
	if (!procPool.broadcast(form_)) return false;
#endif
	return true;
}
