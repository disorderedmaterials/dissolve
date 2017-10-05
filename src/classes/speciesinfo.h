/*
	*** Species Information
	*** src/classes/speciesinfo.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_SPECIESINFO_H
#define DUQ_SPECIESINFO_H

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

	public:
	// Set target Species
	void setSpecies(Species* target);
	// Return target Species
	Species* species();


	/*
	 * Information
	 */
	private:
	// Relative population of the Species
	double population_;
	// Whether to randomly rotate Species on insertion
	bool rotateOnInsertion_;
	// Whether to randomly translate Species on insertion
	bool translateOnInsertion_;
	
	public:
	// Set relative population of the Species
	void setPopulation(double pop);
	// Return relative population of the Species
	double population();
	// Set whether to randomly rotate Species on insertion
	void setRotateOnInsertion(bool b);
	// Return whether to randomly rotate Species on insertion
	bool rotateOnInsertion();
	// Set whether to randomly translate Species on insertion
	void setTranslateOnInsertion(bool b);
	// Return whether to randomly translate Species on insertion
	bool translateOnInsertion();
};

#endif
