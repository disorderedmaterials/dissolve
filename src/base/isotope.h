/*
	*** Isotope Definition
	*** src/base/isotope.h
	Copyright T. Youngs 2012-2016

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

#ifndef DUQ_ISOTOPE_H
#define DUQ_ISOTOPE_H

#include "templates/list.h"
#include "base/dnchar.h"

// Isotope Definition
class Isotope : public MPIListItem<Isotope>
{
	public:
	// Constructor
	Isotope();
	// Destructor
	~Isotope();


	/*
	 * Isotope Information
	 */
	private:
	// Mass number (A) of isotope
	int A_;
	// Atomic weight (given C = 12)
	double atomicWeight_;
	// Bound coherent scattering length (fm)
	double boundCoherent_;
	// Bound incoherent scattering length (fm)
	double boundIncoherent_;
	// Bound coherent scattering cross section (barn)
	double boundCoherentXS_;
	// Bound incoherent scattering cross section (barn)
	double boundIncoherentXS_;
	// Total bound scattering cross section (barn)
	double totalXS_;
	// Absorption cross section for thermal (2200 m/s) neutron (barn)
	double absorptionXS_;
	
	public:
	// Set isotope information
	void set(int A, double weight, double bc, double bi, double sc, double si, double totalxs, double absxs);
	// Mass number (A) of isotope
	int A() const;
	// Return atomic weight (given C = 12)
	double atomicWeight() const;
	// Return bound coherent scattering length (fm)
	double boundCoherent() const;
	// Return bound incoherent scattering length (fm)
	double boundIncoherent() const;
	// Return bound coherent scattering cross section (barn)
	double boundCoherentXS() const;
	// Return bound incoherent scattering cross section (barn)
	double boundIncoherentXS() const;
	// Return total bound scattering cross section (barn)
	double totalXS() const;
	// Return absorption cross section for thermal (2200 m/s) neutron (barn)
	double absorptionXS() const;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool);
};

#endif
