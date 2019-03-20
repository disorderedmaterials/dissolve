/*
	*** Species Information
	*** src/classes/speciesinfo.cpp
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

#include "classes/speciesinfo.h"
#include "base/sysfunc.h"

// Constructor
SpeciesInfo::SpeciesInfo() : ListItem<SpeciesInfo>()
{
	species_ = NULL;
	population_ = 0.0;
	rotateOnInsertion_ = true;
	insertionPositioning_ = SpeciesInfo::RandomPositioning;
}

// Destructor
SpeciesInfo::~SpeciesInfo()
{
}

/*
 * Species Target
 */

// Set target Species
void SpeciesInfo::setSpecies(Species* target)
{
	species_ = target;
}

// Return target Species
Species* SpeciesInfo::species()
{
	return species_;
}

// Set relative population of the Species
void SpeciesInfo::setPopulation(double pop)
{
	population_ = pop;
}

// Return relative population of the Species
double SpeciesInfo::population() const
{
	return population_;
}

/*
 * Insertion Control
 */

const char* PositioningTypeKeywords[] = { "Central", "Current", "Random" };

// Return permitted PositioningType keywords
const char** SpeciesInfo::positioningTypeKeywords()
{
	return PositioningTypeKeywords;
}

// Convert string to positioning type keyword
SpeciesInfo::PositioningType SpeciesInfo::positioningType(const char* s)
{
	for (int pt=0; pt < SpeciesInfo::nPositioningTypes; ++pt) if (DissolveSys::sameString(s, PositioningTypeKeywords[pt])) return (SpeciesInfo::PositioningType) pt;

	return SpeciesInfo::nPositioningTypes;
}

// Convert positioning type to string
const char* SpeciesInfo::positioningType(SpeciesInfo::PositioningType pt)
{
	return PositioningTypeKeywords[pt];
}

// Set whether to randomly rotate Species on insertion
void SpeciesInfo::setRotateOnInsertion(bool b)
{
	rotateOnInsertion_ = b;
}

// Return whether to randomly rotate Species on insertion
bool SpeciesInfo::rotateOnInsertion() const
{
	return rotateOnInsertion_;
}

// Set positioning type for Species on insertion
void SpeciesInfo::setInsertionPositioning(SpeciesInfo::PositioningType posType)
{
	insertionPositioning_ = posType;
}

// Return positioning type for Species on insertion
SpeciesInfo::PositioningType SpeciesInfo::insertionPositioning() const
{
	return insertionPositioning_;
}
