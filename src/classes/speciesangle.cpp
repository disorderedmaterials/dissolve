/*
	*** SpeciesAngle Definition
	*** src/classes/speciesangle.cpp
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

#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/enumhelpers.h"

// Constructor
SpeciesAngle::SpeciesAngle() : SpeciesIntra(), DynamicArrayObject<SpeciesAngle>()
{
	clear();
}

// Destructor
SpeciesAngle::~SpeciesAngle()
{
}

/*
 * DynamicArrayObject Virtuals
 */

// Clear object, ready for re-use
void SpeciesAngle::clear()
{
	parent_ = NULL;
	i_ = NULL;
	j_ = NULL;
	k_ = NULL;
	form_ = SpeciesAngle::NoForm;
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
	if (i_) i_->addAngle(this);
	if (j_) j_->addAngle(this);
	if (k_) k_->addAngle(this);
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

// Return enum options for AngleFunction
EnumOptions<SpeciesAngle::AngleFunction> SpeciesAngle::angleFunctions()
{
	static EnumOptionsList AngleFunctionOptions = EnumOptionsList() <<
		EnumOption(SpeciesAngle::HarmonicForm, 		"Harmonic",	2,2) <<
		EnumOption(SpeciesAngle::CosineForm, 		"Cos",		4,4) <<
		EnumOption(SpeciesAngle::Cos2Form, 		"Cos2",		4,4);

	static EnumOptions<SpeciesAngle::AngleFunction> options("AngleFunction", AngleFunctionOptions);

	return options;
}

// Set up any necessary parameters
void SpeciesAngle::setUp()
{
}

// Calculate and return fundamental frequency for the interaction
double SpeciesAngle::fundamentalFrequency(double reducedMass) const
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	double k = 0.0;
	if (form() == SpeciesAngle::HarmonicForm) k = params[0];
	else
	{
		Messenger::error("Functional form of SpeciesAngle term not set, or no force constant available, so can't determine fundamental frequency.\n");
		return 0.0;
	}

	// Convert force constant from (assumed) kJ mol-1 A-2 into J m-2 (kg s-2)
	k *= 1000.0 * 1.0e20 / AVOGADRO;
// 	printf("K = %f\n", k);

	// Convert reduced mass from amu to kg
	double mu = reducedMass / (AVOGADRO * 1000.0);
// 	printf("mu = %e\n", mu);

	// Calculate fundamental frequency
	double v = (1.0 / TWOPI) * sqrt(k / mu);
// 	printf("v = %e\n", v);

	return v;
}

// Return type of this interaction
SpeciesIntra::InteractionType SpeciesAngle::type() const
{
	return SpeciesIntra::AngleInteraction;
}

// Return energy for specified angle
double SpeciesAngle::energy(double angleInDegrees) const
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	if (form() == SpeciesAngle::HarmonicForm)
	{
		/*
		 * U(theta) = 0.5 * forcek * (theta - eq)**2
		 *
		 * Parameters:
		 * 0 : Force constant, k
		 * 1 : Equilibrium angle, eq (degrees)
		 */
		double delta = (angleInDegrees - params[1]) / DEGRAD;
		return 0.5*params[0]*delta*delta;
	}
	else if (form() == SpeciesAngle::CosineForm)
	{
		/*
		 * U(theta) = forcek * (1 + s * cos(n*theta - eq))
		 *
		 * Parameters:
		 * 0 : Force constant, k
		 * 1 : Periodicity, N
		 * 2 : Equilibrium angle, eq (degrees)
		 * 3 : Sign, s
		 */
		return params[0] * (1.0 + params[3] * cos(params[1] * angleInDegrees/DEGRAD - params[2]));
	}
	else if (form() == SpeciesAngle::Cos2Form)
	{
		/*
		 * U(theta) = forcek * (C0 + C1 * cos(theta) + C2 * cos(2*theta))
		 *
		 * Parameters:
		 * 0 : Force constant, k
		 * 1 : Constant C0
		 * 2 : Constant C1
		 * 3 : Constant C2
		 */
		const double angleInRadians = angleInDegrees /DEGRAD;
		return params[0] * (params[1] + params[2] * cos(angleInRadians) + params[3] * cos(2.0 * angleInRadians));
	}

	Messenger::error("Functional form of SpeciesAngle term not set, so can't calculate energy.\n");
	return 0.0;
}

// Return force multiplier for specified angle
double SpeciesAngle::force(double angleInDegrees) const
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	// Convert angle to radians
	const double angleInRadians = angleInDegrees /DEGRAD;

	// Set initial derivative of angle w.r.t. cos(angle) for chain rule
	const double dTheta_dCosTheta = -1.0 / sin(angleInDegrees/DEGRAD);

	if (form() == SpeciesAngle::HarmonicForm)
	{
		/*
		 * dU/d(theta) = k * (theta - eq)
		 *
		 * Parameters:
		 * 0 : Force constant, k
		 * 1 : Equilibrium angle, eq (degrees)
		 */

		// Chain rule - multiply by derivative of energy w.r.t. angle (harmonic form)
		return dTheta_dCosTheta * -params[0]*((angleInDegrees-params[1])/DEGRAD);
	}
	else if (form() == SpeciesAngle::CosineForm)
	{
		/*
		 * dU/d(theta) = -forcek * n * s * sin(n*theta - eq)
		 *
		 * Parameters:
		 * 0 : Force constant, k
		 * 1 : Periodicity, N
		 * 2 : Equilibrium angle, eq (degrees)
		 * 3 : Sign, s
		 */
		return dTheta_dCosTheta * -params[0] * params[1] * params[3] * sin(params[1] * angleInRadians - params[2]/DEGRAD);
	}
	else if (form() == SpeciesAngle::Cos2Form)
	{
		/*
		 * dU/d(theta) = -forcek * (c1 * sin(theta) + 2 * c2 * sin(2*theta))
		 *
		 * Parameters:
		 * 0 : Force constant, k
		 * 1 : Constant C0 (unused)
		 * 2 : Constant C1
		 * 3 : Constant C2
		 */
		return dTheta_dCosTheta * -params[0] * (params[2] * sin(angleInRadians) + 2.0 * params[3] * sin(2.0 * angleInRadians));
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
