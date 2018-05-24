/*
	*** Periodic Table (Element and Isotope information)
	*** src/base/ptable.h
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

#ifndef DISSOLVE_PERIODICTABLE_H
#define DISSOLVE_PERIODICTABLE_H

#include "base/element.h"
#include "base/isotope.h"

// Forward Declarations
/* none */

/*
 * \short Periodic Table
 * \details Periodic table-style structure containing information on all elements and their isotopes, along with neutron scattering length information.
 */
class PeriodicTable
{
	public:
	// Constructor
	PeriodicTable();
	// Destructor
	~PeriodicTable();
	// Clear all data
	void clear();


	/*
	 * Element Data
	 */
	private:
	// Element array
	static Element* elements_;
	// Number of elements defined in array
	static int nElements_;
	
	public:
	// Load basic element information from file specified
	static bool loadElements(const char* filename);
	// Load Isotope information from file specified
	static bool loadIsotopes(const char* filename);
	// Return atomic number of element in string
	static int find(const char* query);
	// Return number of defined elements
	static int nElements();
	// Return element specified
	static Element& element(int z);
	
	
	/*
	 * Empirical Formula Generation
	 */
	private:
	// Element counts array
	static int* elementCount_;

	public:
	// Clear counts array
	static void resetEmpiricalFormula();
	// Add Atom (element) to empirical formula
	static void addToEmpirical(int z, int count = 1);
	// Return current empirical formula
	static const char* empiricalFormula();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool);
};

// Periodic table data
extern PeriodicTable periodicTable;

#endif
