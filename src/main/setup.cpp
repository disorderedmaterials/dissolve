/*
	*** dUQ - Setup
	*** src/main/setup.cpp
	Copyright T. Youngs 2012-2014

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
#include <string.h>

// Find configuration by name
Configuration* DUQ::findConfiguration(const char* name) const
{
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next) if (strcmp(name, cfg->name()) == 0) return cfg;

	return NULL;
}
	
// Return random seed
int DUQ::seed()
{
	return seed_;
}

// Setup all data, checking it as we go
bool DUQ::setup()
{
	msg.print("\n");
	msg.print("*** SYSTEM SETUP BEGINS ***\n");

	/* Check each defined Species */
	msg.print("\n");
	msg.print("*** Checking Species definitions...\n");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		msg.print("--- Species '%s'...\n", sp->name());
		if (!sp->checkSetup(atomTypes_)) return false;
	}

	msg.print("\n");
	msg.print("*** Setting up Configurations...\n");
	int index = 0;
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next, ++index)
	{
		msg.print("*** Configuration %2i: '%s'\n", index, cfg->name());

		if (!cfg->setup(atomTypes_, pairPotentialRange_, boxNormalisationPoints_)) return false;
	}

	/* Pair Potentials */
	/* We expect a PairPotential to have been defined for every combination of AtomType used in the system */
	msg.print("\n");
	msg.print("Checking PairPotential definitions....TODO!!!!\n");
// 	for (RefListItem<AtomType,int>* ri = typeCount.first(); ri != NULL; ri = ri->next)
// 	{
// 		for (RefListItem<AtomType,int>* rj = ri; rj != NULL; rj = rj->next)
// 		{
// 			PairPotential* pot = hasPairPotential(ri->item, rj->item);
// 			if (pot == NULL)
// 			{
// 				msg.error("A PairPotential between AtomTypes '%s' and '%s' is required, but has not been defined.\n", ri->item->name(), rj->item->name());
// 				++nErrors;
// 			}
// 			else if ((ri->data == 0) || (rj->data == 0))
// 			{
// 				msg.error("A PairPotential between AtomTypes '%s' and '%s' has been defined, but is not required.\n", ri->item->name(), rj->item->name());
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

	msg.print("--> %i unique AtomTypes (disregarding isotopic substitutions) used over all configurations:\n", pairPotentialAtomTypeIndex_.nItems());

	// Complain about unused AtomTypes
	int nErrors = 0;
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next)
	{
		// If this AtomType is not in pairPotentialAtomTypeIndex_, then it is never used
		if (pairPotentialAtomTypeIndex_.indexOf(at) == -1)
		{
			msg.error("AtomType '%s' is defined but is not present in the system as it stands.\n", at->name());
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
	msg.print("--> Creating matrix (%ix%i)...\n", pairPotentialAtomTypeIndex_.nItems(), pairPotentialAtomTypeIndex_.nItems());
	if (!potentialMap_.initialise(pairPotentialAtomTypeIndex_, pairPotentials_, pairPotentialRange_)) return false;

	return true;
}

// Print Full System Setup
bool DUQ::printSetup()
{
	msg.print("\n");
	msg.print("*** AtomTypes\n");

	msg.print("*** Species\n");
// 	for (Species* sp = species_.first(); sp != NULL; sp = sp->next) sp->print();
// 
// 	msg.print("*** S
// 	msg.print("Simulation Setup:\n");
// 	msg.print("%-40s = %s\n", "Bragg calculation", braggCalculationOn_ ? "On" : "Off");
// 	msg.print("%-40s = %9.3e (%s)\n", "Bragg calculation Q(max)", braggMaximumQ_, "1/Angstroms");
// 	msg.print("%-40s = %9.3e (%s)\n", "Delta(Q) to use in S(Q) calculation", qDelta_, "1/Angstroms");
// 	msg.print("%-40s = %9.3e (%s)\n", "Q(max) to use in S(Q) calculation", qMax_, "1/Angstroms");
// 	msg.print("%-40s = %9.3e\n", "Q-Dependent Broadening FWHM", qDependentFWHM_);
// 	msg.print("%-40s = %9.3e\n", "Q-Independent Broadening FWHM", qIndependentFWHM_);
// 	msg.print("%-40s = %9i\n", "Random Seed", seed_);
// 	msg.print("%-40s = %9.3e (%s)\n", "RDF Bin Width", rdfBinWidth_, "Angstroms");
// 	msg.print("%-40s = %9.3e (%s)\n", "RDF Range", rdfRange_, "Angstroms");
// 	msg.print("%-40s = %s\n", "RDF Calculation Method", rdfMethod(rdfMethod_));
// 	msg.print("%-40s = %9i\n", "Simplex NCycles", simplexNCycles_);
// 	msg.print("%-40s = %9i\n", "Simplex NMoves", simplexNMoves_);
// 	msg.print("%-40s = %9.3e (%s)\n", "Simplex Temperature", simplexTemperature_, "barns/sr/atom");
// 	msg.print("%-40s = %9.3e (%s)\n", "Simplex Tolerance", simplexTolerance_, "barns/sr/atom");
// 	msg.print("%-40s = %9.3e (%s)\n", "Simulation Temperature", temperature_, "K");

	return true;
}
