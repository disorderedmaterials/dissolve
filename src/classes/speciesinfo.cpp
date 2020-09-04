/*
    *** Species Information
    *** src/classes/speciesinfo.cpp
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

#include "classes/speciesinfo.h"
#include "base/sysfunc.h"

SpeciesInfo::SpeciesInfo() : ListItem<SpeciesInfo>()
{
    species_ = nullptr;
    population_ = 0;
}

SpeciesInfo::~SpeciesInfo() {}

/*
 * Species Target
 */

// Set target Species
void SpeciesInfo::setSpecies(Species *sp) { species_ = sp; }

// Return target Species
Species *SpeciesInfo::species() { return species_; }

// Zero the population of the Species
void SpeciesInfo::zeroPopulation() { population_ = 0; }

// Add to the population of the Species
void SpeciesInfo::addPopulation(int pop) { population_ += pop; }

// Return relative population of the Species
int SpeciesInfo::population() const { return population_; }
