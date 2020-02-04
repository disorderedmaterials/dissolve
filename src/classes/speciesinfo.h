/*
	*** Species Information
	*** src/classes/speciesinfo.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_SPECIESINFO_H
#define DISSOLVE_SPECIESINFO_H

#include "templates/listitem.h"

// Forward Declarations
class Species;

/*
 * Species Information
 */
class SpeciesInfo : public ListItem<SpeciesInfo>
{
	public:
	// Constructor
	SpeciesInfo();
	// Destructor
	~SpeciesInfo();


	/*
	 * Species Target
	 */
	private:
	// Target Species
	Species* species_;
	// Population of the Species
	int population_;

	public:
	// Set target Species
	void setSpecies(Species* sp);
	// Return target Species
	Species* species();
	// Zero the population of the Species
	void zeroPopulation();
	// Add to population of the Species
	void addPopulation(int pop);
	// Return population of the Species
	int population() const;
};

#endif
