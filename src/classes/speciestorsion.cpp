/*
	*** SpeciesTorsion Definition
	*** src/classes/speciestorsion.cpp
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

#include "classes/speciestorsion.h"
#include "classes/speciesatom.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/enumhelpers.h"

// Constructor
SpeciesTorsion::SpeciesTorsion() : SpeciesIntra(), ListItem<SpeciesTorsion>()
{
	parent_ = NULL;
	i_ = NULL;
	j_ = NULL;
	k_ = NULL;
	l_ = NULL;
	form_ = SpeciesTorsion::nTorsionFunctions;
}

// Destructor
SpeciesTorsion::~SpeciesTorsion()
{
}

/*
 * Atom Information
 */

// Set Atoms involved in Torsion
void SpeciesTorsion::setAtoms(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l)
{
	i_ = i;
	j_ = j;
	k_ = k;
	l_ = l;
#ifdef CHECKS
	if (i_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* i in SpeciesTorsion::set().\n");
	if (j_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* j in SpeciesTorsion::set().\n");
	if (k_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* k in SpeciesTorsion::set().\n");
	if (l_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* l in SpeciesTorsion::set().\n");
#endif
}

// Return first SpeciesAtom
SpeciesAtom* SpeciesTorsion::i() const
{
	return i_;
}

// Return second SpeciesAtom
SpeciesAtom* SpeciesTorsion::j() const
{
	return j_;
}

// Return third SpeciesAtom
SpeciesAtom* SpeciesTorsion::k() const
{
	return k_;
}

// Return fourth SpeciesAtom
SpeciesAtom* SpeciesTorsion::l() const
{
	return l_;
}

// Return index (in parent Species) of first SpeciesAtom
int SpeciesTorsion::indexI() const
{
#ifdef CHECKS
	if (i_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'i' found in SpeciesTorsion::indexI(). Returning 0...\n");
		return 0;
	}
#endif
	return i_->index();
}

// Return index (in parent Species) of second (central) SpeciesAtom
int SpeciesTorsion::indexJ() const
{
#ifdef CHECKS
	if (j_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'j' found in SpeciesTorsion::indexJ(). Returning 0...\n");
		return 0;
	}
#endif
	return j_->index();
}

// Return index (in parent Species) of third SpeciesAtom
int SpeciesTorsion::indexK() const
{
#ifdef CHECKS
	if (k_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'k' found in SpeciesTorsion::indexK(). Returning 0...\n");
		return 0;
	}
#endif
	return k_->index();
}

// Return index (in parent Species) of fourth SpeciesAtom
int SpeciesTorsion::indexL() const
{
#ifdef CHECKS
	if (l_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'l' found in SpeciesTorsion::indexL(). Returning 0...\n");
		return 0;
	}
#endif
	return l_->index();
}

// Return index (in parent Species) of nth SpeciesAtom in interaction
int SpeciesTorsion::index(int n) const
{
	if (n == 0) return indexI();
	else if (n == 1) return indexJ();
	else if (n == 2) return indexK();
	else if (n == 3) return indexL();

	Messenger::error("SpeciesAtom index %i is out of range in SpeciesTorsion::index(int). Returning 0...\n");
	return 0;
}

// Return whether Atoms in Torsion match those specified
bool SpeciesTorsion::matches(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l) const
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

// Torsion function keywords
const char* TorsionFunctionKeywords[] = { "Cos", "Cos3", "Cos4", "Cos3C" };
int TorsionFunctionNParameters[] = { 4, 3, 4, 4 };

// Convert string to functional form
SpeciesTorsion::TorsionFunction SpeciesTorsion::torsionFunction(const char* s)
{
	for (int n=0; n<SpeciesTorsion::nTorsionFunctions; ++n) if (DissolveSys::sameString(s, TorsionFunctionKeywords[n])) return (SpeciesTorsion::TorsionFunction) n;
	return SpeciesTorsion::nTorsionFunctions;
}

// Return functional form text
const char* SpeciesTorsion::torsionFunction(SpeciesTorsion::TorsionFunction func)
{
	return TorsionFunctionKeywords[func];
}

// Return functional form array
const char** SpeciesTorsion::torsionFunctions()
{
	return TorsionFunctionKeywords;
}

// Return number of parameters required for functional form
int SpeciesTorsion::nFunctionParameters(SpeciesTorsion::TorsionFunction func)
{
	return TorsionFunctionNParameters[func];
}

// Set up any necessary parameters
void SpeciesTorsion::setUp()
{
}

// Calculate and return fundamental frequency for the interaction
double SpeciesTorsion::fundamentalFrequency(double reducedMass) const
{
	Messenger::warn("No fundamental frequency can be calculated for this torsion interaction.\n");
	return 0.0;
}

// Return type of this interaction
SpeciesIntra::IntramolecularType SpeciesTorsion::type() const
{
	return SpeciesIntra::IntramolecularTorsion;
}

// Return energy for specified angle
double SpeciesTorsion::energy(double angleInDegrees) const
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	// Convert torsion angle from degrees to radians
	double phi = angleInDegrees / DEGRAD;

	if (form() == SpeciesTorsion::CosineForm)
	{
		/*
		 * U(phi) = forcek * (1 + s*cos(period*phi - eq))
		 *
		 * Parameters:
		 * 0 : force constant k1
		 * 1 : Period N
		 * 2 : equilibrium angle (degrees)
		 * 3 : Sign S
		 */
		return params[0] * (1.0 + params[3] * cos(params[1]*phi - (params[2] / DEGRAD)));
	}
	else if (form() == SpeciesTorsion::Cos3Form)
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
	else if (form() == SpeciesTorsion::Cos4Form)
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
	else if (form() == SpeciesTorsion::Cos3CForm)
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

	Messenger::error("Functional form of SpeciesTorsion term not set, so can't calculate energy.\n");
	return 0.0;
}

// Return force multiplier for specified angle
double SpeciesTorsion::force(double angleInDegrees) const
{
	// Get pointer to relevant parameters array
	const double* params = parameters();

	// Convert torsion angle from degrees to radians, and calculate derivative w.r.t. change in torsion angle
	double phi = angleInDegrees / DEGRAD;
	double dphi_dcosphi = (phi < 1E-8 ? 0.0 : -1.0 / sin(phi));

	if (form() == SpeciesTorsion::CosineForm)
	{
		/*
		 * dU/dphi = forcek * period * s * -sin(period*phi - eq)
		 *
		 * Parameters:
		 * 0 : force constant k1
		 * 1 : Period N
		 * 2 : equilibrium angle (degrees)
		 * 3 : Sign S
		 */
		return dphi_dcosphi * params[1] * params[0] * params[3] * -sin(params[1]*phi - (params[2] / DEGRAD));
	}
	else if (form() == SpeciesTorsion::Cos3Form)
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
	else if (form() == SpeciesTorsion::Cos4Form)
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
	else if (form() == SpeciesTorsion::Cos3CForm)
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

	Messenger::error("Functional form of SpeciesTorsion term not set, so can't calculate force.\n");
	return 0.0;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool SpeciesTorsion::broadcast(ProcessPool& procPool, const List<SpeciesAtom>& atoms)
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
