/*
	*** Isotope Definition
	*** src/base/isotope.cpp
	Copyright T. Youngs 2012-2014

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

#include "base/isotope.h"
#include "base/comms.h"

// Constructor
Isotope::Isotope() : MPIListItem<Isotope>()
{
	A_ = 0;
	atomicWeight_ = 0.0;
	boundCoherent_ = 0.0;
	boundIncoherent_ = 0.0;
	boundCoherentXS_ = 0.0;
	boundIncoherentXS_ = 0.0;
	totalXS_ = 0.0;
	absorptionXS_ = 0.0;
}

// Destructor
Isotope::~Isotope()
{
}

/*
 * Isotope Definition
 */

// Set isotope information
void Isotope::set(int A, double weight, double bc, double bi, double sc, double si, double totalxs, double absxs)
{
	A_ = A;
	atomicWeight_ = weight;
	boundCoherent_ = bc;
	boundIncoherent_ = bi;
	boundCoherentXS_ = sc;
	boundIncoherentXS_ = si;
	totalXS_ = totalxs;
	absorptionXS_ = absxs;
}

// Mass number (A) of isotope
int Isotope::A() const
{
	return A_;
}

// Return atomic weight (given C = 12)
double Isotope::atomicWeight() const
{
	return atomicWeight_;
}

// Return bound coherent scattering length (fm)
double Isotope::boundCoherent() const
{
	return boundCoherent_;
}

// Return bound incoherent scattering length (fm)
double Isotope::boundIncoherent() const
{
	return boundIncoherent_;
}

// Return bound coherent scattering cross section (barn)
double Isotope::boundCoherentXS() const
{
	return boundCoherentXS_;
}

// Return bound incoherent scattering cross section (barn)
double Isotope::boundIncoherentXS() const
{
	return boundIncoherentXS_;
}

// Return total bound scattering cross section (barn)
double Isotope::totalXS() const
{
	return totalXS_;
}

// Return absorption cross section for thermal (2200 m/s) neutron (barn)
double Isotope::absorptionXS() const
{
	return absorptionXS_;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Isotope::broadcast()
{
#ifdef PARALLEL
	// Send isotope info
	if (!Comm.broadcast(&A_, 1)) return false;
	if (!Comm.broadcast(&atomicWeight_, 1)) return false;
	if (!Comm.broadcast(&boundCoherent_, 1)) return false;
	if (!Comm.broadcast(&boundIncoherent_, 1)) return false;
	if (!Comm.broadcast(&boundCoherentXS_, 1)) return false;
	if (!Comm.broadcast(&boundIncoherentXS_, 1)) return false;
	if (!Comm.broadcast(&totalXS_, 1)) return false;
	if (!Comm.broadcast(&absorptionXS_, 1)) return false;
#endif
	return true;
}
