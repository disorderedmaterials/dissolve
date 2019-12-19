/*
	*** Isotopologue Set
	*** src/classes/isotopologueset.h
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

#ifndef DISSOLVE_ISOTOPOLOGUESET_H
#define DISSOLVE_ISOTOPOLOGUESET_H

#include "classes/isotopologues.h"
#include "genericitems/base.h"
#include "templates/list.h"

// Forward Declarations
class Configuration;
class Species;
class Isotopologue;
class LineParser;
class ProcessPool;

// IsotopologueSet - Isotopologues for one or more Species in a single Configuration
class IsotopologueSet : public ListItem<IsotopologueSet>, public GenericItemBase
{
	public:
	// Constructor
	IsotopologueSet();
	// Destructor
	~IsotopologueSet();


	/*
	 * Mix Definitions
	 */
	private:
	// Configuration in which the Species are used
	Configuration* configuration_;
	// Isotopologue mixtures for individual Species
	List<Isotopologues> isotopologues_;

	public:
	// Clear all existing data
	void clear();
	// Set Configuration in which the Species are used
	void setConfiguration(Configuration* cfg);
	// Return Configuration in which the Species are used
	Configuration* configuration() const;
	// Add Isotopologue with the specified relative weight
	void add(Isotopologue* iso, double relativeWeight);
	// Return whether an IsotopologueSet for the specified Species exists
	bool contains(const Species* sp) const;
	// Return Isotopologues for the specified Species
	Isotopologues* isotopologues(const Species* sp);


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
};

#endif
