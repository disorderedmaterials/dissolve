/*
	*** Species Information
	*** src/classes/speciesinfo.cpp
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

#include "classes/speciesinfo.h"

// Constructor
SpeciesInfo::SpeciesInfo() : ListItem<SpeciesInfo>()
{
	species_ = NULL;
	population_ = 0.0;
	rotateOnInsertion_ = true;
	translateOnInsertion_ = true;
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

/*
 * Information
 */
	
// Set relative population of the Species
void SpeciesInfo::setPopulation(double pop)
{
	population_ = pop;
}

// Return relative population of the Species
double SpeciesInfo::population()
{
	return population_;
}

// Set whether to randomly rotate Species on insertion
void SpeciesInfo::setRotateOnInsertion(bool b)
{
	rotateOnInsertion_ = b;
}

// Return whether to randomly rotate Species on insertion
bool SpeciesInfo::rotateOnInsertion()
{
	return rotateOnInsertion_;
}

// Set whether to randomly translate Species on insertion
void SpeciesInfo::setTranslateOnInsertion(bool b)
{
	translateOnInsertion_ = b;
}

// Return whether to randomly translate Species on insertion
bool SpeciesInfo::translateOnInsertion()
{
	return translateOnInsertion_;
}

