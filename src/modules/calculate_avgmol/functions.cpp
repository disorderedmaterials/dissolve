/*
	*** Calculate Average Molecule Module - Functions
	*** src/modules/calculate_avgmol/functions.cpp
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

#include "modules/calculate_avgmol/avgmol.h"

/*
 * Private Functions
 */

// Update the local species with the coordinates from the supplied arrays
void CalculateAvgMolModule::updateSpecies(const Array<SampledDouble>& x, const Array<SampledDouble>& y, const Array<SampledDouble>& z)
{
	// Loop over atoms in our species
	for (int n=0; n<averageSpecies_.nAtoms(); ++n) averageSpecies_.setAtomCoordinates(n, x.constAt(n).value(), y.constAt(n).value(), z.constAt(n).value());
}

/*
 * Public Functions
 */

// Return average Species
Species& CalculateAvgMolModule::averageSpecies()
{
	return averageSpecies_;
}
