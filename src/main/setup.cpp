/*
	*** Dissolve - Setup
	*** src/main/setup.cpp
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

#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "genericitems/listhelper.h"

// Set up all simulation data, checking it as we go
bool Dissolve::setUpSimulation()
{
	/*
	 * Check Species
	 */

	Messenger::print("*** Checking Species definitions...\n");
	for (Species* sp = species().first(); sp != NULL; sp = sp->next())
	{
		Messenger::print("--- Species '%s'...\n", sp->name());
		if (!sp->checkSetUp(coreData_.atomTypes())) return false;
	}

	/*
	 * Pair Potentials
	 * We expect a PairPotential to have been defined for every combination of AtomType used in the system
	 */

	Messenger::print("\n");
	Messenger::print("*** Setting up PairPotentials...\n");
	int nMissingPots = 0;
	for (AtomType* at1 = coreData_.atomTypes().first(); at1 != NULL; at1 = at1->next())
	{
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next())
		{
			PairPotential* pot = pairPotential(at1, at2);
			if (pot == NULL)
			{
				Messenger::error("A PairPotential between AtomTypes '%s' and '%s' is required, but has not been defined.\n", at1->name(), at2->name());
				++nMissingPots;
				continue;
			}

			// Setup PairPotential
			if (!pot->setUp(pairPotentialRange_, pairPotentialDelta_, pairPotentialsIncludeCoulomb_))
			{
				Messenger::error("Failed to set up PairPotential between AtomTypes '%s' and '%s'.\n", at1->name(), at2->name());
				return false;
			}

			// Retrieve additional potential from the processing module data, if present
			CharString itemName("Potential_%s-%s_Additional", pot->atomTypeNameI(), pot->atomTypeNameJ());

			if (!processingModuleData_.contains(itemName, "Dissolve")) continue;
			pot->setUAdditional(GenericListHelper<Data1D>::retrieve(processingModuleData_, itemName, "Dissolve", Data1D()));
		}
	}
	if (nMissingPots > 0) return false;

	// Create PairPotential matrix
	Messenger::print("Creating PairPotential matrix (%ix%i)...\n", coreData_.nAtomTypes(), coreData_.nAtomTypes());
	if (!potentialMap_.initialise(coreData_.atomTypes(), pairPotentials_, pairPotentialRange_)) return false;


	/*
	 * Perform Set-Up Steps in Modules
	 */

	Messenger::print("*** Performing Module set-up...\n");

	// Loop over all used modules (in Configurations and ModuleLayers)
	RefListIterator<Module> moduleIterator(moduleInstances_);
	while (Module* module = moduleIterator.iterate())
	{
		if (!module->setUp(*this, worldPool())) return false;
	}

	return true;
}
