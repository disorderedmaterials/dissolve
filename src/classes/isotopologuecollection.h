/*
	*** Isotopologue Collection
	*** src/classes/isotopologuecollection.h
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

#ifndef DISSOLVE_ISOTOPOLOGUECOLLETION_H
#define DISSOLVE_ISOTOPOLOGUECOLLETION_H

#include "classes/isotopologueset.h"
#include "genericitems/base.h"
#include "templates/list.h"

// Forward Declarations
class Configuration;
class Species;
class Isotopologue;
class LineParser;
class ProcessPool;

// IsotopologueCollection - Isotopologues over one or more Configurations
class IsotopologueCollection : public GenericItemBase
{
	public:
	// Constructor
	IsotopologueCollection();
	// Destructor
	~IsotopologueCollection();


	/*
	 * Sets
	 */
	private:
	// IsotopologueSets for individual Configurations
	List<IsotopologueSet> isotopologueSets_;

	public:
	// Clear all existing data
	void clear();
	// Add IsotopologueSet for the specified Configuration
	IsotopologueSet* add(Configuration* cfg);
	// Return defined sets
	List<IsotopologueSet>& isotopologueSets();
	// Return whether a set exists for the supplied Configuration
	bool hasIsotopologueSet(const Configuration* cfg) const;
	// Return IsotopologueSet for the specified Configuration
	IsotopologueSet* isotopologueSet(const Configuration* cfg);
	// Return whether the Species has a defined set of isotopologues in the specified Configuration
	bool hasIsotopologues(const Configuration* cfg, const Species* sp) const;
	// Return Isotopologues for the Species in the specified Configuration
	Isotopologues* isotopologues(const Configuration* cfg, const Species* sp);


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
