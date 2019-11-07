/*
	*** SpeciesImproper Definition
	*** src/classes/speciesimproper.cpp
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

#include "classes/speciesimproper.h"
#include "classes/speciesatom.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/enumhelpers.h"

// Constructor
SpeciesImproper::SpeciesImproper() : SpeciesIntra(), DynamicArrayObject<SpeciesImproper>()
{
	clear();
}

// Destructor
SpeciesImproper::~SpeciesImproper()
{
}

/*
 * DynamicArrayObject Virtuals
 */

// Clear object, ready for re-use
void SpeciesTorsion::clear()
{
	parent_ = NULL;
	i_ = NULL;
	j_ = NULL;
	k_ = NULL;
	l_ = NULL;
	form_ = SpeciesImproper::nImproperFunctions;
}

/*
 * Atom Information
 */

// Set Atoms involved in Improper
void SpeciesImproper::setAtoms(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l)
{
	i_ = i;
	j_ = j;
	k_ = k;
	l_ = l;
#ifdef CHECKS
	if (i_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* i in SpeciesImproper::set().\n");
	if (j_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* j in SpeciesImproper::set().\n");
	if (k_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* k in SpeciesImproper::set().\n");
	if (l_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* l in SpeciesImproper::set().\n");
#endif
}

// Return first SpeciesAtom
SpeciesAtom* SpeciesImproper::i() const
{
	return i_;
}

// Return second SpeciesAtom
SpeciesAtom* SpeciesImproper::j() const
{
	return j_;
}

// Return third SpeciesAtom
SpeciesAtom* SpeciesImproper::k() const
{
	return k_;
}

// Return fourth SpeciesAtom
SpeciesAtom* SpeciesImproper::l() const
{
	return l_;
}

// Return index (in parent Species) of first SpeciesAtom
int SpeciesImproper::indexI() const
{
#ifdef CHECKS
	if (i_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'i' found in SpeciesImproper::indexI(). Returning 0...\n");
		return 0;
	}
#endif
	return i_->index();
}

// Return index (in parent Species) of second (central) SpeciesAtom
int SpeciesImproper::indexJ() const
{
#ifdef CHECKS
	if (j_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'j' found in SpeciesImproper::indexJ(). Returning 0...\n");
		return 0;
	}
#endif
	return j_->index();
}

// Return index (in parent Species) of third SpeciesAtom
int SpeciesImproper::indexK() const
{
#ifdef CHECKS
	if (k_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'k' found in SpeciesImproper::indexK(). Returning 0...\n");
		return 0;
	}
#endif
	return k_->index();
}

// Return index (in parent Species) of fourth SpeciesAtom
int SpeciesImproper::indexL() const
{
#ifdef CHECKS
	if (l_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'l' found in SpeciesImproper::indexL(). Returning 0...\n");
		return 0;
	}
#endif
	return l_->index();
}

// Return index (in parent Species) of nth SpeciesAtom in interaction
int SpeciesImproper::index(int n) const
{
	if (n == 0) return indexI();
	else if (n == 1) return indexJ();
	else if (n == 2) return indexK();
	else if (n == 3) return indexL();

	Messenger::error("SpeciesAtom index %i is out of range in SpeciesImproper::index(int). Returning 0...\n");
	return 0;
}

// Return whether Atoms in Improper match those specified
bool SpeciesImproper::matches(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l) const
{
	if (i_ == i)
	{
		if (j_ != j) return false;
		if (k_ != k) return false;
		if (l_ == l) return true;
	}
	else if (i_ == l)
	{
		if (j_ != k) return false;
		if (k_ != j) return false;
		if (l_ == i) return true;
	}

	return false;
}

/*
 * Interaction Parameters
 */

// Improper function keywords
const char* ImproperFunctionKeywords[] = { "Cos", "Cos3", "Cos4", "Cos3C", "UFFCosine" };
int ImproperFunctionNParameters[] = { 4, 3, 4, 4, 3 };

// Convert string to functional form
SpeciesImproper::ImproperFunction SpeciesImproper::torsionFunction(const char* s)
{
	for (int n=0; n<SpeciesImproper::nImproperFunctions; ++n) if (DissolveSys::sameString(s, ImproperFunctionKeywords[n])) return (SpeciesImproper::ImproperFunction) n;
	return SpeciesImproper::nImproperFunctions;
}

// Return functional form text
const char* SpeciesImproper::torsionFunction(SpeciesImproper::ImproperFunction func)
{
	return ImproperFunctionKeywords[func];
}

// Return functional form array
const char** SpeciesImproper::torsionFunctions()
{
	return ImproperFunctionKeywords;
}

// Return number of parameters required for functional form
int SpeciesImproper::nFunctionParameters(SpeciesImproper::ImproperFunction func)
{
	return ImproperFunctionNParameters[func];
}

// Set up any necessary parameters
void SpeciesImproper::setUp()
{
}

// Calculate and return fundamental frequency for the interaction
double SpeciesImproper::fundamentalFrequency(double reducedMass) const
{
	Messenger::warn("No fundamental frequency can be calculated for this torsion interaction.\n");
	return 0.0;
}

// Return type of this interaction
SpeciesIntra::IntramolecularType SpeciesImproper::type() const
{
	return SpeciesIntra::IntramolecularImproper;
}

// Return energy for specified angle
double SpeciesImproper::energy(double angleInDegrees) const
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	// Convert torsion angle from degrees to radians
	double phi = angleInDegrees / DEGRAD;

	if (form() == SpeciesImproper::CosineForm)
	{
		/*
		 * U(phi) = k * (1 + s*cos(n*phi - eq))
		 *
		 * Parameters:
		 * 0 : force constant k
		 * 1 : Period 'n'
		 * 2 : equilibrium angle (degrees)
		 * 3 : Sign 's'
		 */
		return params[0] * (1.0 + params[3] * cos(params[1]*phi - (params[2] / DEGRAD)));
	}
	else if (form() == SpeciesImproper::Cos3Form)
	{
		/*
		 * U(phi) = 0.5 * ( k1*(1+cos(phi)) + k2*(1-cos(2*phi)) + k3*(1+cos(3*phi)) )
		 *
		 * Parameters:
		 * 0 : force constant k1
		 * 1 : force constant k2
		 * 2 : force constant k3
		 */
		return 0.5 * (params[0] * (1.0 + cos(phi)) + params[1] * (1.0 - cos(2.0*phi)) + params[2] * (1.0 + cos(3.0*phi)));
	}
	else if (form() == SpeciesImproper::Cos4Form)
	{
		/*
		 * U(phi) = 0.5 * ( k1*(1+cos(phi)) + k2*(1-cos(2*phi)) + k3*(1+cos(3*phi)) + k4*(1-cos(4*phi)) )
		 *
		 * Parameters:
		 * 0 : force constant k1
		 * 1 : force constant k2
		 * 2 : force constant k3
		 * 3 : force constant k4
		 */
		return 0.5 * (params[0]*(1.0+cos(phi)) + params[1]*(1.0-cos(2.0*phi)) + params[2]*(1.0+cos(3.0*phi)) + params[3]*(1.0-cos(4.0*phi)) );
	}
	else if (form() == SpeciesImproper::Cos3CForm)
	{
		/*
		 * U(phi) = k0 + 0.5 * ( k1*(1+cos(phi)) + k2*(1-cos(2*phi)) + k3*(1+cos(3*phi)) )
		 *
		 * Parameters:
		 * 0 : force constant k0
		 * 1 : force constant k1
		 * 2 : force constant k2
		 * 3 : force constant k3
		 */
		return params[0] + 0.5 * (params[1]*(1.0+cos(phi)) + params[2]*(1.0-cos(2.0*phi)) + params[3]*(1.0+cos(3.0*phi)) );
	}
	else if (form() == SpeciesImproper::UFFCosineForm)
	{
		/*
		 * U(phi) = 0.5 * V * (1 - cos(n*eq) * cos(n*phi))
		 *
		 * Parameters:
		 * 0 : Force constant, V
		 * 1 : Periodicity, n
		 * 2 : Equilibrium angle, eq (degrees)
		 */
		return 0.5 * params[0] * (1.0 - cos(params[1]*params[2]/DEGRAD) * cos(params[1]*phi));
	}

	Messenger::error("Functional form of SpeciesImproper term not set, so can't calculate energy.\n");
	return 0.0;
}

// Return force multiplier for specified angle
double SpeciesImproper::force(double angleInDegrees) const
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	// Convert torsion angle from degrees to radians, and calculate derivative w.r.t. change in torsion angle
	double phi = angleInDegrees / DEGRAD;
	double dphi_dcosphi = (phi < 1E-8 ? 0.0 : -1.0 / sin(phi));

	if (form() == SpeciesImproper::CosineForm)
	{
		/*
		 * dU/dphi = k * n * s * -sin(n*phi - eq)
		 *
		 * Parameters:
		 * 0 : Force constant 'k'
		 * 1 : Period 'n'
		 * 2 : Equilibrium angle (degrees)
		 * 3 : Sign 's'
		 */
		return dphi_dcosphi * params[1] * params[0] * params[3] * -sin(params[1]*phi - (params[2] / DEGRAD));
	}
	else if (form() == SpeciesImproper::Cos3Form)
	{
		/*
		 * dU/dphi = 0.5 * ( -k1*sin(phi) + 2 * k2*sin(2*phi) - 3 * k3*(sin(3*phi)) )
		 *
		 * Parameters:
		 * 0 : force constant k1
		 * 1 : force constant k2
		 * 2 : force constant k3
		 */
		return dphi_dcosphi * 0.5 * ( -params[0]*sin(phi) + 2.0*params[1]*sin(2.0*phi) - 3.0*params[2]*sin(3.0*phi));
	}
	else if (form() == SpeciesImproper::Cos4Form)
	{
		/*
		 * dU/dphi = 0.5 * ( -k1*sin(phi) + 2 * k2*sin(2*phi) - 3 * k3*(sin(3*phi)) )
		 *
		 * Parameters:
		 * 0 : force constant k1
		 * 1 : force constant k2
		 * 2 : force constant k3
		 * 3 : force constant k4
		 */
		return dphi_dcosphi * 0.5 * ( params[0]*sin(phi) + params[1]*sin(2.0*phi) + params[2]*sin(3.0*phi) + params[3]*sin(4.0*phi));
	}
	else if (form() == SpeciesImproper::Cos3CForm)
	{
		/*
		 * dU/dphi = 0.5 * ( -k1*sin(phi) + 2 * k2*sin(2*phi) - 3 * k3*(sin(3*phi)) + 4 * k4*(sin(4*phi)))
		 *
		 * Parameters:
		 * 0 : force constant k0
		 * 1 : force constant k1
		 * 2 : force constant k2
		 * 3 : force constant k3
		 */
		return dphi_dcosphi * 0.5 * ( -params[1]*sin(phi) + 2.0*params[2]*sin(2.0*phi) - 3.0*params[3]*sin(3.0*phi));
	}
	else if (form() == SpeciesImproper::UFFCosineForm)
	{
		/*
		 * dU/d(phi) = 0.5 * V * cos(n*eq) * n * sin(n*phi)
		 *
		 * Parameters:
		 * 0 : Force constant, V
		 * 1 : Periodicity, n
		 * 2 : Equilibrium angle, eq (degrees)
		 */
		return 0.5 * params[0] * params[0] * cos(params[1]*params[2]/DEGRAD) * params[1] * sin(params[1]*phi);
	}

	Messenger::error("Functional form of SpeciesImproper term not set, so can't calculate force.\n");
	return 0.0;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool SpeciesImproper::broadcast(ProcessPool& procPool, const List<SpeciesAtom>& atoms)
{
#ifdef PARALLEL
	int buffer[4];

	// Put atom indices into buffer and send
	if (procPool.isMaster())
	{
		buffer[0] = indexI();
		buffer[1] = indexJ();
		buffer[2] = indexK();
		buffer[3] = indexL();
	}
	if (!procPool.broadcast(buffer, 4)) return false;
	
	// Slaves now take Atom pointers from supplied List
	if (procPool.isSlave())
	{
		i_ = atoms.item(buffer[0]);
		j_ = atoms.item(buffer[1]);
		k_ = atoms.item(buffer[2]);
		l_ = atoms.item(buffer[3]);
	}
	
	// Send parameter info
	if (!procPool.broadcast(parameters_, MAXINTRAPARAMS)) return false;
	if (!procPool.broadcast(form_)) return false;
#endif
	return true;
}
