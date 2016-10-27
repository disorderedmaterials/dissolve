/*
	*** dUQ - Simulation
	*** src/main/simulation.cpp
	Copyright T. Youngs 2012-2016

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

#include "main/duq.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "base/comms.h"

// Set number of test points to use when calculating Box normalisation arrays
void DUQ::setBoxNormalisationPoints(int nPoints)
{
	boxNormalisationPoints_ = nPoints;
}

// Bin width to use when calculating RMSE between Sample F(Q) and reference data
void DUQ::setRMSEDeltaQ(double dq)
{
	rmseDeltaQ_ = dq;
}

// Set random seed
void DUQ::setSeed(int seed)
{
	seed_ = seed;
}

// Return random seed
int DUQ::seed()
{
	return seed_;
}

// Set window function to use for all Fourier transforms
void DUQ::setWindowFunction(Data2D::WindowFunction wf)
{
	windowFunction_ = wf;
}

// Setup all simulation data, checking it as we go
bool DUQ::setupSimulation()
{
	Messenger::print("\n");
	Messenger::print("*** SYSTEM SETUP BEGINS ***\n");

	/* Check each defined Species */
	Messenger::print("\n");
	Messenger::print("*** Checking Species definitions...\n");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		Messenger::print("--- Species '%s'...\n", sp->name());
		if (!sp->checkSetup(atomTypes_)) return false;
	}

	Messenger::print("\n");
	Messenger::print("*** Setting up Configurations...\n");
	int index = 0;
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next, ++index)
	{
		Messenger::print("*** Configuration %2i: '%s'\n", index, cfg->name());

		if (!cfg->setup(atomTypes_, pairPotentialRange_, boxNormalisationPoints_)) return false;
	}

	/* Pair Potentials */
	/* We expect a PairPotential to have been defined for every combination of AtomType used in the system */
	Messenger::print("\n");
	Messenger::print("Checking PairPotential definitions....TODO!!!!\n");
// 	for (RefListItem<AtomType,int>* ri = typeCount.first(); ri != NULL; ri = ri->next)
// 	{
// 		for (RefListItem<AtomType,int>* rj = ri; rj != NULL; rj = rj->next)
// 		{
// 			PairPotential* pot = hasPairPotential(ri->item, rj->item);
// 			if (pot == NULL)
// 			{
// 				Messenger::error("A PairPotential between AtomTypes '%s' and '%s' is required, but has not been defined.\n", ri->item->name(), rj->item->name());
// 				++nErrors;
// 			}
// 			else if ((ri->data == 0) || (rj->data == 0))
// 			{
// 				Messenger::error("A PairPotential between AtomTypes '%s' and '%s' has been defined, but is not required.\n", ri->item->name(), rj->item->name());
// 				++nErrors;
// 			}
// 		}
// 	}

	// We will construct a complete list of all AtomTypes used over all Samples used in all Configuration.
	// So, loop over Configurations and go through Isotopologues in each used Sample mixture, and set global AtomType indices in each Configuration
	pairPotentialAtomTypeIndex_.clear();
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		// Loop over the atomtypes used in this Configuration, adding them to the master list
		for (AtomTypeData* atd = cfg->usedAtomTypes(); atd != NULL; atd = atd->next) pairPotentialAtomTypeIndex_.add(atd->atomType(), NULL, atd->population());

		// Set global AtomType indices in the Configuration
		cfg->setGlobalAtomTypeIndices(pairPotentialAtomTypeIndex_);
	}

	Messenger::print("--> %i unique AtomTypes (disregarding isotopic substitutions) used over all configurations:\n", pairPotentialAtomTypeIndex_.nItems());

	// Complain about unused AtomTypes
	int nErrors = 0;
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next)
	{
		// If this AtomType is not in pairPotentialAtomTypeIndex_, then it is never used
		if (pairPotentialAtomTypeIndex_.indexOf(at) == -1)
		{
			Messenger::error("AtomType '%s' is defined but is not present in the system as it stands.\n", at->name());
			++nErrors;
		}
	}
	if (nErrors > 0) return false;

	// Calculate fractional populations
	pairPotentialAtomTypeIndex_.finalise();
	
	// Print AtomType information (excluding isotope information)
	pairPotentialAtomTypeIndex_.print();

	// Assign indices to atom types
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next) at->setIndex(pairPotentialAtomTypeIndex_.indexOf(at));

	// Create PairPotential matrix
	Messenger::print("--> Creating matrix (%ix%i)...\n", pairPotentialAtomTypeIndex_.nItems(), pairPotentialAtomTypeIndex_.nItems());
	if (!potentialMap_.initialise(pairPotentialAtomTypeIndex_, pairPotentials_, pairPotentialRange_)) return false;

	return true;
}

// Run Simulation
bool DUQ::go()
{
	/*
	 * This is the main program loop. We perform the following steps:
	 *
	 *  1)	For all configurations, run all Evolution modules
	 *  2)	For all configurations, run all Analysis modules
	 *  3)	RDF calculation?
	 */

	// TEST - Calculate energy of current system, partial data, write, and quit
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		totalEnergyTest(cfg);
		totalEnergy(cfg);
		intramolecularEnergy(cfg);
	}

	return true;
}
