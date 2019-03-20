/*
	*** Species Information
	*** src/classes/speciesinfo.h
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
	// Relative population of the Species
	double population_;

	public:
	// Set target Species
	void setSpecies(Species* target);
	// Return target Species
	Species* species();
	// Set relative population of the Species
	void setPopulation(double pop);
	// Return relative population of the Species
	double population() const;


	/*
	 * Control
	 */
	public:
	// Positioning Type
	enum PositioningType
	{
		CentralPositioning,			/* Position the Species at the centre of the Box */
		CurrentPositioning,			/* Use current Species coordinates */
		RandomPositioning,			/* Set position randomly */
		nPositioningTypes
	};
	// Return permitted PositioningType keywords
	static const char** positioningTypeKeywords();
	// Convert string to positioning type keyword
	static PositioningType positioningType(const char* s);
	// Convert positioning type to string
	static const char* positioningType(SpeciesInfo::PositioningType pt);

	private:
	// Whether to randomly rotate Species on insertion
	bool rotateOnInsertion_;
	// Position of Species on insertion
	PositioningType insertionPositioning_;
	
	public:
	// Set whether to randomly rotate Species on insertion
	void setRotateOnInsertion(bool b);
	// Return whether to randomly rotate Species on insertion
	bool rotateOnInsertion() const;
	// Set positioning type for Species on insertion
	void setInsertionPositioning(PositioningType posType);
	// Return positioning type for Species on insertion
	PositioningType insertionPositioning() const;
};

#endif
