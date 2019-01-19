/*
	*** SanityCheck Module - Processing
	*** src/modules/sanitycheck/process.cpp
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

#include "modules/sanitycheck/sanitycheck.h"
#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "base/sysfunc.h"

// Run main processing
bool SanityCheckModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * This is a parallel routine.
	 * As much data as possible is checked over all processes, checking that the "everybody knows everything" ideal is true.
	 */

	// Basic checks on data from Dissolve
	int i = 0;
	for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next, ++i)
	{
		int j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
		{
			PairPotential* pp = dissolve.pairPotential(at1, at2);
			if (!pp)
			{
				Messenger::error("Failed to find PairPotential for AtomTypes '%s' and '%s'.\n", at1->name(), at2->name());
				return false;
			}

			// Check for equality
			Messenger::printVerbose("Sanity checking PairPotential %s-%s...\n", at1->name(), at2->name());
			if (!pp->uOriginal().equality(procPool)) return Messenger::error("Sanity check failed - PairPotential %s-%s uOriginal are not equal.\n", at1->name(), at2->name());
			if (!pp->uFull().equality(procPool)) return Messenger::error("Sanity check failed - PairPotential %s-%s uFull are not equal.\n", at1->name(), at2->name());
			if (!pp->uAdditional().equality(procPool)) return Messenger::error("Sanity check failed - PairPotential %s-%s uAdditional are not equal.\n", at1->name(), at2->name());
		}
	}

	// Loop over all Configurations
	ListIterator<Configuration> configIterator(dissolve.configurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		// TODO This is the most basic sanity checking we can do. Need to extend to bonds, angles, molecules etc.
		// Number of Atoms and atomic positions
		Messenger::printVerbose("Sanity checking Configuration %s atoms...\n", cfg->name());
		if (!procPool.equality(cfg->nAtoms())) return Messenger::error("Failed sanity check for Configuration '%s' nAtoms (%i).\n", cfg->name(), cfg->nAtoms());
		for (int n=0; n<cfg->nAtoms(); ++n)
		{
			Vec3<double> r = cfg->atom(n)->r();
			if (!procPool.equality(cfg->atom(n)->r())) return Messenger::error("Failed sanity check for Configuration '%s' atom position %i (%f %f %f).\n", cfg->name(), n, r.x, r.y, r.z);
		}

		// Module data within the Configuration
		Messenger::printVerbose("Sanity checking Configuration %s module data...\n", cfg->name());
		if (!cfg->moduleData().equality(procPool)) return Messenger::error("Failed sanity check for Configuration '%s' module data.\n", cfg->name());
	}

	// Processing module data
	Messenger::printVerbose("Sanity checking processing module data...\n");
	if (!dissolve.processingModuleData().equality(procPool)) return Messenger::error("Failed sanity check for processing module data.\n");

	Messenger::print("All checked data passed equality tests.\n");

	return true;
}

