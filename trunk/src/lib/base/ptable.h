/*
	*** Periodic Table (Element and Isotope information)
	*** src/lib/base/ptable.h
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

#ifndef DUQ_PERIODICTABLE_H
#define DUQ_PERIODICTABLE_H

#include "base/element.h"
#include "base/isotope.h"

// Forward Declarations
/* none */

/*!
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


	/*!
	 * \name Element Data
	 */
	///@{
	private:
	// Element array
	static Element* elements_;
	// Number of elements defined in array
	static int nElements_;
	
	public:
	// Load basic element information from file specified
	static bool loadElements(const char* fileName);
	// Load Isotope information from file specified
	static bool loadIsotopes(const char* fileName);
	// Load Parameters from file specified
	static bool loadParameters(const char* fileName);
	// Return atomic number of element in string
	static int find(const char* query);
	// Return number of defined elements
	static int nElements();
	// Return element specified
	static Element &element(int z);
	///@}
	
	
	/*!
	 * \name Empirical Formula Generation
	 */
	///@{
	private:
	// Element counts array
	static int *elementCount_;

	public:
	// Clear counts array
	static void resetEmpiricalFormula();
	// Add Atom (element) to empirical formula
	static void addToEmpirical(int z, int count = 1);
	// Return current empirical formula
	static const char* empiricalFormula();
	///@}


	/*!
	 * \name Parallel Comms
	 */
	///@{
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast();
	///@}
};

#endif
