/*
	*** Isotope Data
	*** src/classes/isotopedata.h
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

#ifndef DISSOLVE_ISOTOPEDATA_H
#define DISSOLVE_ISOTOPEDATA_H

#include "templates/mpilistitem.h"

// Forward Declarations
class CoreData;
class Isotope;
class LineParser;

/*
 * IsotopeData Definition
 */
class IsotopeData : public MPIListItem<IsotopeData>
{
	public:
	// Constructor
	IsotopeData();
	// Copy Constructor
	IsotopeData(const IsotopeData& source);
	// Assignment Operator
	void operator=(const IsotopeData& source);


	/*
	 * Properties
	 */
	private:
	// Reference Isotope
	Isotope* isotope_;
	// Population of Isotope
	double population_;
	// Local fractional population (e.g. within an AtomTypeData)
	double fraction_;

	public:
	// Initialise
	bool initialise(Isotope* isotope);
	// Add to population of Isotope
	void add(double nAdd);
	// Finalise, calculating local fractional population (e.g. within an AtomTypeData)
	void finalise(double totalAtoms);
	// Zero population and fraction
	void zeroPopulation();
	// Return reference Isotope
	Isotope* isotope() const;
	// Return total population
	double population() const;
	// Return local fractional population (e.g. within an AtomTypeData)
	double fraction() const;


	/*
	 * I/O
	 */
	public:
	// Write data through specified LineParser
	bool write(LineParser& parser);
	// Read data through specified LineParser
	bool read(LineParser& parser, const CoreData& coreData);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, int root = 0);
	// Check item equality
	bool equality(ProcessPool& procPool);
};

#endif
