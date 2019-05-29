/*
	*** Isotopologue Reference
	*** src/classes/isotopologuereference.h
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

#ifndef DISSOLVE_ISOTOPOLOGUEREFERENCE_H
#define DISSOLVE_ISOTOPOLOGUEREFERENCE_H

#include "genericitems/base.h"
#include "templates/listitem.h"

// Forward Declarations
class Configuration;
class Species;
class Isotopologue;
class LineParser;
class ProcessPool;

// Isotopologue Reference
class IsotopologueReference : public ListItem<IsotopologueReference>, public GenericItemBase
{
	public:
	// Constructor
	IsotopologueReference();
	// Destructor
	~IsotopologueReference();


	/*
	 * Data
	 */
	private:
	// Configuration in which the Isotopologue is used
	Configuration* configuration_;
	// Species to which Isotopologue is related
	Species* species_;
	// Isotopologue for Species
	Isotopologue* isotopologue_;
	// Weight
	double weight_;
	
	public:
	// Set all pointers
	void set(Configuration* cfg, Species* sp, Isotopologue* iso, double weight = 1.0);
	// Set Configuration in which the Isotopologue is used
	void setConfiguration(Configuration* cfg);
	// Return Configuration in which the Isotopologue is used
	Configuration* configuration();
	// Set Species to which Isotopologue is related
	void setSpecies(Species* sp);
	// Return Species to which Isotopologue is related
	Species* species();
	// Isotopologue for Species
	void setIsotopologue(Isotopologue* iso);
	// Return Isotopologue for Species
	Isotopologue* isotopologue();
	// Set weight
	void setWeight(double weight);
	// Return weight
	double weight();
	// Return whether the supplied Configuration and Species match
	bool matches(Configuration* cfg, Species* sp) const;


	/*
	 * GenericItemBase Implementations
	 */
	public:
	// Return class name
	static const char* itemClassName();
	// Read data through specified LineParser
	bool read(LineParser& parser, const CoreData& coreData);
	// Write data through specified LineParser
	bool write(LineParser& parser);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data
	bool broadcast(ProcessPool& procPool, const int root, const CoreData& coreData);
	// Check item equality
	bool equality(ProcessPool& procPool);
};

#endif
