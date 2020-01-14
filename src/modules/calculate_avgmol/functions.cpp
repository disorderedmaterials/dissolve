/*
	*** Calculate Average Molecule Module - Functions
	*** src/modules/calculate_avgmol/functions.cpp
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

#include "modules/calculate_avgmol/avgmol.h"
#include "main/dissolve.h"
#include "genericitems/listhelper.h"

/*
 * Private Functions
 */

// Ensure arrays are the correct size for the current target Species
void CalculateAvgMolModule::updateArrays(Dissolve& dissolve)
{
	int requiredSize = targetSpecies_ ? targetSpecies_->nAtoms() : -1;

	// Retrieve / create the three data arrays, and size accordingly
	Array<SampledDouble>& x = GenericListHelper< Array<SampledDouble> >::realise(dissolve.processingModuleData(), "X", uniqueName(), GenericItem::InRestartFileFlag);
	Array<SampledDouble>& y = GenericListHelper< Array<SampledDouble> >::realise(dissolve.processingModuleData(), "Y", uniqueName(), GenericItem::InRestartFileFlag);
	Array<SampledDouble>& z = GenericListHelper< Array<SampledDouble> >::realise(dissolve.processingModuleData(), "Z", uniqueName(), GenericItem::InRestartFileFlag);

	if (requiredSize > 0)
	{
		if (x.nItems() == requiredSize && y.nItems() == requiredSize && z.nItems() == requiredSize) Messenger::print("Using existing coordinate arrays for average species.\n");
		else
		{
			Messenger::printVerbose("Initialising arrays for average molecule: size = %i\n", requiredSize);
			x.initialise(requiredSize);
			y.initialise(requiredSize);
			z.initialise(requiredSize);
		}
	}
	else
	{
		x.clear();
		y.clear();
		z.clear();
	}
}

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
