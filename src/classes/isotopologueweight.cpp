/*
	*** Isotopologue Weight
	*** src/classes/isotopologueweight.cpp
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

#include "classes/isotopologueweight.h"

// Constructor
IsotopologueWeight::IsotopologueWeight(const Isotopologue* iso, double weight) : ListItem<IsotopologueWeight>()
{
	set(iso, weight);
}

// Destructor
IsotopologueWeight::~IsotopologueWeight()
{
}

/*
 * Isotopologue and Weight
 */

// Set associated Isotopologue and weight
void IsotopologueWeight::set(const Isotopologue* iso, double weight)
{
	isotopologue_ = iso;
	weight_ = weight;
}

// Set associated Isotopologue
void IsotopologueWeight::setIsotopologue(const Isotopologue* iso)
{
	isotopologue_ = iso;
}

// Return associated Isotopologue
const Isotopologue* IsotopologueWeight::isotopologue() const
{
	return isotopologue_;
}

// Set relative weight
void IsotopologueWeight::setWeight(double weight)
{
	weight_ = weight;
}

// Return relative weight
double IsotopologueWeight::weight() const
{
	return weight_;
}
